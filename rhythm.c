#pragma warning( disable : 4996 )

#include "rhythm.h"

const char kRgTitle[] = "리듬게임";

const char kDiffList[4][20] = { "easy", "normal", "hard", "돌아가기" };
const char kHpGauge[6][20] = { "", "■", "■■", "■■■", "■■■■", "■■■■■" };

const int kRgWidth[] = { 46, 56, 66, 100 };
const int kRgHeight = 25;
const int kRgMapWidth[] = { 30, 40, 50 };
const int kRgMapLine[] = { 3, 4, 5 };
const int kRgNoteGoal = 21;

static int g_rg_status;
static int g_rg_cur_screen;

static byte g_selected_key;

static char g_rg_sel_list[4][20] = { "시작", "랭킹", "돌아가기" };

static HANDLE g_rg_screen[2];
static HANDLE run_handle;
static HANDLE key_handle;
static HANDLE skip_handle;

static Player g_rg_players[3][1000];

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
static int g_rg_player_size[3];
static int g_create_time;
static int g_move_time;
static Note g_note[21];
static clock_t g_last_create_time;
static clock_t g_last_move_time;

unsigned __stdcall rg_running_game(void* a);
unsigned __stdcall rg_push_key(void* a);
unsigned __stdcall rg_skip_key(void* a);

void load_data();

void rg_init() 
{
    skip_handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)rg_skip_key, 0, 0, NULL);

    g_rg_status = kStatus_Init;
    g_rg_cur_screen = 0;
    g_rg_cur_sel = 0;
    g_cur_width = kWidth;
    g_cur_height = kHeight;

    init_screen(g_rg_screen, g_cur_width, g_cur_height);
    hide_cursor(g_rg_screen);
    load_data();

    g_rg_status = kStatus_Select;
}

void rg_game_init()
{
    g_score = 0;
    g_success = 0;
    g_miss = 0;
    g_combo = 0;
    g_hp = 10;
    g_create_time = 300;
    g_move_time = 100;

    for (int i = 0; i < kRgNoteGoal; i++)
        g_note[i].visible = FALSE;

    run_handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)rg_running_game, 0, 0, NULL);
    key_handle = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)rg_push_key, 0, 0, NULL);
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

Player rg_make_player(char* name, int score)
{
    Player p;
    strcpy(p.name, name);
    p.score = score;

    return p;
}

void rg_swap(Player* arr, int a, int b)
{
    Player temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}

void rg_rank_sort(Player* arr, int m, int n)
{
    if (m < n)
    {
        int key = m;
        int i = m + 1;
        int j = n;

        while (i <= j)
        {
            while (i <= n && arr[i].score >= arr[key].score)
                i++;
            while (j > m && arr[j].score <= arr[key].score)
                j--;
            if (i > j)
                rg_swap(arr, j, key);
            else
                rg_swap(arr, i, j);
        }

        rg_rank_sort(arr, m, j - 1);
        rg_rank_sort(arr, j + 1, n);
    }
}

void save_data(char* name)
{
    FILE* fp;
    char buffer[100];

    fp = fopen("data.txt", "a+");

    if (fp == NULL)
        return;

    Player p = rg_make_player(name, g_score);
    g_rg_players[g_rg_cur_diff][g_rg_player_size[g_rg_cur_diff]++] = p;
    rg_rank_sort(g_rg_players[g_rg_cur_diff], 0, g_rg_player_size[g_rg_cur_diff] - 1);

    fprintf(fp, "%s:%d:%d\n", name, g_rg_cur_diff, g_score);

    fclose(fp);
}

void load_data()
{
    FILE* fp;
    char* name;
    char buffer[100];
    int diff, score;

    fp = fopen("data.txt", "r");

    if (fp == NULL)
        return;

    memset(g_rg_player_size, 0, sizeof(int) * 3);

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        name = strtok(buffer, ":");
        diff = atoi(strtok(NULL, ":"));
        score = atoi(strtok(NULL, ":"));
        g_rg_players[diff][g_rg_player_size[diff]++] = rg_make_player(name, score);

        memset(buffer, 0, sizeof(buffer));
    }

    rg_rank_sort(g_rg_players[0], 0, g_rg_player_size[0] - 1);
    rg_rank_sort(g_rg_players[1], 0, g_rg_player_size[1] - 1);
    rg_rank_sort(g_rg_players[2], 0, g_rg_player_size[2] - 1);

    fclose(fp);
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

void rg_rank_screen()
{
    g_rg_status = kStatus_Rank;
    rg_max(3);
}

void rg_save_screen()
{
    g_rg_status = kStatus_Save;
    HANDLE hIn = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleActiveScreenBuffer(hIn);
    char name[20];
    DWORD dw;
    COORD cursor_position = { 0, 0 };

    system("cls");

    do {
        printf("닉네임 : ");
        gets_s(name, sizeof(name));
    } while (strlen(name) == 0);

    fflush(stdin);

    save_data(name);

    rg_select_screen();
}

void rg_create_note()
{
    if (clock() - g_last_create_time > g_create_time)
    {
        for (int i = 0; i < kRgNoteGoal; i++)
        {
            if (!g_note[i].visible)
            {
                g_note[i].x = rand() % kRgMapLine[g_rg_cur_diff];
                g_note[i].y = 1;
                g_note[i].visible = TRUE;
                break;
            }
        }
        g_last_create_time = clock();
    }
}

void rg_move_note()
{
    if (clock() - g_last_move_time > g_move_time)
    {
        for (int i = 0; i < kRgNoteGoal; i++)
        {
            if (g_note[i].visible)
            {
                g_note[i].y = g_note[i].y + 1;

                if (g_note[i].y - 1 == kRgNoteGoal)
                {
                    g_note[i].visible = FALSE;
                    g_miss++;
                    g_hp--;
                    g_combo = 0;
                    if (g_hp == 0)
                        rg_save_screen();
                }
            }
        }
        g_last_move_time = clock();
    }
}

void rg_hit_note()
{
    for (int i = 0; i < kRgNoteGoal; i++)
    {
        if (g_note[i].visible)
        {
            if (g_note[i].y == kRgNoteGoal)
            {
                if (g_note[i].x == 0 && (g_selected_key & 0x1) || g_note[i].x == 1 && (g_selected_key & 0x2)
                    || g_note[i].x == 2 && (g_selected_key & 0x4) || g_note[i].x == 3 && (g_selected_key & 0x8)) {
                    g_note[i].visible = FALSE;
                    g_combo++;
                    g_success++;
                    g_score += g_combo;
                    g_create_time = g_score == 0 ? 300 : 300 - (g_score / 100 * 10);
                    g_move_time = g_score == 0 ? 100 : 100 - (g_score / 100 * 5);
                    if (g_combo == 2)
                    {
                        g_hp++;
                        if (g_hp > 10)
                            g_hp = 10;
                    }
                }
            }
        }
    }
    g_selected_key = 0;
    g_acc = (g_success == 0 && g_miss == 0) ? 100.0f : (float)g_success / (g_success + g_miss) * 100;
}

unsigned __stdcall rg_running_game(void* a)
{
    while (g_rg_status == kStatus_Play)
    {
        rg_move_note();
        rg_create_note();
    }

    return 0;
}

unsigned __stdcall rg_push_key(void* a)
{
    while (g_rg_status == kStatus_Play)
    {
        if (GetAsyncKeyState(kKey_Q) & 0x0001)
            g_selected_key |= 0x1;

        if (GetAsyncKeyState(kKey_W) & 0x0001)
            g_selected_key |= 0x2;

        if (GetAsyncKeyState(kKey_E) & 0x0001)
            g_selected_key |= 0x4;

        if (GetAsyncKeyState(kKey_R) & 0x0001)
            g_selected_key |= 0x8;

        if (GetAsyncKeyState(kKey_T) & 0x0001)
            g_selected_key |= 0x10;


        rg_hit_note();
    }

    return 0;
}

unsigned __stdcall rg_skip_key(void* a)
{
    char c;
    while (1)
    {

        while (_kbhit())
        {
            c = _getch();
        }
        Sleep(10);
    }

    return 0;
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
                rg_rank_screen();
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
    else if (g_rg_status == kStatus_Play)
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
            rg_select_screen();
    }
    else if (g_rg_status == kStatus_Rank)
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
            rg_select_screen();
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

void rg_show_note()
{
    for (int i = 0; i < kRgNoteGoal; i++)
        if (g_note[i].visible)
            print_screen(g_rg_screen, g_rg_cur_screen, g_note[i].x * 10 + 2, g_note[i].y, "■■■■");
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

void rg_show_rank()
{

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < g_cur_height; j++)
        {
            int x = kRgWidth[3] / 3 * i;
            if (i == 3)
                x--;

            print_screen(g_rg_screen, g_rg_cur_screen, x, j, "┃");

            if (j == 0 && i != 3)
                print_screen(g_rg_screen, g_rg_cur_screen, x + 2, 0, kDiffList[i]);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (j >= g_rg_player_size[i])
                break;
            int x = kRgWidth[3] / 3 * i;
            if (i == 3)
                x--;

            Player p = g_rg_players[i][j];
            char msg[30];
            sprintf(msg, "%d위: %s %d", j + 1, p.name, p.score);

            print_screen(g_rg_screen, g_rg_cur_screen, x + 2, j * 2 + 2, msg);
        }
    }

    print_screen(g_rg_screen, g_rg_cur_screen, g_cur_width - strlen("ESC: 돌어가기") - 2, g_cur_height - 1, "ESC: 돌아가기");
}

void rg_render()
{
    if (g_rg_status == kStatus_Save)
        return;

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
        rg_show_note();
        rg_show_info();
    }
    else if (g_rg_status == kStatus_Rank)
    {
        rg_show_rank();
    }

    if (g_rg_status == kStatus_Save)
        return;

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
        while (1)
        {
            cur_time = clock();
            if (cur_time - old_time > (1 / 60 * 1000))
                break;
        }
    }

    release(g_rg_screen);
    return 0;
}