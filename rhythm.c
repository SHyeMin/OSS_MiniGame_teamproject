#pragma warning( disable : 4996 )

#include "rhythm.h"

const char kRgTitle[] = "리듬게임";

const char kDiffList[4][20] = { "easy", "normal", "hard", "돌아가기" };
const char kHpGauge[6][20] = { "", "■", "■■", "■■■", "■■■■", "■■■■■" };

const int kRgWidth[] = { 46, 56, 66, 100 };
const int kRgHeight = 25;
const int kRgMapWidth[] = { 30, 40, 50 };
const int kRgMapLine[] = { 3, 4, 5 };

static int g_rg_status;
static HANDLE g_rg_screen[2];
static int g_rg_cur_screen;
static char g_rg_sel_list[4][20] = { "시작", "랭킹", "돌아가기" };

static int g_rg_cur_sel;
static int g_rg_cur_diff;
static int g_cur_width;
static int g_cur_height;
static int g_score;
static int g_success;
static int g_miss;
static int g_combo;
static float g_acc;
static int g_hp;

void rg_init() 
{
    g_rg_status = kStatus_Init;
    g_rg_cur_screen = 0;
    g_rg_cur_sel = 0;
    g_cur_width = kWidth;
    g_cur_height = kHeight;

    init_screen(g_rg_screen, g_cur_width, g_cur_height);
    hide_cursor(g_rg_screen);

    g_rg_status = kStatus_Select;
}

void rg_game_init()
{
    g_score = 0;
    g_success = 0;
    g_miss = 0;
    g_combo = 0;
    g_hp = 10;
}

void rg_mini()
{
    g_cur_width = kWidth;
    g_cur_height = kHeight;

    change_screen(g_rg_screen, g_cur_width, g_cur_height);
}

void rg_max(int size)
{
    g_cur_width = kRgWidth[size];
    g_cur_height = kRgHeight;

    change_screen(g_rg_screen, g_cur_width, g_cur_height);
}

void rg_select_screen()
{
    g_rg_status = kStatus_Select;
    rg_mini();
}

void rg_game_screen()
{
    rg_game_init();

    g_rg_status = kStatus_Play;
    rg_max(g_rg_cur_diff);
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
                g_rg_status = kStatus_Diff;
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
    else if (g_rg_status == kStatus_Diff)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001)
        {
            g_rg_cur_diff -= 1;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
        {
            g_rg_cur_diff += 1;
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
        {
            if (g_rg_cur_diff == 3)
                rg_select_screen();
            else
                rg_game_screen();
        }

        if (g_rg_cur_diff < 0)
            g_rg_cur_diff = 3;
        else if (g_rg_cur_diff > 3)
            g_rg_cur_diff = 0;
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

void rg_show_diff_list()
{
    for (int i = 0; i < 4; i++)
    {
        if (g_rg_cur_diff == i)
            print_screen(g_rg_screen, g_rg_cur_screen, g_cur_width / 2 - 5, g_cur_height / 2 + i, "▶");
        else
            print_screen(g_rg_screen, g_rg_cur_screen, g_cur_width / 2 - 5, g_cur_height / 2 + i, "▷");
        print_screen(g_rg_screen, g_rg_cur_screen, g_cur_width / 2 - 2, g_cur_height / 2 + i, kDiffList[i]);
    }
}

void rg_show_map()
{
    int line = kRgMapLine[g_rg_cur_diff];
    int map_width = kRgMapWidth[g_rg_cur_diff];

    for (int i = 0; i < kRgMapWidth[g_rg_cur_diff]; i++)
    {
        print_screen(g_rg_screen, g_rg_cur_screen, i, 0, "━");
        print_screen(g_rg_screen, g_rg_cur_screen, i, g_cur_height - 3, "━");
        print_screen(g_rg_screen, g_rg_cur_screen, i, g_cur_height - 1, "━");
    }

    for (int i = 0; i <= line; i++)
    {
        if (i == 0)
        {
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, 0, "┏");
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, g_cur_height - 3, "┣");
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, g_cur_height - 1, "┗");
        }
        else if (i == line)
        {
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, 0, "┓");
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, g_cur_height - 3, "┫");
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, g_cur_height - 1, "┛");
        }
        else
        {
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, 0, "┳");
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, g_cur_height - 3, "╋");
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, g_cur_height - 1, "┻");
        }

        for (int j = 1; j < g_cur_height - 1; j++)
            print_screen(g_rg_screen, g_rg_cur_screen, map_width / line * i, j, "┃");
    }
}

void rg_show_key()
{
    print_screen(g_rg_screen, g_rg_cur_screen, 4, g_cur_height - 2, "Q(q)");
    print_screen(g_rg_screen, g_rg_cur_screen, 14, g_cur_height - 2, "W(w)");
    print_screen(g_rg_screen, g_rg_cur_screen, 24, g_cur_height - 2, "E(e)");

    if (g_rg_cur_diff > 0)
        print_screen(g_rg_screen, g_rg_cur_screen, 34, g_cur_height - 2, "R(r)");

    if (g_rg_cur_diff > 1)
        print_screen(g_rg_screen, g_rg_cur_screen, 44, g_cur_height - 2, "T(t)");
}

void rg_show_info()
{
    char score[15];
    sprintf(score, "SCORE: %d", g_score);
    print_screen(g_rg_screen, g_rg_cur_screen, kRgMapWidth[g_rg_cur_diff] + 3, 10, score);

    char combo[15];
    sprintf(combo, "COMBO: %d", g_combo);
    print_screen(g_rg_screen, g_rg_cur_screen, kRgMapWidth[g_rg_cur_diff] + 3, 12, combo);

    char acc[15];
    sprintf(acc, "ACC: %.1f", g_acc);
    print_screen(g_rg_screen, g_rg_cur_screen, kRgMapWidth[g_rg_cur_diff] + 3, 14, acc);

    print_screen(g_rg_screen, g_rg_cur_screen, kRgMapWidth[g_rg_cur_diff] + 3, 16, "HP");
    print_screen_with_color(g_rg_screen, g_rg_cur_screen, kRgMapWidth[g_rg_cur_diff] + 3, 17, kHpGauge[(int) ceil(g_hp / 2.0f)], FOREGROUND_RED);

    print_screen(g_rg_screen, g_rg_cur_screen, kRgMapWidth[g_rg_cur_diff] + 3, 19, "ESC: 종료");
}

void rg_render()
{
    clear_screen(g_rg_screen, g_rg_cur_screen, g_cur_width, g_cur_height);

    if (g_rg_status == kStatus_Select)
    {
        rg_show_title();
        rg_show_list();
    }
    else if (g_rg_status == kStatus_Diff)
    {
        rg_show_title();
        rg_show_diff_list();
    }
    else if (g_rg_status == kStatus_Play)
    {
        rg_show_map();
        rg_show_key();
        rg_show_info();
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