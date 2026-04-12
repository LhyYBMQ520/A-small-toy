# Terminal ASCII Clock - Java Edition
一个基于 **Java** + **jfiglet** 开发的**终端风格 ASCII 艺术数字时钟**，实时显示日期与时间。

## ✨ 效果预览
- 彩色 ASCII 艺术字（青色标题、橙色标签、绿色内容），模拟现代终端风格
- 实时刷新时间（100ms 精度）
- 显示：年-月-日 星期 + 时:分:秒
- 带系统更新时间戳状态栏（精确到毫秒）
- Windows 自动启用 ANSI 支持

## 🚀 快速开始

### 前置要求
- **Oracle OpenJDK 21.0.3** 或更高版本
- Gradle 8.14.2（项目内置 Gradle Wrapper，无需单独安装）

### 运行方式
``` bash
# 编译项目
gradlew.bat build

# 运行 JAR 包
java -jar Terminal-ASCII-Clock-1.0.jar
```


## 🛠 技术栈
- **Oracle OpenJDK 21.0.3**
- **Gradle Kotlin DSL**（构建工具）
- **jfiglet**（ASCII 艺术字生成库）

## ⚙️ 功能特性

### 1. 跨平台 ANSI 支持
程序会自动检测操作系统，在 Windows 上自动启用虚拟终端级别以支持 ANSI 颜色代码：
``` java
// Windows 自动执行注册表修改
reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f
```

### 2. 彩色输出
使用 RGB ANSI 颜色码实现丰富的色彩效果：
- **青色** (`#00FFFF`) - 标题栏
- **橙色** (`#FFA500`) - 标签（[DATE]、[DAY]、[TIME]）
- **绿色** (`#00FF00`) - ASCII 艺术字内容
- **灰色** (`#808080`) - 状态栏

### 3. 智能字符间距
`spaceOut()` 方法为每个字符添加空格，使 ASCII 艺术字更加美观：
``` java
"12:34:56" → "1 2 : 3 4 : 5 6"
```

### 4. 优雅退出
支持 `Ctrl+C` 中断程序，自动重置终端颜色：
``` java
catch (InterruptedException e) {
    Thread.currentThread().interrupt();
    System.out.println(ANSI_RESET); // 恢复默认颜色
}
```

## 🎨 自定义选项

### 修改刷新频率
编辑 `Main.java` 第 25 行：
``` java
Thread.sleep(100); // 改为其他值（ms）
```

### 修改颜色方案
编辑顶部的 ANSI 颜色常量：
``` java
private static final String ANSI_CYAN = "\u001B[38;2;0;255;255m";     // 青色
private static final String ANSI_ORANGE = "\u001B[38;2;255;165;0m";   // 橙色
private static final String ANSI_GREEN = "\u001B[38;2;0;255;0m";      // 绿色
```

### 修改日期格式
编辑 `updateTerminal()` 方法中的格式化器：
``` java
String currentTime = now.format(DateTimeFormatter.ofPattern("HH:mm:ss"));
String currentDate = now.format(DateTimeFormatter.ofPattern("yyyy-MM-dd"));
```

## ❓ 常见问题
### Q: Windows 终端不显示颜色？
**A**: 程序会自动尝试启用 ANSI 支持。如果仍然无效，请确保使用以下终端之一：
- Windows Terminal（推荐）
- PowerShell 5.1+
- cmd（Windows 10 1709+）

### Q: 如何更改 ASCII 字体样式？
**A**: 当前使用 jfiglet 默认字体。如需更换字体，需要：
1. 下载 `.flf` 字体文件
2. 放置到 `src/main/resources/` 目录
3. 修改代码加载指定字体文件

### Q: 编译时提示依赖错误？
**A**: 确保网络连接正常，Gradle 会自动下载 jfiglet 依赖。也可以手动同步：
``` bash
gradlew.bat --refresh-dependencies
```

### Q: ASCII 艺术字显示不完整？
**A**: 请扩大终端窗口尺寸，确保有足够的宽度和高度显示完整的艺术字。

## 📄 许可证
本项目遵循与主仓库相同的许可证协议。

---

**享受你的 ASCII 时钟之旅！** ⏰✨
