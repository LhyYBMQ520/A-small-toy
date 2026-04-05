# ASCII码风格时钟--Windows

> 基于本项目Python版本衍生而来的C语言版本

## 功能特色

- 检测本机日期，星期和时间，以ASCII码风格显示
- 新增NTP时间获取(时区与用户相同),并计算NTP时间与本机时间差值
- 增加NTP服务器连通性Debug显示,包括IP地址和连通性结果

## 不足之处

- 由于C语言无法直接调用figlet库,故C语言项目使用时需要解压文件包(figlet主程序以及字体文件)
- 限制程序控制台大小，防止用户调整控制台大小时ASCII艺术字乱码
- 100ms刷新一次,有频闪现象
- 启动程序时,会占用鼠标直至用户主动退出

# ASCII码风格时钟--Linux

> Windows中的问题已在Linux-Docker版本中解决

## 运行要求

安装 Docker Desktop

https://www.docker.com/products/docker-desktop/

## 三种使用方式

### 方式一：从 Docker Hub 拉取运行

适用场景：有网络连接，需要快速获取最新版本。

用户运行命令：

`docker run --rm -it lucasandrew0120/terminal-clock:latest`

**Docker默认使用容器时间(无设置为零时区)，若想容器使用宿主机时间，请使用此命令**

`docker run --rm -it -v /etc/localtime:/etc/localtime:ro lucasandrew0120/terminal-clock:latest`

说明：首次运行会自动拉取镜像，之后直接启动。按 q 键退出程序。

### 方式二：从 tar 文件加载

适用场景：无网络环境，或通过 U 盘、网盘分发。

获取镜像文件：

`docker save -o terminal-clock.tar terminal-clock`

或使用压缩版本：

`docker save terminal-clock | gzip > terminal-clock.tar.gz`

用户加载镜像：

`docker load -i terminal-clock.tar`

或加载压缩版本：

`gunzip -c terminal-clock.tar.gz | docker load`

运行程序：

`docker run --rm -it terminal-clock:latest`

### 方式三：从源码构建

适用场景：需要修改代码，或信任源码而非预构建镜像。

准备文件：

- app-linux.c
- Dockerfile

构建镜像：

`docker build -t terminal-clock .`

运行程序：

`docker run --rm -it terminal-clock`

## 操作说明

按 q 键退出程序
按 ESC 键退出程序
支持窗口大小自适应，改变终端窗口大小时界面会自动调整

## 程序功能

显示当前日期、星期、时间（ASCII 艺术格式）
自动同步 NTP 时间
彩色终端输出
实时毫秒显示
NTP 时间差显示

## 常见问题

- 问题：运行后没有显示 ASCII 艺术字

解决方法：在容器内测试 figlet 命令

```
docker run --rm -it terminal-clock sh
figlet "TEST"
exit
```

- 问题：按 q 键无法退出

解决方法：确保使用 -it 参数运行

`docker run --rm -it terminal-clock`

问题：窗口大小改变后布局错乱

解决方法：程序已支持自动适应，重新调整窗口大小即可恢复

## 镜像信息

- 基础镜像：Alpine Linux
- 体积：约 200 MB
- 包含组件：figlet、gcc、C 标准库、网络库、字体文件 