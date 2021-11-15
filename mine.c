#pragma warning( disable : 4996 )

#include "mine.h"

const char kMgTitle[] = "지뢰찾기";

static int g_mg_status;
static HANDLE g_mg_screen[2];
static int g_mg_cur_screen;
static char g_mg_sel_list[5][20] = { "쉬움", "보통", "어려움","돌아가기"};
static int g_mg_cur_sel;
static int g_cur_width;
static int g_cur_height;

void mg_init() 
{
    g_mg_status = kStatus_Init;
    g_mg_cur_screen = 0;
    g_mg_cur_sel = 0;
    g_cur_width = kWidth;
    g_cur_height = kHeight;

    init_screen(g_mg_screen, g_cur_width, g_cur_height);
    hide_cursor(g_mg_screen);

    g_mg_status = kStatus_Select;
}

void mg_update()
{
    if (g_mg_status == kStatus_Select)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001)
        {
            g_mg_cur_sel -= 1;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
        {
            g_mg_cur_sel += 1;
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
        {
            if (g_mg_cur_sel == 3)
                g_mg_status = kStatus_End;
        }

        if (g_mg_cur_sel < 0)
            g_mg_cur_sel = 3;
        else if (g_mg_cur_sel > 3)
            g_mg_cur_sel = 0;
        
    }
}

void mg_show_title()
{
    print_screen(g_mg_screen, g_mg_cur_screen, g_cur_width / 2 - strlen(kMgTitle) / 2, g_cur_height / 4, kMgTitle);
}

void mg_show_list()
{
    for (int i = 0; i < 4; i++)
    {
        if (g_mg_cur_sel == i)
            print_screen(g_mg_screen, g_mg_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▶");
        else
            print_screen(g_mg_screen, g_mg_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▷");
        print_screen(g_mg_screen, g_mg_cur_screen, g_cur_width / 2 - 2, g_cur_height / 2 + i, g_mg_sel_list[i]);
    }
}

void mg_render()
{
    clear_screen(g_mg_screen, g_mg_cur_screen, g_cur_width, g_cur_height);

    if (g_mg_status == kStatus_Select)
    {
        mg_show_title();
        mg_show_list();
    }

    flip_screen(g_mg_screen, &g_mg_cur_screen);
}


int mg_main()
{
    mg_init();

    while (g_mg_status != kStatus_End)
    {
        mg_update();
        mg_render();
    }

    release(g_mg_screen);
    return 0;
}

