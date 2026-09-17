#include "../include/terminal.h"
#include "../include/graphics.h"
#include "../include/string.h"
#include "../include/fs.h"

#define TERM_COLS 60
#define TERM_ROWS 20
#define PROMPT "> "

static char term_buffer[TERM_ROWS][TERM_COLS];
static uint32_t term_cursor_x = 0;
static uint32_t term_cursor_y = 0;
static uint8_t is_open = 0;

static int32_t term_win_x = -1;
static int32_t term_win_y = -1;

extern volatile uint32_t tick;

static void terminal_scroll(void) {
    for (int i = 1; i < TERM_ROWS; i++) {
        for (int j = 0; j < TERM_COLS; j++) {
            term_buffer[i - 1][j] = term_buffer[i][j];
        }
    }
    for (int j = 0; j < TERM_COLS; j++) {
        term_buffer[TERM_ROWS - 1][j] = '\0';
    }
    term_cursor_y = TERM_ROWS - 1;
}

static void terminal_print(const char* str) {
    while (*str) {
        if (*str == '\n') {
            term_cursor_x = 0;
            term_cursor_y++;
            if (term_cursor_y >= TERM_ROWS) {
                terminal_scroll();
            }
        } else {
            term_buffer[term_cursor_y][term_cursor_x++] = *str;
            if (term_cursor_x >= TERM_COLS - 1) {
                term_cursor_x = 0;
                term_cursor_y++;
                if (term_cursor_y >= TERM_ROWS) {
                    terminal_scroll();
                }
            }
        }
        str++;
    }
}

static void itoa_simple(uint32_t num, char* str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    char temp[16];
    int temp_idx = 0;
    while (num > 0) {
        temp[temp_idx++] = (num % 10) + '0';
        num /= 10;
    }
    while (temp_idx > 0) {
        str[i++] = temp[--temp_idx];
    }
    str[i] = '\0';
}


static void term_print_helper(const char* str) {
    terminal_print(str);
}

static void terminal_execute(char* cmd) {
    terminal_print("\n");
    if (strcmp(cmd, "help") == 0) {
        terminal_print("Available commands:\n");
        terminal_print("  help    - Show this message\n");
        terminal_print("  whoami  - Print user information\n");
        terminal_print("  uptime  - Show system uptime\n");
        terminal_print("  clear   - Clear terminal screen\n");
        terminal_print("  ls      - List files\n");
        terminal_print("  cat <f> - Read a file\n");
    } else if (strcmp(cmd, "whoami") == 0) {
        terminal_print("Recruiter (Guest)\n");
    } else if (strcmp(cmd, "uptime") == 0) {
        uint32_t seconds = tick / 50; // 50Hz timer
        char buf[16];
        itoa_simple(seconds, buf);
        terminal_print("Uptime: ");
        terminal_print(buf);
        terminal_print(" seconds\n");
    } else if (strcmp(cmd, "clear") == 0) {
        for (int i = 0; i < TERM_ROWS; i++) {
            for (int j = 0; j < TERM_COLS; j++) {
                term_buffer[i][j] = '\0';
            }
        }
        term_cursor_x = 0;
        term_cursor_y = 0;
    } else if (strcmp(cmd, "ls") == 0) {
        fs_list_files(term_print_helper);
    } else if (cmd[0] == 'c' && cmd[1] == 'a' && cmd[2] == 't' && cmd[3] == ' ') {
        char* filename = &cmd[4];
        char file_buf[2048];
        int size = fs_read_file(filename, file_buf, sizeof(file_buf));
        if (size >= 0) {
            terminal_print(file_buf);
            terminal_print("\n");
        } else {
            terminal_print("File not found: ");
            terminal_print(filename);
            terminal_print("\n");
        }
    } else if (strlen(cmd) > 0) {
        terminal_print("Command not found: ");
        terminal_print(cmd);
        terminal_print("\n");
    }
    terminal_print(PROMPT);
}

void terminal_init(void) {
    for (int i = 0; i < TERM_ROWS; i++) {
        for (int j = 0; j < TERM_COLS; j++) {
            term_buffer[i][j] = '\0';
        }
    }
    term_cursor_x = 0;
    term_cursor_y = 0;
    terminal_print("VOS Terminal Emulator v1.0\n");
    terminal_print("Type 'help' for a list of commands.\n");
    terminal_print(PROMPT);
}

void terminal_toggle(void) {
    is_open = !is_open;
}

uint8_t terminal_is_open(void) {
    return is_open;
}

void terminal_get_rect(int32_t* x, int32_t* y, uint32_t* w, uint32_t* h) {
    *w = TERM_COLS * 8 + 20;
    *h = TERM_ROWS * 10 + 40;
    if (term_win_x == -1) {
        *x = (get_screen_width() - *w) / 2;
        *y = (get_screen_height() - *h) / 2 - 20;
    } else {
        *x = term_win_x;
        *y = term_win_y;
    }
}

void terminal_set_pos(int32_t x, int32_t y) {
    term_win_x = x;
    term_win_y = y;
}

void terminal_render(void) {
    if (!is_open) return;
    
    int32_t win_x, win_y;
    uint32_t win_w, win_h;
    terminal_get_rect(&win_x, &win_y, &win_w, &win_h);
    
    // Set actual variables in case they were -1
    term_win_x = win_x;
    term_win_y = win_y;
    
    // Window background
    draw_rect(win_x, win_y, win_w, win_h, 0x1e1e1e);
    draw_rect(win_x, win_y, win_w, 1, 0x333337);
    draw_rect(win_x, win_y, 1, win_h, 0x333337);
    draw_rect(win_x + win_w - 1, win_y, 1, win_h, 0x333337);
    draw_rect(win_x, win_y + win_h - 1, win_w, 1, 0x333337);
    
    // Title bar
    draw_rect(win_x + 1, win_y + 1, win_w - 2, 24, 0x333337);
    draw_string(win_x + 10, win_y + 9, "Terminal", 0xd4d4d4, 0x333337);
    
    // Close button
    uint32_t close_x = win_x + win_w - 24;
    uint32_t close_y = win_y + 1;
    draw_rect(close_x, close_y, 23, 24, 0xe81123);
    draw_string(close_x + 8, close_y + 8, "X", 0xFFFFFF, 0xe81123);
    
    // Text area
    draw_rect(win_x + 10, win_y + 30, win_w - 20, win_h - 40, 0x000000);
    
    // Draw text
    for (int i = 0; i < TERM_ROWS; i++) {
        for (int j = 0; j < TERM_COLS; j++) {
            char c = term_buffer[i][j];
            if (c) {
                draw_char_scaled(win_x + 10 + j * 8, win_y + 32 + i * 10, c, 0xFFFFFF, 0x000000, 1);
            }
        }
    }
    
    // Cursor blink
    if ((tick / 25) % 2 == 0) {
        draw_rect(win_x + 10 + term_cursor_x * 8, win_y + 32 + term_cursor_y * 10, 8, 10, 0xFFFFFF);
    }
}

void terminal_handle_keyboard(char c) {
    if (!is_open) return;
    
    if (c == '\n') {
        // Extract command from prompt
        char cmd[TERM_COLS];
        int j = 2; // skip PROMPT "> "
        int k = 0;
        while (term_buffer[term_cursor_y][j] != '\0' && j < TERM_COLS) {
            cmd[k++] = term_buffer[term_cursor_y][j++];
        }
        cmd[k] = '\0';
        terminal_execute(cmd);
    } else if (c == '\b') {
        if (term_cursor_x > 2) {
            term_cursor_x--;
            term_buffer[term_cursor_y][term_cursor_x] = '\0';
        }
    } else {
        if (term_cursor_x < TERM_COLS - 1) {
            term_buffer[term_cursor_y][term_cursor_x] = c;
            term_cursor_x++;
            term_buffer[term_cursor_y][term_cursor_x] = '\0';
        }
    }
}
