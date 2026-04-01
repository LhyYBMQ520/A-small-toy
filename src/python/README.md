# Terminal ASCII Clock
一个基于 Python + Tkinter 开发的**终端风格 ASCII 艺术数字时钟**，实时显示日期与时间。

## ✨ 效果预览
- 纯黑背景 + 绿色 ASCII 艺术字，模拟经典终端风格
- 实时刷新时间（100ms 精度）
- 显示：年-月-日 星期 + 时:分:秒
- 带系统更新时间戳状态栏
- 窗口固定大小，不可缩放/最大化

## 📦 依赖安装
本项目依赖 `pyfiglet` 生成 ASCII 艺术字，执行以下命令安装：
```bash
pip install -r requirements.txt
```

## 🚀 运行方式
1. 确保已安装 Python 3.x
2. 安装依赖库
3. 直接运行主程序：
```bash
python app.py
```

## 🛠 技术栈
- **Python 3**
- **Tkinter**（Python 内置 GUI 库，无需额外安装）
- **pyfiglet**（ASCII 艺术字生成）
- **datetime**（系统时间获取）
