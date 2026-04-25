// 引用Figgle库：用于生成控制台艺术字（ASCII Art）
using Figgle;
// 引用Terminal.Gui库：用于创建专业的控制台GUI界面
using Terminal.Gui;

// 时钟刷新间隔：500毫秒（0.5秒刷新一次时间）
const int refreshMilliseconds = 500;
// 进入终端【备用屏幕】：程序运行时不覆盖原终端内容，退出后恢复原界面
using var alternateScreen = AlternateScreenScope.TryEnter();

// ===================== 初始化Terminal.Gui应用 =====================
// 使用系统原生控制台（而非库自带的模拟控制台）
Application.UseSystemConsole = true;
// 初始化GUI应用程序
Application.Init();
// 隐藏控制台光标
Application.Driver.SetCursorVisibility(CursorVisibility.Invisible);
// 标准化Windows控制台缓冲区大小（避免滚动条、显示异常）
NormalizeConsoleBuffer();

try
{
    // 获取应用的顶层窗口（所有UI控件的容器）
    var top = Application.Top;
    // 创建核心的时钟视图控件
    var clockView = new ClockView
    {
        X = 0,          // 水平起始位置：最左侧
        Y = 0,          // 垂直起始位置：最顶部
        Width = Dim.Fill(),  // 宽度：填满整个窗口
        Height = Dim.Fill(), // 高度：填满整个窗口
        CanFocus = true     // 允许获取焦点（用于接收键盘按键）
    };

    // 将时钟视图添加到顶层窗口
    top.Add(clockView);
    // 让时钟视图默认获取焦点（确保键盘按键生效）
    clockView.SetFocus();

    // 绑定【窗口大小改变】事件
    Application.Resized += _ =>
    {
        // 重新适配控制台缓冲区
        NormalizeConsoleBuffer();
        // 标记时钟视图需要重绘（刷新界面）
        clockView.SetNeedsDisplay();
    };

    // 绑定【Ctrl+C】终止事件
    Console.CancelKeyPress += (_, e) =>
    {
        // 取消系统默认的终止行为
        e.Cancel = true;
        // 请求停止GUI应用（安全退出）
        Application.RequestStop();
    };

    // 主循环：添加定时任务（每500毫秒执行一次）
    Application.MainLoop.AddTimeout(TimeSpan.FromMilliseconds(refreshMilliseconds), _ =>
    {
        // 刷新时钟界面
        clockView.SetNeedsDisplay();
        // 返回true：表示定时任务持续执行（false则只执行一次）
        return true;
    });

    // 启动GUI应用主循环（阻塞运行，直到退出）
    Application.Run();
}
finally
{
    // 无论程序是否异常，最终都会关闭应用、释放资源
    Application.Shutdown();
}

// ===================== 辅助方法：标准化Windows控制台缓冲区 =====================
static void NormalizeConsoleBuffer()
{
    // 非Windows系统 或 控制台输出被重定向时，直接跳过
    if (!OperatingSystem.IsWindows() || Console.IsOutputRedirected)
    {
        return;
    }

    try
    {
        // 获取当前控制台窗口的宽高
        var windowWidth = Console.WindowWidth;
        var windowHeight = Console.WindowHeight;
        // 宽高非法时跳过
        if (windowWidth <= 0 || windowHeight <= 0)
        {
            return;
        }

        // 目标缓冲区大小 = 窗口大小
        var targetWidth = windowWidth;
        var targetHeight = windowHeight;

        // 如果当前缓冲区小于窗口大小，先扩大缓冲区
        if (Console.BufferWidth < targetWidth || Console.BufferHeight < targetHeight)
        {
            Console.SetBufferSize(
                Math.Max(Console.BufferWidth, targetWidth),
                Math.Max(Console.BufferHeight, targetHeight));
        }

        // 最终将缓冲区严格设置为窗口大小（消除滚动条）
        if (Console.BufferWidth != targetWidth || Console.BufferHeight != targetHeight)
        {
            Console.SetBufferSize(targetWidth, targetHeight);
        }
    }
    catch
    {
        // 部分终端（如ConPTY）不支持修改缓冲区，忽略异常
    }
}

// ===================== 自定义类：终端备用屏幕作用域（实现自动释放） =====================
internal sealed class AlternateScreenScope : IDisposable
{
    // 标记是否成功进入备用屏幕
    private readonly bool _entered;

    // 私有构造函数
    private AlternateScreenScope(bool entered)
    {
        _entered = entered;
    }

    // 尝试进入终端备用屏幕
    public static AlternateScreenScope TryEnter()
    {
        // 输出被重定向时，不进入备用屏幕
        if (Console.IsOutputRedirected)
        {
            return new AlternateScreenScope(false);
        }

        try
        {
            // ANSI转义序列：
            // \x1b[?1049h → 进入终端备用屏幕
            // \x1b[?25l → 隐藏光标
            Console.Write("\x1b[?1049h\x1b[?25l");
            // 刷新输出缓冲区
            Console.Out.Flush();
            return new AlternateScreenScope(true);
        }
        catch
        {
            // 失败则返回未进入状态
            return new AlternateScreenScope(false);
        }
    }

    // 释放资源：退出备用屏幕，恢复终端原始状态
    public void Dispose()
    {
        // 未进入备用屏幕则直接返回
        if (!_entered)
        {
            return;
        }

        try
        {
            // ANSI转义序列：
            // \x1b[?25h → 显示光标
            // \x1b[?1049l → 退出备用屏幕，回到主终端
            Console.Write("\x1b[?25h\x1b[?1049l");
            Console.Out.Flush();
        }
        catch
        {
            // 忽略终端恢复失败的异常
        }
    }
}

// ===================== 核心自定义控件：时钟视图 =====================
internal sealed class ClockView : View
{
    // Figgle字体渲染参数：紧凑模式（0=无间距）
    private const int FiggleFullWidthSmush = 0;

    // 定义四种UI颜色属性（前景色+背景色）
    private readonly Terminal.Gui.Attribute _headerAttr;  // 标题颜色
    private readonly Terminal.Gui.Attribute _labelAttr;   // 标签颜色
    private readonly Terminal.Gui.Attribute _artAttr;     // 艺术字颜色
    private readonly Terminal.Gui.Attribute _hintAttr;    // 提示文字颜色

    // 构造函数：初始化颜色主题
    public ClockView()
    {
        // 黑色背景，搭配不同前景色
        _headerAttr = Application.Driver.MakeAttribute(Color.BrightBlue, Color.Black);
        _labelAttr = Application.Driver.MakeAttribute(Color.BrightYellow, Color.Black);
        _artAttr = Application.Driver.MakeAttribute(Color.BrightGreen, Color.Black);
        _hintAttr = Application.Driver.MakeAttribute(Color.Gray, Color.Black);
    }

    // 处理键盘按键事件
    public override bool ProcessKey(KeyEvent keyEvent)
    {
        // 按下 ESC / Q / q / Ctrl+C 时退出程序
        if (keyEvent.Key == Key.Esc
            || keyEvent.Key == Key.Q
            || keyEvent.Key == Key.q
            || keyEvent.Key == (Key.CtrlMask | Key.C))
        {
            Application.RequestStop();
            return true; // 表示按键已处理
        }

        // 其他按键交给父类处理
        return base.ProcessKey(keyEvent);
    }

    // 重绘视图核心方法（界面刷新时自动调用）
    public override void Redraw(Rect bounds)
    {
        // 确保重绘时光标保持隐藏
        Application.Driver.SetCursorVisibility(CursorVisibility.Invisible);

        // 生成当前时间的时钟艺术字帧数据
        var frame = BuildFrame(DateTime.Now);
        // 获取屏幕宽高
        var screenW = Bounds.Width;
        var screenH = Bounds.Height;

        // 设置为提示文字颜色，清空屏幕
        Driver.SetAttribute(_hintAttr);
        ClearSafe(screenW, screenH);

        // 计算居中偏移：让时钟在窗口中水平/垂直居中
        var leftPad = screenW > frame.ContentWidth ? (screenW - frame.ContentWidth) / 2 : 0;
        var topPad = screenH > frame.ContentHeight ? (screenH - frame.ContentHeight) / 2 : 0;

        // 遍历所有渲染行，绘制到控制台
        for (var i = 0; i < frame.Lines.Count; i++)
        {
            var y = topPad + i;
            // 超出屏幕范围的行跳过
            if (y < 0 || y >= screenH)
            {
                continue;
            }

            var line = frame.Lines[i];
            var text = line.Text;

            // 计算水平绘制位置
            var x = leftPad;
            // 如果需要居中，重新计算X坐标
            if (line.CenterInBlock)
            {
                x = leftPad + Math.Max((frame.ContentWidth - text.Length) / 2, 0);
            }

            // 超出屏幕右侧，跳过
            if (x >= screenW)
            {
                continue;
            }

            // 超出屏幕左侧，裁剪文本
            if (x < 0)
            {
                var skip = -x;
                if (skip >= text.Length)
                {
                    continue;
                }
                text = text[skip..];
                x = 0;
            }

            // 避免在控制台最后一列绘制（防止自动换行/滚动）
            var remainingWidth = screenW - x - 1;
            if (remainingWidth <= 0)
            {
                continue;
            }

            // 文本超长时裁剪
            if (text.Length > remainingWidth)
            {
                text = text[..remainingWidth];
            }

            // 移动光标到指定位置
            Move(x, y);
            // 设置当前行的颜色
            Driver.SetAttribute(ToAttribute(line.LineColor));
            // 绘制文本
            Driver.AddStr(text);
        }
    }

    // 安全清空屏幕：避免宽高异常导致报错
    private static void ClearSafe(int screenW, int screenH)
    {
        if (screenW <= 0 || screenH <= 0)
        {
            return;
        }

        // 留最后一列，防止自动换行
        var fillWidth = Math.Max(screenW - 1, 0);
        if (fillWidth == 0)
        {
            return;
        }

        // 生成空白字符串，逐行清空
        var blank = new string(' ', fillWidth);
        for (var y = 0; y < screenH; y++)
        {
            Application.Driver.Move(0, y);
            Application.Driver.AddStr(blank);
        }
    }

    // 将自定义颜色枚举 转换为 Terminal.Gui 颜色属性
    private Terminal.Gui.Attribute ToAttribute(LineColor color)
    {
        return color switch
        {
            LineColor.Header => _headerAttr,
            LineColor.Label => _labelAttr,
            LineColor.Art => _artAttr,
            _ => _hintAttr,
        };
    }

    // 核心方法：根据当前时间，生成时钟的艺术字帧数据
    private static ClockFrame BuildFrame(DateTime now)
    {
        // 格式化时间字符串
        var dateStr = now.ToString("yyyy-MM-dd");
        var weekStr = now.DayOfWeek.ToString();
        var timeStr = now.ToString("HH:mm:ss");

        // 使用Figgle的Slant字体，生成艺术字
        var dateArt = TrimTrailingNewLine(FiggleFonts.Slant.Render(dateStr, FiggleFullWidthSmush));
        var weekArt = TrimTrailingNewLine(FiggleFonts.Slant.Render(weekStr, FiggleFullWidthSmush));
        var timeArt = TrimTrailingNewLine(FiggleFonts.Slant.Render(timeStr, FiggleFullWidthSmush));

        // 初始化渲染行列表
        var lines = new List<RenderLine>
        {
            // 标题行：居中、蓝色
            new("REALTIME SYSTEM CLOCK", LineColor.Header, true),
            // 空行（分隔）
            new(string.Empty, LineColor.Hint, false)
        };

        // 依次添加 日期、星期、时间 三个模块
        AppendSection(lines, "[DATE]", dateArt);
        AppendSection(lines, "[DAY]", weekArt);
        AppendSection(lines, "[TIME]", timeArt);
        // 底部提示文字
        lines.Add(new("Press 'Q' or 'ESC' to exit", LineColor.Hint, false));

        // 计算整个时钟的最大宽度
        var contentWidth = lines.Count == 0 ? 0 : lines.Max(l => l.Text.Length);
        // 返回完整的时钟帧
        return new ClockFrame(lines, contentWidth, lines.Count);
    }

    // 辅助方法：去除字符串末尾的换行符
    private static string TrimTrailingNewLine(string text)
    {
        return text.TrimEnd('\r', '\n');
    }

    // 辅助方法：拼接一个模块（标签+艺术字）
    private static void AppendSection(List<RenderLine> lines, string label, string art)
    {
        // 添加模块标签（如 [DATE]）
        lines.Add(new(label, LineColor.Label, false));

        // 将艺术字按行拆分，添加到渲染列表
        foreach (var line in art.Split('\n'))
        {
            lines.Add(new(line.TrimEnd('\r'), LineColor.Art, false));
        }

        // 添加空行分隔
        lines.Add(new(string.Empty, LineColor.Hint, false));
    }
}

// ===================== 枚举：定义文本颜色类型 =====================
internal enum LineColor
{
    Header,  // 标题
    Label,   // 模块标签
    Art,     // 艺术字
    Hint     // 提示文字
}

// ===================== 记录：存储单行渲染数据 =====================
internal sealed record RenderLine(
    string Text,        // 要绘制的文本
    LineColor LineColor,// 文本颜色
    bool CenterInBlock  // 是否居中
);

// ===================== 记录：存储整个时钟帧的渲染数据 =====================
internal sealed record ClockFrame(
    IReadOnlyList<RenderLine> Lines, // 所有行
    int ContentWidth,                // 总宽度
    int ContentHeight                // 总高度
);
