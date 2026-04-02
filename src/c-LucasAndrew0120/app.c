#include <winsock2.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h> // 必须包含，用于切换工作目录

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#define CLEAR "cls"
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#define CLEAR "clear"
#define sleep_ms(ms) usleep((ms) * 1000)
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

// --- 全局变量 ---
char g_font[50] = "standard";
time_t g_ntp_timestamp = 0;
double g_time_difference = 0;
char g_ntp_status[100] = "Initializing...";
const char *g_ntp_server_ip = "203.107.6.88"; 

#ifdef _WIN32
HANDLE g_hConsole;
#endif

// --- 颜色定义 ---
#define COLOR_CYAN 11
#define COLOR_GREEN 10
#define COLOR_GRAY 8
#define COLOR_RESET 7

// --- 工具函数 ---
void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

void hide_cursor() {
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

// --- NTP 相关功能 ---
void get_ntp_time() {
    unsigned char packet[48] = { 0x1B, 0 };
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(g_ntp_server_ip);
    server_addr.sin_port = htons(123);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return;

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock != INVALID_SOCKET) {
        int timeout = 2000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        if (sendto(sock, (char *)packet, 48, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) != -1) {
            if (recv(sock, (char *)packet, 48, 0) > 0) {
                unsigned long long high = (unsigned long long)packet[40] << 24 | (unsigned long long)packet[41] << 16 |
                                          (unsigned long long)packet[42] << 8  | (unsigned long long)packet[43];
                g_ntp_timestamp = (time_t)(high - 2208988800ull);
                g_time_difference = difftime(g_ntp_timestamp, time(NULL));
                snprintf(g_ntp_status, sizeof(g_ntp_status), "NTP Success (%s)", g_ntp_server_ip);
            } else {
                strcpy(g_ntp_status, "NTP Receive Timeout");
            }
        }
        closesocket(sock);
    }
    WSACleanup();
}

// --- FIGlet 渲染函数 ---
void figlet_render(const char* text, char* output, size_t output_size) {
    char command[512];
    // 配合 main 中的 _chdir，直接使用当前目录下的 figlet.exe 和字体
    snprintf(command, sizeof(command), "figlet.exe -f \"standard.flf\" \"%s\" 2>nul", text);

    FILE* fp = popen(command, "r");
    if (fp == NULL) return;
    output[0] = '\0';
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) 
        strncat(output, buffer, output_size - strlen(output) - 1);
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

int main() {
    #ifdef _WIN32
        // 补丁：强制关闭自动换行，防止字符折叠重叠
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT; // 禁用自动换行
        SetConsoleMode(hOut, dwMode);

        // 补丁：将缓冲区设得足够大
        COORD bufSize = { 200, 100 }; 
        SetConsoleScreenBufferSize(hOut, bufSize);
    #endif
    // 1. 设置编码与光标
    system("chcp 65001 > nul");
    hide_cursor();

    // 2. 核心路径对齐：确保 figlet 能够运行
    char base_path[MAX_PATH];
    GetModuleFileNameA(NULL, base_path, MAX_PATH);
    char *last_slash = strrchr(base_path, '\\');
    if (last_slash) {
        *last_slash = '\0';
        _chdir(base_path); // 切换到程序所在目录
    }

    // 3. 绘制静态 UI 标题
    system(CLEAR);
    set_color(COLOR_CYAN);
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                    TERMINAL CLOCK - ASCII ART                                    ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════════╝\n");

    get_ntp_time();
    int ntp_update_counter = 0;
    char date_ascii[8192], time_ascii[8192], weekday_ascii[8192];
    char lines[10][256];

    while (1) {
        if (_kbhit()) { int ch = _getch(); if (ch == 27 || ch == 'q' || ch == 'Q') break; }

        // 每分钟更新一次 NTP
        if (++ntp_update_counter > 600) { get_ntp_time(); ntp_update_counter = 0; }

        // 获取本地时间
        SYSTEMTIME st;
        GetLocalTime(&st);
        char time_str[15], date_str[20], weekday_str[20], ms_str[5], spaced_time[30];
        
        snprintf(time_str, 10, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
        snprintf(date_str, 20, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
        snprintf(ms_str, 5, "%03d", st.wMilliseconds);
        const char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
        snprintf(weekday_str, 20, "%s", days[st.wDayOfWeek]);
        add_spaces(time_str, spaced_time);

        // --- 绘制日期 (Y轴 6) ---
        figlet_render(date_str, date_ascii, sizeof(date_ascii));
        int cnt = 0; char *p = strtok(date_ascii, "\n");
        while (p && cnt < 8) { strcpy(lines[cnt++], p); p = strtok(NULL, "\n"); }
        for (int i = 0; i < 8; i++) {
            gotoxy(3, 6 + i); set_color(COLOR_GREEN);
            printf("%-120s", i < cnt ? lines[i] : "");
        }

        // --- 绘制星期 (Y轴 14) ---
        figlet_render(weekday_str, weekday_ascii, sizeof(weekday_ascii));
        cnt = 0; p = strtok(weekday_ascii, "\n");
        while (p && cnt < 8) { strcpy(lines[cnt++], p); p = strtok(NULL, "\n"); }
        for (int i = 0; i < 8; i++) {
            gotoxy(3, 14 + i); set_color(COLOR_GREEN);
            printf("%-120s", i < cnt ? lines[i] : "");
        }

        // --- 绘制时间 (Y轴 22) ---
        figlet_render(spaced_time, time_ascii, sizeof(time_ascii));
        cnt = 0; p = strtok(time_ascii, "\n");
        while (p && cnt < 8) { strcpy(lines[cnt++], p); p = strtok(NULL, "\n"); }
        for (int i = 0; i < 8; i++) {
            gotoxy(3, 22 + i); set_color(COLOR_GREEN);
            printf("%-120s", i < cnt ? lines[i] : "");
        }

        // --- 绘制底部装饰线 ---
        gotoxy(0, 30);
        set_color(COLOR_GRAY);
        printf("└────────────────────────────────────────────────────────────────────────────────────────────────┘\n");

        // --- 计算并绘制 NTP 时间差异 ---
        char ntp_display[50] = "N/A";
        if (g_ntp_timestamp > 0) {
            time_t dynamic_ntp = time(NULL) + (time_t)g_time_difference;
            struct tm* ntp_tm = localtime(&dynamic_ntp);
            strftime(ntp_display, sizeof(ntp_display), "%H:%M:%S", ntp_tm);
        }

        gotoxy(3, 32);
        printf("[SYSTEM] %s.%s  |  [NTP] %s  |  [DIFF] %+.1fs  |  ESC/Q to exit",
               time_str, ms_str, ntp_display, g_time_difference);
        
        gotoxy(3, 33);
        printf("[DEBUG] NTP Status: %-80s", g_ntp_status);

        set_color(COLOR_RESET);
        sleep_ms(100);
    }

    // 退出前恢复
    set_color(COLOR_RESET);
    CONSOLE_CURSOR_INFO ci = { 1, TRUE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
    return 0;
}