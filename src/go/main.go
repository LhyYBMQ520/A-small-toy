package main

import (
	"strings"    // 字符串处理工具
	"sync"       // 同步原语，用于保证退出操作只执行一次
	"time"       // 时间相关操作，用于获取系统时间、定时刷新

	// 第三方库：生成ASCII艺术字
	"github.com/common-nighthawk/go-figure"
	// 第三方库：终端底层屏幕控制
	"github.com/gdamore/tcell/v2"
	// 第三方库：终端UI组件库，用于构建界面
	"github.com/rivo/tview"
)

func main() {
	// ASCII艺术字的字体样式
	const font = "slant"

	// ==================== 1. 初始化应用与核心组件 ====================
	// 创建tview应用实例，管理整个终端UI
	app := tview.NewApplication()
	// 定义内容区域的宽高变量，用于动态适配界面
	contentHeight := 0 // 内容高度（行数）
	contentWidth := 0  // 内容宽度（字符数）

	// 创建文本展示组件：标题单独居中，其余内容保持左对齐（防止艺术字错位）
	headerView := tview.NewTextView().
		SetDynamicColors(true).          // 启用动态颜色（支持tview的颜色标签）
		SetTextAlign(tview.AlignCenter). // 仅标题行居中
		SetWrap(false)                   // 禁用自动换行

	bodyView := tview.NewTextView().
		SetDynamicColors(true).        // 启用动态颜色
		SetTextAlign(tview.AlignLeft). // 正文左对齐（防止艺术字错位）
		SetWrap(false)                 // 禁用自动换行，保证艺术字完整显示

	// ==================== 2. 布局设计：实现屏幕居中显示 ====================
	// 创建四个空白占位组件，用于实现内容居中（上下左右留白）
	topPad := tview.NewBox()    // 顶部留白
	bottomPad := tview.NewBox() // 底部留白
	leftPad := tview.NewBox()  // 左侧留白
	rightPad := tview.NewBox() // 右侧留白

	// 内容容器：标题 + 正文
	contentContainer := tview.NewFlex().
		SetDirection(tview.FlexRow).
		AddItem(headerView, 1, 0, false). // 标题固定 1 行
		AddItem(bodyView, 0, 1, true)     // 正文占剩余空间

	// 垂直方向布局：顶部留白 + 内容容器 + 底部留白
	// FlexRow：垂直排列组件
	vertical := tview.NewFlex().
		SetDirection(tview.FlexRow).
		AddItem(topPad, 0, 1, false).    // 顶部占位：权重1，不可聚焦
		AddItem(contentContainer, 0, 1, true).// 内容容器：权重1，可聚焦
		AddItem(bottomPad, 0, 1, false)  // 底部占位：权重1，不可聚焦

	// 根布局：水平方向 + 垂直布局嵌套
	// FlexColumn：水平排列组件
	root := tview.NewFlex().
		SetDirection(tview.FlexColumn).
		AddItem(leftPad, 0, 1, false).  // 左侧占位：权重1
		AddItem(vertical, 0, 1, true).  // 嵌套垂直布局：权重1
		AddItem(rightPad, 0, 1, false)  // 右侧占位：权重1

	// ==================== 3. 程序退出控制 ====================
	// 通道：用于通知协程停止运行
	done := make(chan struct{})
	// 同步工具：保证退出函数只执行一次，避免重复关闭通道
	var stopOnce sync.Once
	// stop 退出程序的封装函数
	stop := func() {
		stopOnce.Do(func() {
			close(done)    // 关闭通道，终止定时刷新协程
			app.Stop()     // 停止UI应用，退出终端界面
		})
	}

	// 设置按键捕获：监听退出快捷键
	app.SetInputCapture(func(event *tcell.EventKey) *tcell.EventKey {
		switch {
		// 按下 ESC / Ctrl+C / Q / q 任意一个，触发退出
		case event.Key() == tcell.KeyEsc,
			event.Key() == tcell.KeyCtrlC,
			event.Rune() == 'q',
			event.Rune() == 'Q':
			stop()
			return nil // 拦截事件，不传递给其他组件
		default:
			return event // 其他按键正常传递
		}
	})

	// ==================== 4. 内容生成：构建时钟艺术字 ====================
	// buildContent 根据当前时间，生成带颜色的ASCII艺术字内容
	// 返回值：标题行、正文内容、内容总行数
	buildContent := func(now time.Time) (string, string, int) {
		// 格式化时间为字符串
		dateStr := now.Format("2006-01-02") // 日期：年-月-日
		weekStr := now.Weekday().String()   // 星期
		timeStr := now.Format("15:04:05")   // 时间：时:分:秒

		// 生成ASCII艺术字，并去除末尾多余换行符
		dateArt := strings.TrimSuffix(figure.NewFigure(dateStr, font, true).String(), "\n")
		weekArt := strings.TrimSuffix(figure.NewFigure(weekStr, font, true).String(), "\n")
		timeArt := strings.TrimSuffix(figure.NewFigure(timeStr, font, true).String(), "\n")

		// 转义标签，防止tview解析为颜色指令
		dateLabel := tview.Escape("[DATE]")
		dayLabel := tview.Escape("[DAY]")
		timeLabel := tview.Escape("[TIME]")

		// 标题行单独返回，用 headerView 居中显示
		headerText := "[blue]🕒  REALTIME SYSTEM CLOCK  🕒[-]"

		// 正文内容保持左对齐，但整体块会通过左右留白实现“终端居中”
		bodyText := "\n" + strings.Join([]string{
			"[yellow]" + dateLabel + "[-]",
			"[green]" + dateArt + "[-]",
			"[yellow]" + dayLabel + "[-]",
			"[green]" + weekArt + "[-]",
			"[yellow]" + timeLabel + "[-]",
			"[green]" + timeArt + "[-]",
			"Press 'Q' or 'ESC' to exit",
		}, "\n\n") // 用两个换行分隔内容，提升美观度

		fullText := headerText + "\n" + bodyText
		return headerText, bodyText, len(strings.Split(fullText, "\n"))
	}

	// maxTaggedWidth 计算带颜色标签的文本的最大显示宽度
	maxTaggedWidth := func(text string) int {
		maxW := 0
		// 遍历每一行文本
		for _, line := range strings.Split(text, "\n") {
			// 计算去除标签后的实际显示宽度
			w := tview.TaggedStringWidth(line)
			if w > maxW {
				maxW = w
			}
		}
		return maxW
	}

	// ==================== 5. 屏幕绘制前回调：动态适配居中 ====================
	app.SetBeforeDrawFunc(func(screen tcell.Screen) bool {
		// 获取终端屏幕的宽高
		screenW, screenH := screen.Size()

		// 垂直方向适配：屏幕足够大时，固定内容高度，上下留白均分
		if screenH > 0 {
			if screenH > contentHeight && contentHeight > 0 {
				// 屏幕高度 > 内容高度：启用留白，垂直居中
				vertical.ResizeItem(contentContainer, contentHeight, 0)
				vertical.ResizeItem(topPad, 0, 1)
				vertical.ResizeItem(bottomPad, 0, 1)
			} else {
				// 屏幕过小：取消留白，内容占满垂直空间
				vertical.ResizeItem(contentContainer, 0, 1)
				vertical.ResizeItem(topPad, 0, 0)
				vertical.ResizeItem(bottomPad, 0, 0)
			}
		}

		// 水平方向适配：屏幕足够大时，固定内容宽度，左右留白均分
		if screenW > 0 {
			if screenW > contentWidth && contentWidth > 0 {
				// 屏幕宽度 > 内容宽度：启用留白，水平居中
				root.ResizeItem(vertical, contentWidth, 0)
				root.ResizeItem(leftPad, 0, 1)
				root.ResizeItem(rightPad, 0, 1)
			} else {
				// 屏幕过小：取消留白，内容占满水平空间
				root.ResizeItem(vertical, 0, 1)
				root.ResizeItem(leftPad, 0, 0)
				root.ResizeItem(rightPad, 0, 0)
			}
		}

		return false // 返回false，继续执行默认绘制逻辑
	})

	// ==================== 6. 更新UI函数 ====================
	// update 更新时间、内容宽高，并刷新界面
	update := func(now time.Time) {
		headerText, bodyText, lines := buildContent(now) // 生成新的时钟内容
		contentHeight = lines                          // 更新内容高度
		bodyW := maxTaggedWidth(bodyText)
		headerW := tview.TaggedStringWidth(headerText)
		if headerW > bodyW {
			contentWidth = headerW
		} else {
			contentWidth = bodyW
		}
		headerView.SetText(headerText)
		bodyView.SetText(bodyText)
	}

	// ==================== 7. 初始化渲染 ====================
	// 程序启动时，先渲染一次初始时间
	initialHeader, initialBody, initialLines := buildContent(time.Now())
	headerView.SetText(initialHeader)
	bodyView.SetText(initialBody)
	contentHeight = initialLines
	bodyW := maxTaggedWidth(initialBody)
	headerW := tview.TaggedStringWidth(initialHeader)
	if headerW > bodyW {
		contentWidth = headerW
	} else {
		contentWidth = bodyW
	}

	// ==================== 8. 定时刷新协程 ====================
	// 创建定时器：每500毫秒刷新一次时间
	ticker := time.NewTicker(500 * time.Millisecond)
	defer ticker.Stop() // 程序退出时关闭定时器

	// 启动后台协程，定时更新UI
	go func() {
		for {
			select {
			case <-done:
				// 收到退出信号，终止协程
				return
			case now := <-ticker.C:
				// 定时器触发，安全更新UI（tview要求UI操作必须在主线程）
				app.QueueUpdateDraw(func() {
					update(now)
				})
			}
		}
	}()

	// ==================== 9. 启动应用 ====================
	// 设置根布局，启用全屏，运行UI循环
	if err := app.SetRoot(root, true).EnableMouse(false).Run(); err != nil {
		// 运行失败直接返回，tview会自动处理终端恢复
		return
	}
}
