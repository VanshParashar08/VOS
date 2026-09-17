#include "../include/desktop.h"
#include "../include/graphics.h"
#include "../include/mouse.h"
#include "../include/wallpapers.h"
#include "../include/terminal.h"
#include "../include/rtc.h"

// Modern OS Theme Colors
#define COLOR_TASKBAR    0x1e1e1e
#define COLOR_WINDOW     0x252526
#define COLOR_WIN_TITLE  0x333337
#define COLOR_TEXT       0xd4d4d4
#define COLOR_WHITE      0xFFFFFF
#define COLOR_BLACK      0x000000
#define COLOR_ACCENT     0x007acc // Blue accent for Start menu selection/bars
#define COLOR_RED        0xe81123 // Close button red

static const uint32_t* wallpaper_images[] = {wallpaper_1, wallpaper_2, wallpaper_3, wallpaper_4};
static uint32_t current_wallpaper_idx = 1; // Default to minimalist landscape

static int32_t last_mouse_x = -1;
static int32_t last_mouse_y = -1;
static uint8_t last_mouse_btns = 0;
static uint8_t start_menu_open = 0;
static uint8_t my_computer_open = 0;

static int32_t mycomp_x = 150;
static int32_t mycomp_y = 150;
static uint8_t dragging_mycomp = 0;
static uint8_t dragging_term = 0;
static int32_t drag_offset_x = 0;
static int32_t drag_offset_y = 0;

extern void sleep(uint32_t ms);

void show_loading_screen(void) {
    uint32_t w = get_screen_width();
    uint32_t h = get_screen_height();
    
    // Black background
    draw_rect(0, 0, w, h, COLOR_BLACK);
    
    // Draw scaled logo: "VOS"
    uint32_t scale = 10;
    uint32_t logo_w = 3 * 8 * scale;
    uint32_t logo_x = (w - logo_w) / 2;
    uint32_t logo_y = h / 2 - 100;
    
    draw_string_scaled(logo_x, logo_y, "VOS", COLOR_WHITE, COLOR_BLACK, scale);
    
    // Loading bar frame (modernized)
    uint32_t bar_w = 400;
    uint32_t bar_h = 4;
    uint32_t bar_x = (w - bar_w) / 2;
    uint32_t bar_y = h / 2 + 50;
    
    draw_rect(bar_x, bar_y, bar_w, bar_h, COLOR_WIN_TITLE);
    
    draw_string((w - 112) / 2, bar_y - 20, "VOS is loading...", COLOR_TEXT, COLOR_BLACK);
    
    // Animate loading bar
    for (int i = 0; i <= 100; i += 2) {
        uint32_t fill_w = (bar_w * i) / 100;
        draw_rect(bar_x, bar_y, fill_w, bar_h, COLOR_ACCENT);
        sleep(20);
    }
    sleep(200);
}

static void draw_start_menu(void) {
    uint32_t screen_h = get_screen_height();
    uint32_t menu_w = 200;
    uint32_t menu_h = 60;
    uint32_t menu_x = 2;
    uint32_t menu_y = screen_h - 30 - menu_h - 2; // slight gap
    
    // Flat background with subtle border
    draw_rect(menu_x, menu_y, menu_w, menu_h, COLOR_TASKBAR);
    draw_rect(menu_x, menu_y, menu_w, 1, COLOR_WIN_TITLE); // Top border
    draw_rect(menu_x + menu_w - 1, menu_y, 1, menu_h, COLOR_WIN_TITLE); // Right border
    
    // Draw items (Flat)
    draw_string(menu_x + 20, menu_y + 25, "Terminal", COLOR_TEXT, COLOR_TASKBAR);
}

static void draw_my_computer_icon(uint32_t x, uint32_t y) {
    // Modern flat monitor icon
    draw_rect(x + 4, y + 4, 32, 24, COLOR_TASKBAR);
    draw_rect(x + 6, y + 6, 28, 20, COLOR_WINDOW);
    draw_rect(x + 16, y + 28, 8, 6, COLOR_TASKBAR);
    draw_rect(x + 8, y + 34, 24, 4, COLOR_TASKBAR);
    
    draw_string(x - 12, y + 42, "My Computer", COLOR_WHITE, 0xFFFFFFFF); // Transparent background
}

static void draw_my_computer_window(void) {
    uint32_t win_x = mycomp_x, win_y = mycomp_y;
    uint32_t win_w = 300, win_h = 200;
    
    // Flat window background
    draw_rect(win_x, win_y, win_w, win_h, COLOR_WINDOW);
    draw_rect(win_x, win_y, win_w, 1, COLOR_TASKBAR); // Subtle border
    draw_rect(win_x, win_y, 1, win_h, COLOR_TASKBAR);
    draw_rect(win_x + win_w - 1, win_y, 1, win_h, COLOR_TASKBAR);
    draw_rect(win_x, win_y + win_h - 1, win_w, 1, COLOR_TASKBAR);
    
    // Title bar
    draw_rect(win_x + 1, win_y + 1, win_w - 2, 24, COLOR_WIN_TITLE);
    draw_string(win_x + 10, win_y + 9, "System Specs", COLOR_TEXT, COLOR_WIN_TITLE);
    
    // Close button
    uint32_t close_x = win_x + win_w - 24;
    uint32_t close_y = win_y + 1;
    draw_rect(close_x, close_y, 23, 24, COLOR_WIN_TITLE);
    draw_string(close_x + 8, close_y + 8, "X", COLOR_TEXT, COLOR_WIN_TITLE);
    
    // Content
    draw_string(win_x + 10, win_y + 40, "Local Disk (C:)", COLOR_TEXT, COLOR_WINDOW);
    draw_string(win_x + 10, win_y + 60, "Total Space: 128 MB", COLOR_TEXT, COLOR_WINDOW);
    
    draw_string(win_x + 10, win_y + 100, "System Specifications:", COLOR_TEXT, COLOR_WINDOW);
    draw_string(win_x + 10, win_y + 120, "OS: VOS (Modern)", COLOR_TEXT, COLOR_WINDOW);
    draw_string(win_x + 10, win_y + 140, "CPU: i386 Architecture", COLOR_TEXT, COLOR_WINDOW);
    draw_string(win_x + 10, win_y + 160, "RAM: 128 MB", COLOR_TEXT, COLOR_WINDOW);
}

void desktop_init(void) {
    // Draw full-screen graphical wallpaper (400x300 scaled by 2)
    draw_image_scaled(0, 0, 400, 300, wallpaper_images[current_wallpaper_idx], 2);
    
    // Draw My Computer Icon
    draw_my_computer_icon(20, 20);
    
    // Draw the taskbar at the bottom (30 pixels high)
    uint32_t screen_h = get_screen_height();
    uint32_t screen_w = get_screen_width();
    
    draw_rect(0, screen_h - 30, screen_w, 30, COLOR_TASKBAR);
    
    // Draw a "Start" button (flat)
    if (start_menu_open) {
        draw_rect(0, screen_h - 30, 60, 30, COLOR_WIN_TITLE);
    } else {
        draw_rect(0, screen_h - 30, 60, 30, COLOR_TASKBAR);
    }
    // Simple Start text
    draw_string(14, screen_h - 20, "Start", COLOR_TEXT, start_menu_open ? COLOR_WIN_TITLE : COLOR_TASKBAR);
    
    // Change Wallpaper button on taskbar
    uint32_t wall_btn_w = 140;
    uint32_t wall_btn_x = screen_w - wall_btn_w - 90;
    draw_rect(wall_btn_x, screen_h - 30, wall_btn_w, 30, COLOR_WIN_TITLE);
    draw_string(wall_btn_x + 10, screen_h - 20, "Change Wallpaper", COLOR_TEXT, COLOR_WIN_TITLE);
    
    // Draw Clock
    uint8_t h, m, s;
    rtc_read_time(&h, &m, &s);
    char time_str[9] = "00:00:00";
    time_str[0] = (h / 10) + '0';
    time_str[1] = (h % 10) + '0';
    time_str[3] = (m / 10) + '0';
    time_str[4] = (m % 10) + '0';
    time_str[6] = (s / 10) + '0';
    time_str[7] = (s % 10) + '0';
    
    uint32_t clock_x = screen_w - 80;
    draw_string(clock_x + 8, screen_h - 20, time_str, COLOR_TEXT, COLOR_TASKBAR);

    
    if (my_computer_open) {
        draw_my_computer_window();
    }
    
    // Draw Start Menu if open
    if (start_menu_open) {
        draw_start_menu();
    }
    
    // Draw Terminal if open
    if (terminal_is_open()) {
        terminal_render();
    }
}

static void draw_cursor(int32_t x, int32_t y) {
    // Windows-style arrow cursor
    static const uint8_t cursor_bmp[16][12] = {
        {1,1,0,0,0,0,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0,0,0,0,0},
        {1,2,2,1,0,0,0,0,0,0,0,0},
        {1,2,2,2,1,0,0,0,0,0,0,0},
        {1,2,2,2,2,1,0,0,0,0,0,0},
        {1,2,2,2,2,2,1,0,0,0,0,0},
        {1,2,2,2,2,2,2,1,0,0,0,0},
        {1,2,2,2,2,2,2,2,1,0,0,0},
        {1,2,2,2,2,2,2,2,2,1,0,0},
        {1,2,2,2,2,2,1,1,1,1,0,0},
        {1,2,2,1,2,2,1,0,0,0,0,0},
        {1,2,1,0,1,2,2,1,0,0,0,0},
        {1,1,0,0,1,2,2,1,0,0,0,0},
        {0,0,0,0,0,1,2,2,1,0,0,0},
        {0,0,0,0,0,1,2,2,1,0,0,0},
        {0,0,0,0,0,0,1,1,0,0,0,0}
    };
    
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 12; j++) {
            if (cursor_bmp[i][j] == 1) {
                put_pixel(x + j, y + i, 0xFFFFFF); // White outline
            } else if (cursor_bmp[i][j] == 2) {
                put_pixel(x + j, y + i, 0x000000); // Black fill
            }
        }
    }
}

void desktop_render(void) {
    int32_t mx, my;
    uint8_t btns;
    
    mouse_get_state(&mx, &my, &btns);
    
    uint8_t needs_redraw = 0;
    
    if (!(btns & 1)) {
        dragging_mycomp = 0;
        dragging_term = 0;
    }
    
    if (dragging_mycomp) {
        mycomp_x = mx - drag_offset_x;
        mycomp_y = my - drag_offset_y;
        needs_redraw = 1;
    } else if (dragging_term) {
        terminal_set_pos(mx - drag_offset_x, my - drag_offset_y);
        needs_redraw = 1;
    }
    
    if ((btns & 1) && !(last_mouse_btns & 1)) {
        uint32_t screen_h = get_screen_height();
        uint32_t screen_w = get_screen_width();
        
        // 1. Check Start button (x: 0 to 60, y: screen_h - 30 to screen_h)
        if (mx >= 0 && mx <= 60 && my >= (int32_t)(screen_h - 30) && my <= (int32_t)(screen_h)) {
            start_menu_open = !start_menu_open;
            needs_redraw = 1;
        } 
        // 2. Check Wallpaper button
        else if (mx >= (int32_t)(screen_w - 140 - 90) && mx <= (int32_t)(screen_w - 90) && mx <= (int32_t)screen_w && my >= (int32_t)(screen_h - 30) && my <= (int32_t)screen_h) {
            current_wallpaper_idx = (current_wallpaper_idx + 1) % 4;
            start_menu_open = 0;
            needs_redraw = 1;
        }
        // 3. Check My Computer Icon
        else if (mx >= 20 && mx <= 60 && my >= 20 && my <= 60) {
            my_computer_open = 1;
            start_menu_open = 0;
            needs_redraw = 1;
        }
        // 4. Check My Computer Close button (if open)
        else if (my_computer_open && mx >= mycomp_x + 300 - 24 && mx <= mycomp_x + 300 && my >= mycomp_y + 1 && my <= mycomp_y + 25) {
            my_computer_open = 0;
            start_menu_open = 0;
            needs_redraw = 1;
        }
        // 4.5 Check My Computer Title bar (for dragging)
        else if (my_computer_open && mx >= mycomp_x && mx <= mycomp_x + 300 && my >= mycomp_y && my <= mycomp_y + 25) {
            dragging_mycomp = 1;
            drag_offset_x = mx - mycomp_x;
            drag_offset_y = my - mycomp_y;
            start_menu_open = 0;
        }
        // 5. Check Start Menu items (Terminal)
        else if (start_menu_open && mx >= 2 && mx <= 202 && my >= (int32_t)(screen_h - 30 - 60 - 2) && my <= (int32_t)(screen_h - 30 - 2)) {
            uint32_t menu_y = screen_h - 30 - 60 - 2;
            if (my >= menu_y + 15 && my <= menu_y + 45) {
                if (!terminal_is_open()) terminal_toggle();
            }
            start_menu_open = 0;
            needs_redraw = 1;
        }
        // 6. Check Terminal Close button (if open)
        else if (terminal_is_open()) {
            int32_t win_x, win_y;
            uint32_t win_w, win_h;
            terminal_get_rect(&win_x, &win_y, &win_w, &win_h);
            
            if (mx >= win_x + (int32_t)win_w - 24 && mx <= win_x + (int32_t)win_w && my >= win_y + 1 && my <= win_y + 25) {
                terminal_toggle();
                needs_redraw = 1;
            }
            // 6.5 Check Terminal Title bar (for dragging)
            else if (mx >= win_x && mx <= win_x + (int32_t)win_w && my >= win_y && my <= win_y + 25) {
                dragging_term = 1;
                drag_offset_x = mx - win_x;
                drag_offset_y = my - win_y;
                start_menu_open = 0;
            }
        }
        else {
            if (start_menu_open) {
                start_menu_open = 0;
                needs_redraw = 1;
            }
        }
    }
    
    // Check keyboard
    extern char keyboard_getchar(void);
    char c = keyboard_getchar();
    if (c != 0) {
        if (terminal_is_open()) {
            terminal_handle_keyboard(c);
            needs_redraw = 1;
        }
    }
    
    // Always blink cursor in terminal
    extern volatile uint32_t tick;
    static uint32_t last_tick = 0;
    if (terminal_is_open() && (tick - last_tick) > 12) {
        needs_redraw = 1;
        last_tick = tick;
    }
    
    if (mx != last_mouse_x || my != last_mouse_y || needs_redraw) {
        desktop_init(); 
        draw_cursor(mx, my);
        
        last_mouse_x = mx;
        last_mouse_y = my;
    }
    
    last_mouse_btns = btns;
}