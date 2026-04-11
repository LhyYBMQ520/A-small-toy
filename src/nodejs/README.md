# Realtime System Date/Clock - 终端实时日期时钟

一个基于本项目 **Python** 基准版本，用Nodejs编写的终端时钟应用，以 **ASCII艺术字** 形式实时显示当前日期、星期和时间。

## ✨ 功能特性

- **实时更新**：每100毫秒刷新一次，显示精确到毫秒的时间
- **ASCII艺术字体**：使用figlet库生成精美的Slant风格字体
- **完整日期信息**：显示年月日、星期和时间
- **自适应布局**：自动适应终端窗口大小变化
- **彩色显示**：使用多种颜色增强视觉效果
- **性能优化**：智能防抖和并行处理，确保流畅运行
- **多终端支持**：兼容xterm-256color等多种终端类型
- **优雅退出**：多种退出方式，清理终端状态

## 🚀 快速开始

```bash
# 安装依赖
pnpm install

# 运行程序
pnpm start
```

## 📦 安装指南

### 系统要求

- Node.js 24（老版本未测试）
- 支持ANSI转义序列的终端（如：xterm, iTerm2, Windows Terminal等）

**依赖说明：**

- `pnpm`: 一个高性能的 Node.js 包管理器，替代npm
- `neo-blessed`: 用于创建终端UI界面的强大库
- `figlet`: 用于生成ASCII艺术字体

## 🎮 使用方法

运行程序后，您将看到一个全屏的终端界面，包含：

1. **顶部标题栏**：显示"REALTIME SYSTEM DATE/CLOCK"
2. **中央时钟区域**：
   - [DATE] 标签 + 日期（YYYY-MM-DD）
   - [DAY] 标签 + 星期（如：Monday）
   - [TIME] 标签 + 时间（HH:MM:SS）
3. **底部状态栏**：显示最后更新时间和退出提示

### 环境变量配置

```bash
# 设置终端类型（可选，默认为xterm-256color）
export TERM=xterm-256color
pnpm start
```

## ⚙️ 配置选项

您可以在代码中修改以下配置：

```javascript
// 字体配置
const FONT_CONFIG = {
    font: 'Slant',           // 字体风格
    horizontalLayout: 'full' // 水平布局
};

// 刷新间隔（毫秒）
const REFRESH_INTERVAL_MS = 100;

// 终端类型（从环境变量获取）
const TERM_FOR_BLESSED = process.env.TERM || 'xterm-256color';
```

### 可用字体

figlet支持多种字体，您可以修改`FONT_CONFIG.font`的值：

- `Slant`（默认）
- `Standard`
- `Banner`
- `Big`
- `Block`
- `Bubble`
- 等等...

## 🚪 退出方式

程序支持多种退出方式：

- 按 `q` 键
- 按 `Q` 键
- 按 `ESC` 键
- 按 `Ctrl+C`

退出时程序会自动：
- 恢复终端光标显示
- 清理终端界面
- 释放所有资源

## 🔧 技术栈

- **Node.js**: 运行时环境
- **neo-blessed**: 终端UI框架
- **figlet**: ASCII艺术字体生成器

## ⚡ 性能优化

### 1. 防抖锁机制

```javascript
let isUpdating = false;

async function updateClock() {
    if (isUpdating) return;
    isUpdating = true;
    // ... 更新逻辑
    isUpdating = false;
}
```

防止同时多次更新导致的资源竞争。

### 2. 并行处理

```javascript
const [dateAscii, dayAscii, timeAscii] = await Promise.all([
    figlet.text(dateStr, FONT_CONFIG),
    figlet.text(weekdayStr, FONT_CONFIG),
    figlet.text(timeStr, FONT_CONFIG)
]);
```

同时生成三个ASCII艺术字，提高渲染效率。

### 3. 智能渲染

```javascript
smartCSR: true,  // 启用智能内容变化检测
fullUnicode: true // 支持完整Unicode字符
```

只渲染变化的部分，减少不必要的重绘。

## ❓ 常见问题

### Q: 为什么时钟显示不完整/艺术字显示错误？

**A**: 可能是终端窗口太小。为了保证观感，程序去掉了滚动条，请确保终端窗口足够大以显示完整的ASCII艺术字。

### Q: 如何修改刷新频率？

**A**: 修改`REFRESH_INTERVAL_MS`常量的值。注意：刷新频率过高可能影响性能。

### Q: 支持Windows终端吗？

**A**: 是的，程序兼容Windows Terminal、PowerShell等现代终端。

### Q: 可以自定义颜色吗？

**A**: 可以！修改代码中的颜色标签，如`{green-fg}`、`{blue-fg}`等。

### Q: 为什么退出后光标消失了？

**A**: 正常情况下不会。如果发生这种情况，请检查是否有其他程序干扰了终端状态。
