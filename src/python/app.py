import tkinter as tk
from tkinter import ttk
from datetime import datetime
# 用于生成ASCII艺术字（无需额外安装）
from pyfiglet import Figlet

def update_time():
    """实时更新时间和日期"""
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    current_date = now.strftime("%Y-%m-%d %A")

    # 在字符之间加入空格，避免如“11”这类字符在艺术字中粘连
    spaced_time = " ".join(current_time)
    spaced_date = " ".join(current_date)
    
    # 生成ASCII艺术字
    f = Figlet(font='standard')
    time_ascii = f.renderText(spaced_time)
    date_ascii = f.renderText(spaced_date)
    
    # 清空文本框并插入新内容
    text_box.delete(1.0, tk.END)
    # 插入终端标题
    text_box.insert(tk.END, "===== SYSTEM DATE & TIME (TERMINAL MODE) =====\n\n", "title")
    # 插入日期
    text_box.insert(tk.END, "[DATE]\n", "label")
    text_box.insert(tk.END, date_ascii, "ascii")
    # 插入时间
    text_box.insert(tk.END, "[TIME]\n", "label")
    text_box.insert(tk.END, time_ascii, "ascii")
    text_box.insert(tk.END, f"\n[ SYSTEM STATUS ] Updated: {now.strftime('%H:%M:%S.%f')[:-3]}", "status")
    
    # 每100ms刷新一次
    root.after(100, update_time)

# 主窗口配置
root = tk.Tk()
root.title("Terminal Clock - ASCII Art")
root.geometry("800x600")  # 窗口大小
root.resizable(False, False)  # 禁用调整窗口大小和最大化
root.configure(bg="black") # 纯黑背景

# 终端风格文本框
text_box = tk.Text(
    root,
    bg="black",        # 背景黑色
    fg="#00FF00",      # 字体绿色（终端绿）
    font=("Consolas", 12),  # 等宽字体（终端专用） - 改成 12 字号，可调整数字
    insertbackground="black", # 隐藏光标
    wrap=tk.WORD,
    relief=tk.FLAT,    # 无边框
    padx=20,
    pady=20
)
text_box.pack(fill=tk.BOTH, expand=True)

# 定义文本样式（仿终端高亮）
text_box.tag_configure("title", foreground="#00FFFF")  # 青色标题
text_box.tag_configure("label", foreground="#FFA500") # 橙色标签
text_box.tag_configure("ascii", foreground="#00FF00") # 绿色ASCII字
text_box.tag_configure("status", foreground="#808080") # 灰色状态

# 启动刷新
update_time()

# 运行主循环
root.mainloop()
