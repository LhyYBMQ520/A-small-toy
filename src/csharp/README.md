# 🕒 实时系统时钟 (REALTIME SYSTEM CLOCK)
一个基于本项目 **Python** 基准版本程序，使用 **C#** 语言 **.NET** 开发的**跨平台彩色实时控制台时钟**，以 **ASCII艺术字** 形式实时显示当前日期、星期和时间。

## ✨ 功能特性
- 🎨 **彩色艺术字展示**：使用 Figgle 生成美观的 ASCII 时钟样式
- 🕒 **实时刷新**：500ms 自动更新时间，精准同步系统时间
- 🖥️ **跨平台兼容**：完美支持 Windows、Linux 系统
- 📺 **全屏自适应**：窗口大小变化自动居中，通过TUI实现无闪烁渲染
- ⌨️ **便捷退出**：支持 `Q` / `ESC` / `Ctrl+C` 快速退出程序
- 🧹 **安全终端**：运行不污染原终端界面，退出自动恢复

## 🛠️ 环境要求
- **.NET 10.0** SDK / Runtime（项目目标框架）
- 支持 ANSI 转义序列的标准终端（Windows Terminal / GNOME Terminal 等）

## ⌨️ 操作快捷键
| 按键 | 功能 |
|------|------|
| `Q` / `q` | 退出程序 |
| `ESC` | 退出程序 |
| `Ctrl + C` | 退出程序 |

## 🧩 核心依赖
| 库 | 用途 |
|------|------|
| [Terminal.Gui](https://github.com/gui-cs/Terminal.Gui) | 控制台 GUI 框架，负责界面渲染、事件处理 |
| [Figgle](https://github.com/drewnoakes/figgle) | 生成控制台 ASCII 艺术字 |

## 📝 备注
- 程序运行时会进入终端备用屏幕，退出后自动恢复原终端内容
- 渲染优化：无闪烁、自适应窗口、边界安全裁剪
- 已关闭 Native AOT 以保证跨平台编译
