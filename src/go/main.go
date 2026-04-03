package main

import (
	"strings"
	"time"
	"image/color"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"github.com/common-nighthawk/go-figure"
)

// updateClock 函数：更新界面上的时间、日期、星期显示
// 参数 textContainer：存放文本的界面容器
func updateClock(textContainer *fyne.Container) {
	// 1. 获取当前系统时间
	now := time.Now()

	// 2. 把时间格式化成字符串
	currentTime := now.Format("15:04:05")       // 时:分:秒
	currentDate := now.Format("2006-01-02")    // 年-月-日
	currentWeek := now.Format("Monday")        // 星期几

	// 3. 使用 go-figure 库把普通文字转换成 ASCII 艺术字
	// ""：使用默认字体（效果最好，线条清晰），true：启用彩色
	timeAscii := figure.NewFigure(currentTime, "", true).String()
	dateAscii := figure.NewFigure(currentDate, "", true).String()
	weekAscii := figure.NewFigure(currentWeek, "", true).String()

	// 4. 拼接最终要显示在界面上的所有内容
	content := "===== SYSTEM DATE & TIME (TERMINAL MODE) =====\n\n"
	content += "[DATE]\n"      // 日期标题
	content += dateAscii       // 日期艺术字
	content += "[DAY]\n"       // 星期标题
	content += weekAscii       // 星期艺术字
	content += "[TIME]\n"      // 时间标题
	content += timeAscii       // 时间艺术字
	// 最后一行：显示更新时间戳（精确到毫秒）
	content += "\n[ SYSTEM STATUS ] Updated: " + now.Format("15:04:05.000")

	// 5. fyne.Do：安全地更新 GUI 界面（必须用这个方法，避免界面崩溃）
	fyne.Do(func() {
		// 清空容器里所有旧的文本内容
		textContainer.Objects = nil

		// 把长文本按换行符切割成一行一行的文字
		lines := strings.Split(content, "\n")

		// 遍历每一行文字，创建文本组件并添加到容器中
		for _, line := range lines {
			// 创建一行白色文本
			textLine := canvas.NewText(line, color.White)
			textLine.TextSize = 12 // 字体大小
			textLine.TextStyle = fyne.TextStyle{
				Monospace: true, // 等宽字体（终端风格）
				Bold:      true, // 加粗
			}
			// 把这行文字添加到界面容器里
			textContainer.Objects = append(textContainer.Objects, textLine)
		}

		// 刷新界面，让新内容显示出来
		textContainer.Refresh()
	})

	// 6. 每隔 100 毫秒调用一次自己，实现动态刷新时间
	time.AfterFunc(100*time.Millisecond, func() {
		updateClock(textContainer)
	})
}

// main 函数：程序入口，运行时第一个执行的函数
func main() {
	// 1. 创建一个新的 Fyne 应用程序
	myApp := app.New()

	// 2. 创建应用窗口，设置窗口标题
	myWindow := myApp.NewWindow("Terminal Clock - ASCII Art")

	// 3. 设置窗口大小：宽 900，高 600
	myWindow.Resize(fyne.NewSize(900, 600))

	// 4. 固定窗口大小，禁止用户缩放
	myWindow.SetFixedSize(true)

	// 5. 创建一个垂直布局容器，用来放所有文本行
	textBox := container.NewVBox()

	// 6. 创建一个纯黑色矩形作为背景
	blackBg := canvas.NewRectangle(color.NRGBA{R: 0, G: 0, B: 0, A: 255})

	// 7. 给文本容器添加滚动条（内容超出窗口时可以滚动查看）
	scrollContainer := container.NewScroll(textBox)

	// 8. 堆叠布局：把黑色背景和滚动容器叠在一起（背景+内容）
	contentContainer := container.NewStack(blackBg, scrollContainer)

	// 9. 把堆叠好的内容设置到窗口中
	myWindow.SetContent(contentContainer)

	// 10. 启动一个后台协程，开始刷新时钟
	go updateClock(textBox)

	// 11. 显示窗口，并让程序一直运行（阻塞等待关闭）
	myWindow.ShowAndRun()
}
