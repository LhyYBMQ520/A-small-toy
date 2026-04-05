# 🕒 实时系统时钟 (REALTIME SYSTEM CLOCK)
一个基于本项目 **Python** 基准版本程序，用Go语言重新编写的终端时钟应用，以 **ASCII艺术字** 形式实时显示当前日期、星期和时间。

## 功能特性
- ✨ **实时显示** - 每0.5秒刷新一次，展示当前日期、星期和时间
- 🚫 **无闪烁刷新** - 采用 tview/tcell 差分绘制技术，彻底修复终端闪屏问题，替代传统清屏+全量重绘方式
- 🎨 **彩色艺术字** - 使用go-figure库生成ASCII艺术字
- 📍 **屏幕居中** - 水平和垂直双向居中显示，附带较低分辨率适配
- ⌨️ **快捷退出** - 按下 `Q`、`q` `CTRL + C` 或 `ESC` 键即可退出程序
- 🖥️ **跨平台** - 使用ANSI转义码，兼容各主流终端

## 安装
### 前置要求
- Go 1.26.1

### 安装依赖库
```bash
go mod download
```

### （备用）如果遇到依赖报错，执行这个命令自动修复、整理依赖
```bash
go mod tidy
```

## 使用方法
### 直接运行
```bash
go run main.go
```

### 编译后运行
```bash
go build
```

### 技术亮点
- **无闪烁刷新** - 使用 `tview` / `tcell` 的差分绘制刷新界面（不再每 500ms 全屏清屏）
- **按键捕获退出** - 使用 `tview.Application.SetInputCapture()` 捕获 `Q/q`、`ESC`、`Ctrl+C`
- **动态颜色** - 使用 `tview.TextView.SetDynamicColors(true)` + 颜色标签（如 `[blue]...[-]`）

### 注意事项

由于ASCII艺术字字体较大，如终端分辨率较低可能无法显示完整。
