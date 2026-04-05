# 使用 Alpine Linux (体积小)
FROM alpine:latest

# 安装 figlet 和编译工具
RUN apk add --no-cache figlet gcc musl-dev

# 创建字体目录并确保字体存在
RUN mkdir -p /usr/share/figlet/fonts

# 下载 standard 字体（确保字体文件存在）
RUN wget -O /usr/share/figlet/fonts/standard.flf \
    https://raw.githubusercontent.com/cmatsuoka/figlet/master/fonts/standard.flf

# 创建应用目录
WORKDIR /app

# 复制源代码
COPY app-linux.c .

# 编译程序
RUN gcc app-linux.c -o clock

# 设置环境变量
ENV TERM=xterm-256color

# 运行程序
CMD ["./clock"]