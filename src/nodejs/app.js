const blessed = require('neo-blessed');
const figlet = require('figlet');

const FONT_CONFIG = {
    font: 'Slant',           // 使用 Slant 字体风格
    horizontalLayout: 'full' // 水平布局为完整宽度
};

const REFRESH_INTERVAL_MS = 100; // 时钟刷新间隔（毫秒），100ms = 0.1秒
// 从环境变量获取终端类型，如果没有则使用 xterm-256color 作为默认值
const TERM_FOR_BLESSED = process.env.TERM || 'xterm-256color';

// ==================== 终端控制序列 ====================
// ANSI 转义序列：隐藏终端光标
const HIDE_CURSOR = '\u001B[?25l';
// ANSI 转义序列：显示终端光标
const SHOW_CURSOR = '\u001B[?25h';
// ANSI 转义序列：清除屏幕内容和滚动缓冲区，并将光标移动到左上角
const CLEAR_SCREEN_AND_SCROLLBACK = '\u001B[2J\u001B[3J\u001B[H';

// ==================== 工具函数 ====================

function padZero(num, len = 2) {
    return String(num).padStart(len, '0');
}

function getClockText(now) {
    const year = now.getFullYear();
    const month = padZero(now.getMonth() + 1);
    const day = padZero(now.getDate());
    const dateStr = `${year}-${month}-${day}`;

    const weekdayStr = now.toLocaleDateString('en-US', { weekday: 'long' });

    const hours = padZero(now.getHours());
    const minutes = padZero(now.getMinutes());
    const seconds = padZero(now.getSeconds());
    const ms = padZero(now.getMilliseconds(), 3);
    const timeStr = `${hours}:${minutes}:${seconds}`;
    const timeWithMsStr = `${hours}:${minutes}:${seconds}.${ms}`;

    return { dateStr, weekdayStr, timeStr, timeWithMsStr };
}

// ==================== 初始化 neo-blessed 屏幕 ====================
const screen = blessed.screen({
    terminal: TERM_FOR_BLESSED,  // 指定终端类型
    smartCSR: true,              // 启用智能内容变化检测，优化渲染性能
    fullUnicode: true,           // 支持完整的 Unicode 字符
    title: 'Realtime System Date/Clock', // 设置终端窗口标题
    dockBorders: true,           // 自动处理边框
    cursor: {
        hidden: true,            // 在 blessed 层面隐藏光标
        shape: 'block'           // 光标形状为块状（虽然隐藏了，但设置形状）
    }
});

// ==================== UI 组件创建 ====================

// 1. 标题盒子 - 固定在顶部
const titleBox = blessed.box({
    parent: screen,              // 指定父容器为 screen
    top: 0,                      // 从顶部开始
    left: 0,                     // 从左侧开始
    width: '100%',               // 宽度占满整个屏幕
    height: 1,                   // 高度为1行
    tags: true,                  // 启用标签解析，支持 {color} 这样的语法
    align: 'center',             // 水平居中对齐
    valign: 'middle',            // 垂直居中对齐
    content: '{blue-fg}REALTIME SYSTEM DATE/CLOCK{/blue-fg}' // 蓝色前景色的标题
});

// 2. 时钟内容盒子 - 居中显示
const clockBox = blessed.box({
    parent: screen,
    top: 1,                      // 从标题下方开始（第1行）
    left: 0,                     // 从左侧开始
    width: '100%',               // 宽度占满整个屏幕
    height: '100%-2',            // 高度为屏幕高度减去2行（标题和页脚）
    tags: true,                  // 启用标签解析
    align: 'center',             // 水平居中
    valign: 'middle'             // 垂直居中
});

// 3. 页脚盒子 - 固定在底部
const footerBox = blessed.box({
    parent: screen,
    bottom: 0,                   // 固定在底部
    left: 0,                     // 从左侧开始
    width: '100%',               // 宽度占满整个屏幕
    height: 1,                   // 高度为1行
    tags: false,                 // 不启用标签解析（纯文本）
    align: 'center',             // 水平居中
    content: 'Press q / Q / Esc / Ctrl+C to exit' // 退出提示
});

// 将页脚置于最前面，确保其内容不被其他组件覆盖
footerBox.setFront();

// 防止同时多次更新的锁
let isUpdating = false;

// ==================== 终端光标管理 ====================

// 启动时立即隐藏光标（双重保险，blessed 和 ANSI 两层）
process.stdout.write(HIDE_CURSOR);

/**
 * 恢复终端光标显示
 * 在程序退出时调用，确保光标不会永久消失
 */
function restoreCursor() {
    process.stdout.write(SHOW_CURSOR);
}

/**
 * 清除屏幕和滚动缓冲区
 * 防止历史内容在终端滚动时显示
 */
function clearScreenAndScrollback() {
    if (process.stdout.isTTY) { // 检查是否是终端环境
        process.stdout.write(CLEAR_SCREEN_AND_SCROLLBACK);
    }
}

/**
 * 安全退出程序
 * 恢复光标、销毁屏幕、退出进程
 */
function exitProgram() {
    restoreCursor();
    screen.destroy(); // 释放 blessed 资源
    process.exit(0);
}

// ==================== 退出信号处理 ====================
// 进程退出时恢复光标
process.on('exit', restoreCursor);
// 捕获 Ctrl+C 信号
process.on('SIGINT', () => {
    restoreCursor();
    process.exit(0);
});
// 捕获终止信号
process.on('SIGTERM', () => {
    restoreCursor();
    process.exit(0);
});

// 首次启动时清理屏幕，避免旧内容残留
clearScreenAndScrollback();

// ==================== 核心时钟更新逻辑 ====================

/**
 * 更新时钟显示
 * 1. 获取当前时间
 * 2. 生成 ASCII 艺术字
 * 3. 更新 UI 组件内容
 * 4. 重新渲染屏幕
 */
async function updateClock() {
    // 防止同时多次更新，避免资源竞争
    if (isUpdating) {
        return;
    }
    isUpdating = true;

    try {
        const now = new Date();
        const { dateStr, weekdayStr, timeStr, timeWithMsStr } = getClockText(now);

        try {
            // 并行生成三个 ASCII 艺术字，提高性能
            const [dateAscii, dayAscii, timeAscii] = await Promise.all([
                figlet.text(dateStr, FONT_CONFIG),    // 生成日期 ASCII
                figlet.text(weekdayStr, FONT_CONFIG), // 生成星期 ASCII
                figlet.text(timeStr, FONT_CONFIG)     // 生成时间 ASCII
            ]);

            // 构建时钟内容，使用 blessed 的标签语法设置颜色
            const content = [
                '', // 空行作为顶部间距
                '{#FFA500-fg}[DATE]{/#FFA500-fg}', // 橙色的 [DATE] 标签
                `{green-fg}${dateAscii}{/green-fg}`, // 绿色的日期 ASCII
                '{#FFA500-fg}[DAY]{/#FFA500-fg}',   // 橙色的 [DAY] 标签
                `{green-fg}${dayAscii}{/green-fg}`,  // 绿色的星期 ASCII
                '{#FFA500-fg}[TIME]{/#FFA500-fg}',  // 橙色的 [TIME] 标签
                `{green-fg}${timeAscii}{/green-fg}`  // 绿色的时间 ASCII
            ].join('\n');

            // 更新时钟盒子内容
            clockBox.setContent(content);
            // 更新页脚内容，显示更新时间和退出提示
            footerBox.setContent(`Updated: ${timeWithMsStr} | Press q / Q / Esc / Ctrl+C to exit`);
            // 重新渲染整个屏幕
            screen.render();

        } catch (err) {
            // ASCII 生成失败时的错误处理
            footerBox.setContent(`生成失败: ${err.message}`);
            screen.render();
        }
    } finally {
        // 确保无论成功还是失败，都释放更新锁
        isUpdating = false;
    }
}

// ==================== 事件监听和程序启动 ====================

// 方案一：标准按键监听
// 监听 'q', 'escape', 'C-c' (Ctrl+C) 按键，触发退出
screen.key(['q', 'escape', 'C-c'], exitProgram);

// 方案二：使用 keypress 事件兜底
// 专门捕获大写 Q 和其他可能的退出键
screen.on('keypress', (ch, key) => {
    if (key && (key.name === 'q' || key.full === 'Q' || ch === 'Q')) {
        exitProgram();
    }
});

// 监听终端窗口大小变化事件
screen.on('resize', () => {
    // 窗口大小变化时清理屏幕，避免旧内容残留
    clearScreenAndScrollback();
    // 重新更新时钟显示
    updateClock();
});

// 立即执行一次更新，消除程序启动时的空白期
updateClock();

// 启动定时器，按照配置的间隔定期更新时钟
setInterval(updateClock, REFRESH_INTERVAL_MS);
