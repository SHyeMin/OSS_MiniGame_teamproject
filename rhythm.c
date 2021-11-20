#pragma warning( disable : 4996 )

#include "rhythm.h"

const char kRgTitle[] = "리듬게임";

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
    if (g_rg_status == kStatus_Select) 
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001)
        {
            g_rg_cur_sel -= 1;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
        {
            g_rg_cur_sel += 1;
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
        {
            switch (g_rg_cur_sel)
            {
            case 0:
                break;
            case 1:
                break;
            case 2:
                g_rg_status = kStatus_End;
                break;
            }
        }

        if (g_rg_cur_sel < 0)
            g_rg_cur_sel = 2;
        else if (g_rg_cur_sel > 2)
            g_rg_cur_sel = 0;
    }
}

void rg_show_title()
{
    print_screen(g_rg_screen, g_rg_cur_screen, g_cur_width / 2 - strlen(kRgTitle) / 2, g_cur_height / 4, kRgTitle);
}

void rg_show_list()
{
    for (int i = 0; i < 3; i++)
    {
        if (g_rg_cur_sel == i)
            print_screen(g_rg_screen, g_rg_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▶");
        else
            print_screen(g_rg_screen, g_rg_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▷");
        print_screen(g_rg_screen, g_rg_cur_screen, g_cur_width / 2 - 2, g_cur_height / 2 + i, g_rg_sel_list[i]);
    }
}

void rg_render()
{
    clear_screen(g_rg_screen, g_rg_cur_screen, g_cur_width, g_cur_height);

    if (g_rg_status == kStatus_Select)
    {
        rg_show_title();
        rg_show_list();
    }

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