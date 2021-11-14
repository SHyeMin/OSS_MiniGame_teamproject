#pragma warning( disable : 4996 )

#include "rythm_game.h"

static int g_rg_status;
static HANDLE g_rg_screen[2];
static int g_rg_cur_screen;
static char g_rg_sel_list[4][20] = { "시작", "랭킹", "돌아가기" };
static int g_rg_cur_sel;
static int g_cur_width;
static int g_cur_height;

void rg_init() {
    g_rg_status = kStatus_Init;
    g_rg_cur_screen = 0;
    g_rg_cur_sel = 0;
    g_cur_width = kWidth;
    g_cur_height = kHeight;

    init_screen(g_rg_screen, g_cur_width, g_cur_height);
    hide_cursor(g_rg_screen);

    g_rg_status = kStatus_Select;
}

void rg_update()
{

}

void rg_render()
{
    clear_screen(g_rg_screen, g_rg_cur_screen, g_cur_width, g_cur_height);

    flip_screen(g_rg_screen, &g_rg_cur_screen);
}

int rg_main()
{
    clock_t cur_time, old_time;
    rg_init();

    while (g_rg_status != kStatus_End)
    {
        old_time = clock();
        rg_update();
        rg_render();
    }

    release(g_rg_screen);
    return 0;
}