# 项目 PR 提交及开发规范
欢迎参与本项目的开发与贡献！请通过提交 Pull Request（PR）参与开发，并严格遵循以下步骤与要求。

---

## 一、PR 提交核心步骤

### 1. 克隆项目仓库
将远程仓库克隆到本地：
```bash
git clone https://github.com/LhyYBMQ520/A-small-toy.git
```

### 2. 创建专属开发目录
进入项目根目录，在 `src/` 下**创建你的专属文件夹**，命名必须严格遵守：
```
编程语言名-GitHubID
```
⚠️ **无空格！** 正确示例：
- `python-xxx123`
- `cpp-myUserName`
- `go-myID`

所有代码、测试文件**必须放在此目录内**，禁止修改其他目录文件。

### 3. 开发与测试
- 以 **Python 基准版本** 为标准，复刻功能与界面
- 确保程序**可正常运行、无卡顿、无崩溃**
- 代码整洁、注释清晰

### 4. 提交 PR
PR 标题与描述必须包含：
- 使用的编程语言
- 实现的功能说明
- 相较于基准版本修改的内容

示例：
```
新增：CPP 实现 - GitHubID: xxx123
实现基准版本的 ASCII艺术字 时钟实时刷新、日期时间显示、终端界面。
修改了XXXXX
```

---

## 二、构建文件提交要求（用于 Releases）
PR 通过后，请通过以下指定方式，提供可直接运行的构建文件（二进制文件或打包完成的可执行程序），便于我们整理并上传至 Releases 页面。

提交方式：

社交软件：仅限已添加我 QQ、WeChat 好友的开发者使用

邮箱：
        
- lhy20070708@qq.com
        
- lhyybmq@outlook.com
       
- lhyybmq@gmail.com

### 1. 构建文件命名规则
```
Terminal-ASCII-Clock-系统架构-编程语言
```
示例：
- Windows：`Terminal-ASCII-Clock-Win64-Python.exe`
- Linux：`Terminal-ASCII-Clock-linux-amd64-Python`
- macOS：`Terminal-ASCII-Clock-macos-arm64-Go`

### 2. 必须同步提供的信息
请在 PR 或留言中附上：
1. GitHub ID
2. 系统版本（如：Windows 11 25H2 / Debian13 amd64）
3. 运行环境版本（如：Python 3.13.9 / Go 1.22）

---

## 三、注意事项（必读）
- 严格遵守**目录命名、文件命名**规范，不规范将直接打回
- 禁止修改项目根目录、其他开发者目录、基准版本代码
- 功能须与 Python 基准版本保持大致一致
- 提交前必须本地测试通过，确保无明显 Bug
- 有疑问随时在 Issue 或评论区沟通

感谢你的贡献！期待你的 PR 🎉
