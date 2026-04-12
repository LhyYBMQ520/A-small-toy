package yukifuri.support.asmalltoy;

import com.github.lalyos.jfiglet.FigletFont;
import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.time.format.TextStyle;
import java.util.Locale;

public class Main {
    private static final String
            ANSI_RESET = "\u001B[0m",
            ANSI_CYAN = "\u001B[38;2;0;255;255m",
            ANSI_ORANGE = "\u001B[38;2;255;165;0m",
            ANSI_GREEN = "\u001B[38;2;0;255;0m",
            ANSI_GRAY = "\u001B[38;2;128;128;128m",
    // 修复：加 3J 清滚动区，H 归位
    ANSI_CLEAR = "\033[2J\033[3J\033[H",
    // WT 同步更新
    SYNC_ON = "\033[?2026h",
            SYNC_OFF = "\033[?2026l";

    private static final String OS = System.getProperty("os.name").toLowerCase();
    private static final boolean IS_WT = System.getenv("WT_SESSION") != null;

    private static final DateTimeFormatter
            TIME_FMT = DateTimeFormatter.ofPattern("HH:mm:ss"),
            DATE_FMT = DateTimeFormatter.ofPattern("yyyy-MM-dd"),
            STATUS_FMT = DateTimeFormatter.ofPattern("HH:mm:ss.SSS");

    public static void main(String[] args) {
        try {
            // 预热终端
            System.out.print(ANSI_CLEAR);
            System.out.flush();

            String lastTime = "";

            while (true) {
                String nowTime = LocalDateTime.now().format(TIME_FMT);
                if (!nowTime.equals(lastTime)) {
                    renderFrame();
                    lastTime = nowTime;
                }
                Thread.sleep(50);
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.out.print(ANSI_RESET + "\033[?25h");  // 恢复光标
        }
    }

    private static void renderFrame() {
        LocalDateTime now = LocalDateTime.now();

        String timeStr = spaceOut(now.format(TIME_FMT));
        String dateStr = spaceOut(now.format(DATE_FMT));
        String dayStr = spaceOut(now.getDayOfWeek()
                .getDisplayName(TextStyle.FULL, Locale.ENGLISH));

        try {
            StringBuilder sb = new StringBuilder();

            if (IS_WT) sb.append(SYNC_ON);

            sb.append(ANSI_CLEAR);

            sb.append(ANSI_CYAN)
                    .append("===== SYSTEM DATE & TIME =====")
                    .append(ANSI_RESET).append("\n\n");

            sb.append(ANSI_ORANGE).append("[DATE]").append(ANSI_RESET).append("\n")
                    .append(ANSI_GREEN)
                    .append(FigletFont.convertOneLine(dateStr))
                    .append(ANSI_RESET).append("\n\n");

            sb.append(ANSI_ORANGE).append("[DAY]").append(ANSI_RESET).append("\n")
                    .append(ANSI_GREEN)
                    .append(FigletFont.convertOneLine(dayStr))
                    .append(ANSI_RESET).append("\n\n");

            sb.append(ANSI_ORANGE).append("[TIME]").append(ANSI_RESET).append("\n")
                    .append(ANSI_GREEN)
                    .append(FigletFont.convertOneLine(timeStr))
                    .append(ANSI_RESET).append("\n\n");

            sb.append(ANSI_GRAY)
                    .append("[Updated: ").append(now.format(STATUS_FMT)).append("]")
                    .append(ANSI_RESET);

            if (IS_WT) sb.append(SYNC_OFF);

            byte[] data = sb.toString().getBytes(StandardCharsets.UTF_8);
            System.out.write(data, 0, data.length);
            System.out.flush();

        } catch (Exception e) {
            // 降级：简单输出
            System.out.print(ANSI_CLEAR);
            System.out.println(now);
            System.out.flush();
        }
    }

    private static String spaceOut(String text) {
        if (text == null || text.isEmpty()) return text;
        StringBuilder sb = new StringBuilder(text.length() * 2);
        sb.append(text.charAt(0));
        for (int i = 1; i < text.length(); i++) {
            sb.append(' ').append(text.charAt(i));
        }
        return sb.toString();
    }
}