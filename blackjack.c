#pragma warning( disable : 4996 )
#include "blackjack.h"

const char kBjTitle[] = "블랙잭"; // 게임 타이틀
static int g_bj_status;
static HANDLE g_bj_screen[2];
static int g_bj_cur_screen;
static char g_bj_sel_list[4][20] = { "시작", "규칙", "돌아가기" }; // 타이틀 메뉴 리스트
static int g_bj_cur_sel;
static int g_cur_width;
static int g_cur_height;

void bj_init() {
    g_bj_status = kStatus_Init;
    g_bj_cur_screen = 0;
    g_bj_cur_sel = 0;
    g_cur_width = kWidth;
    g_cur_height = kHeight;

    init_screen(g_bj_screen, g_cur_width, g_cur_height);
    hide_cursor(g_bj_screen);

    g_bj_status = kStatus_Select;
}

void bj_cursor_update() {
    if (g_bj_status == kStatus_Select)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001) {
            g_bj_cur_sel -= 1;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001) {
            g_bj_cur_sel += 1;
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
			if (g_bj_cur_sel == 2) {
                g_bj_status = kStatus_End;
            }
        }

		if (g_bj_cur_sel < 0) { g_bj_cur_sel = 2; }
		else if (g_bj_cur_sel > 2) { g_bj_cur_sel = 0; }
    }
}

void bj_show_title_screen() {
    print_screen(g_bj_screen, g_bj_cur_screen, g_cur_width / 2 - strlen(kBjTitle) / 2, g_cur_height / 4, kBjTitle);
    
    for (int i = 0; i < 3; i++)
    {
        if (g_bj_cur_sel == i) {
            print_screen(g_bj_screen, g_bj_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▶");
        }
        else {
            print_screen(g_bj_screen, g_bj_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▷");
		}
        print_screen(g_bj_screen, g_bj_cur_screen, g_cur_width / 2 - 2, g_cur_height / 2 + i, g_bj_sel_list[i]);
    }
}

void bj_cursor_render()
{
    clear_screen(g_bj_screen, g_bj_cur_screen, g_cur_width, g_cur_height);

    if (g_bj_status == kStatus_Select)
    {
        bj_show_title_screen();
    }

    flip_screen(g_bj_screen, &g_bj_cur_screen);
}

int bj_main() {
	bj_init();

	while (g_bj_status != kStatus_End) {
        bj_cursor_update();
        bj_cursor_render();
    }

    release(g_bj_screen);
	return 0;
}