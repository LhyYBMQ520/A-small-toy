# Terminal ASCII Clock

基于本项目Python版本衍生而来的Go语言版本，使用ASCII艺术字显示实时系统日期、时间和星期信息，具有黑色仿终端风格界面。

## 功能特性

- ✨ **实时时钟显示**：动态更新系统时间，刷新频率为100ms
- 🎨 **ASCII艺术字**：使用go-figure库将文本创意性地展现为ASCII艺术字
- 🖥️ **仿终端界面**：黑色背景加白色文本的经典终端风格
- 📅 **多维度信息**：同时显示日期（年-月-日）、星期几和时间（时:分:秒）

## 系统要求

- **操作系统**：Windows / macOS / Linux
- **Go版本**：1.26.1

## 安装指南

### 1. 安装依赖

```bash
go mod download
```

或者重新生成依赖文件（如果go.mod损坏）：

```bash
go mod tidy
```

### 2. 编译应用

**开发运行**（直接运行）：
```bash
go run main.go
```

**生成可执行文件**：
```bash
go build -o terminal-clock.exe
```

或在Linux/macOS上：
```bash
go build -o terminal-clock
```

## 使用方法

### 运行应用

直接运行编译后的可执行文件或使用以下命令：

```bash
go run main.go
```

### 界面说明

应用启动后会显示一个900×600像素的固定大小窗口，内容包括：

```
===== SYSTEM DATE & TIME (TERMINAL MODE) =====

[DATE]
(ASCII艺术格式的日期)

[DAY]
(ASCII艺术格式的星期)

[TIME]
(ASCII艺术格式的时间)

[ SYSTEM STATUS ] Updated: 15:04:05.123
```

- 每100毫秒自动更新一次显示内容
- 窗口大小固定（900×600），无法调整大小
- 按下窗口关闭按钮可退出应用

## 技术架构

### 主要依赖

| 依赖库 | 版本 | 用途 |
|------|------|------|
| [fyne](https://fyne.io/) | v2.7.3 | 跨平台GUI框架 |
| [go-figure](https://github.com/common-nighthawk/go-figure) | latest | ASCII艺术字生成 |

### 核心函数

#### `updateClock(textContainer *fyne.Container)`

主要业务函数，负责：
1. 获取当前系统时间
2. 格式化时间、日期、星期信息
3. 使用go-figure库转换为ASCII艺术字
4. 使用`fyne.Do()`安全地更新GUI界面
5. 每100ms递归调用自己，实现动态刷新

#### `main()`

程序入口，负责：
1. 初始化Fyne应用
2. 创建固定大小的窗口
3. 设置黑色背景和文本容器
4. 启动后台协程执行`updateClock`
5. 显示窗口并保持运行

## 代码说明

### 关键设计点

- **线程安全**：使用`fyne.Do()`在主GUI线程中更新界面，避免竞态条件
- **高效刷新**：采用100ms刷新间隔，平衡实时性与性能
- **跨平台兼容**：使用Fyne框架确保在不同操作系统上的一致表现
- **模块化结构**：清晰的函数分工，易于维护和扩展

### 文本格式化

- 时间格式：`15:04:05`（24小时制）
- 日期格式：`2006-01-02`（年-月-日）
- 星期格式：`Monday`（英文星期名）
- 时间戳：精确到毫秒的系统更新时间

## 已知问题
艺术字渲染存在显示异常：在 Fyne GUI 中，字符 `---` 会被渲染为一条颜色极淡的细线，影响视觉效果。

该问题暂未修复，推测通过更换指定字体可能能够解决。

## 故障排除

### 问题1：运行时出现缺少依赖的错误

**解决方案**：
```bash
go mod download
go mod tidy
```

### 问题2：窗口无法正常显示

**解决方案**：
- 确保系统已安装必要的图形库支持
- 在Linux上可能需要安装：`libgl1-mesa-glx` 和 `libxcursor1`

### 问题3：时间显示不更新

**解决方案**：
- 检查系统时间设置是否正确
- 重启应用程序

## 许可证

请查看项目根目录的LICENSE文件

## 相关资源

- [Fyne官方文档](https://docs.fyne.io)
- [go-figure GitHub](https://github.com/common-nighthawk/go-figure)
- [Go官方文档](https://golang.org/doc)
