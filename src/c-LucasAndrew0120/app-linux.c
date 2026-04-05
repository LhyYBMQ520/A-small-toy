#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#ifdef _WIN32
// Windows 部分已移除
#else
#define CLEAR "clear"
#define sleep_ms(ms) usleep((ms) * 1000)
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

// 全局运行标志
int running = 1;

// 终端尺寸全局变量
int g_terminal_width = 120;
int g_terminal_height = 40;
int g_needs_full_redraw = 1;  // 需要完全重绘标志

// --- 全局变量 ---
char g_font[50] = "standard";
time_t g_ntp_timestamp = 0;
double g_time_difference = 0;
char g_ntp_status[100] = "Initializing...";
const char *g_ntp_server_ip = "203.107.6.88"; 

// --- 颜色定义 (ANSI)---
#define COLOR_CYAN   "\033[36m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_GRAY   "\033[90m"
#define COLOR_RESET  "\033[0m"

// 信号处理函数
void sigint_handler(int sig) {
    running = 0;
}

// 窗口大小改变信号处理
void sigwinch_handler(int sig) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    g_terminal_width = w.ws_col;
    g_terminal_height = w.ws_row;
    g_needs_full_redraw = 1;  // 标记需要完全重绘
    
    // 清屏
    printf("\033[2J");
    fflush(stdout);
}

// --- Linux 终端控制函数 ---
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y+1, x+1);
}

void set_color(const char* color) {
    printf("%s", color);
}

void hide_cursor() {
    printf("\033[?25l");
    fflush(stdout);
}

void show_cursor() {
    printf("\033[?25h");
    fflush(stdout);
}

// 检测是否有按键按下（无需回车）
int key_pressed() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

// --- NTP 相关功能 ---
void get_ntp_time() {
    unsigned char packet[48] = { 0x1B, 0 };
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(g_ntp_server_ip);
    server_addr.sin_port = htons(123);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock != INVALID_SOCKET) {
        struct timeval timeout = {2, 0};
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        if (sendto(sock, (char *)packet, 48, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) != -1) {
            if (recv(sock, (char *)packet, 48, 0) > 0) {
                unsigned long long high = (unsigned long long)packet[40] << 24 | 
                                          (unsigned long long)packet[41] << 16 |
                                          (unsigned long long)packet[42] << 8  | 
                                          (unsigned long long)packet[43];
                g_ntp_timestamp = (time_t)(high - 2208988800ull);
                g_time_difference = difftime(g_ntp_timestamp, time(NULL));
                snprintf(g_ntp_status, sizeof(g_ntp_status), "NTP Success (%s)", g_ntp_server_ip);
            } else {
                strcpy(g_ntp_status, "NTP Receive Timeout");
            }
        }
        closesocket(sock);
    }
}

// --- FIGlet 渲染函数 (使用系统 figlet)---
void figlet_render(const char* text, char* output, size_t output_size) {
    char command[512];
    
    // 使用系统 figlet 和正确的字体路径
    snprintf(command, sizeof(command), "figlet -f /usr/share/figlet/fonts/standard.flf \"%s\" 2>/dev/null", text);

    FILE* fp = popen(command, "r");
    if (fp == NULL) return;
    output[0] = '\0';
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        strncat(output, buffer, output_size - strlen(output) - 1);
        strncat(output, "\n", output_size - strlen(output) - 1);
    }
    pclose(fp);
}

void add_spaces(const char* src, char* dst) {
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        dst[j++] = src[i];
        if (src[i + 1] != '\0' && src[i] != ' ') dst[j++] = ' ';
    }
    dst[j] = '\0';
}

// 绘制表头
void draw_header() {
    gotoxy(0, 0);
    set_color(COLOR_CYAN);
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                    TERMINAL CLOCK - ASCII ART                                    ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}

int main() {
    // 设置信号处理
    signal(SIGINT, sigint_handler);
    signal(SIGWINCH, sigwinch_handler);  // 监听窗口大小改变
    
    // 保存原始终端设置
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // 关闭规范模式和回显，实现立即读取按键
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // 获取初始窗口大小
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    g_terminal_width = w.ws_col;
    g_terminal_height = w.ws_row;
    
    system(CLEAR);
    hide_cursor();

    get_ntp_time();
    int ntp_update_counter = 0;
    int last_second = -1;
    char lines[10][256];

    while (running) {
        // 检测按键（无需回车）
        if (key_pressed()) {
            char c;
            read(0, &c, 1);
            if (c == 'q' || c == 'Q' || c == 27) {  // q, Q, 或 ESC 退出
                break;
            }
        }
        
        // 如果需要完全重绘（窗口大小改变），重新绘制表头
        if (g_needs_full_redraw) {
            draw_header();  // 重新绘制表头
            last_second = -1;  // 强制重绘所有内容
            g_needs_full_redraw = 0;
        }

        time_t now = time(NULL);
        struct tm *st = localtime(&now);

        // --- 核心逻辑：只有秒数改变时，才重绘大型 ASCII 艺术 ---
        if (st->tm_sec != last_second) {
            last_second = st->tm_sec;

            // 每过 600 秒刷新一次 NTP
            if (++ntp_update_counter > 600) { get_ntp_time(); ntp_update_counter = 0; }

            char time_str[15], date_str[20], weekday_str[20], spaced_time[30];
            snprintf(time_str, 10, "%02d:%02d:%02d", st->tm_hour, st->tm_min, st->tm_sec);
            snprintf(date_str, 20, "%04d-%02d-%02d", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday);
            const char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
            snprintf(weekday_str, 20, "%s", days[st->tm_wday]);
            add_spaces(time_str, spaced_time);

            char date_ascii[4096] = {0}, time_ascii[4096] = {0}, weekday_ascii[4096] = {0};

            // 渲染并打印日期
            figlet_render(date_str, date_ascii, sizeof(date_ascii));
            int cnt = 0; 
            char *p = strtok(date_ascii, "\n");
            while (p && cnt < 8) { 
                strncpy(lines[cnt++], p, 255); 
                p = strtok(NULL, "\n"); 
            }
            for (int i = 0; i < 8; i++) {
                gotoxy(3, 6 + i); 
                set_color(COLOR_GREEN);
                printf("%-150s", i < cnt ? lines[i] : "");
            }

            // 渲染并打印星期
            figlet_render(weekday_str, weekday_ascii, sizeof(weekday_ascii));
            cnt = 0; 
            p = strtok(weekday_ascii, "\n");
            while (p && cnt < 8) { 
                strncpy(lines[cnt++], p, 255); 
                p = strtok(NULL, "\n"); 
            }
            for (int i = 0; i < 8; i++) {
                gotoxy(3, 14 + i); 
                set_color(COLOR_GREEN);
                printf("%-150s", i < cnt ? lines[i] : "");
            }

            // 渲染并打印时间
            figlet_render(spaced_time, time_ascii, sizeof(time_ascii));
            cnt = 0; 
            p = strtok(time_ascii, "\n");
            while (p && cnt < 8) { 
                strncpy(lines[cnt++], p, 255); 
                p = strtok(NULL, "\n"); 
            }
            for (int i = 0; i < 8; i++) {
                gotoxy(3, 22 + i); 
                set_color(COLOR_GREEN);
                printf("%-150s", i < cnt ? lines[i] : "");
            }
        }

        // --- 以下内容每一帧都刷新 ---
        gotoxy(0, 30);
        set_color(COLOR_GRAY);
        printf("└────────────────────────────────────────────────────────────────────────────────────────────────┘\n");

        char ntp_display[50] = "N/A";
        if (g_ntp_timestamp > 0) {
            time_t dynamic_ntp = time(NULL) + (time_t)g_time_difference;
            struct tm* ntp_tm = localtime(&dynamic_ntp);
            if(ntp_tm) strftime(ntp_display, sizeof(ntp_display), "%H:%M:%S", ntp_tm);
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm *current = localtime(&tv.tv_sec);
        
        gotoxy(3, 32);
        printf("[SYSTEM] %02d:%02d:%02d.%03ld  |  [NTP] %s  |  [DIFF] %+.1fs  |  [Q] to exit",
               current->tm_hour, current->tm_min, current->tm_sec, tv.tv_usec/1000, 
               ntp_display, g_time_difference);
        
        gotoxy(3, 33);
        printf("[DEBUG] NTP Status: %-80s", g_ntp_status);

        set_color(COLOR_RESET);
        fflush(stdout);
        sleep_ms(10);
    }

    // 恢复终端设置
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    // 退出恢复
    set_color(COLOR_RESET);
    show_cursor();
    printf("\n");
    printf("Goodbye!\n");
    
    return 0;
}