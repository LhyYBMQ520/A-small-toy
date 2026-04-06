# ASCII 艺术字实时时钟

一个基于本项目Python基准版本，用 HTML +CSS + JavaScript 编写的纯前端网页：

- 以 ASCII 艺术字显示日期、星期和时间
- 支持毫秒模式/普通模式一键切换
- 页面默认黑底终端风格，适合全屏展示

## 功能说明

- 日期显示：格式为 `YYYY-MM-DD`
- 星期显示：英文全称（如 `Monday`）
- 时间显示：
	- 毫秒模式：`HH:mm:ss.SSS`
	- 普通模式：`HH:mm:ss`
- 右上角按钮：`显示/隐藏毫秒`
	- 点击后切换模式
	- 普通模式刷新频率为 500ms
	- 毫秒模式刷新频率为 100ms

## 技术栈

- HTML5
- CSS3
- 原生 JavaScript
- [figlet.js](https://github.com/patorjk/figlet.js)（通过 CDN 引入）
- 本地字体文件：`fonts/Slant.flf`

## 本地运行注意事项

请使用本地静态服务器打开html文件，如使用 VS Code 的 "Live Server" 插件（推荐）。本地直接双击打开 HTML 文件会使用 `file://` 协议，受浏览器安全策略限制，无法加载本地外部的字体资源，导致艺术字生成失败！
