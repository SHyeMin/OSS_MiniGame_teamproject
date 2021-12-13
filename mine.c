#pragma warning( disable : 4996 )

#include "mine.h"

const char kMgTitle[] = "지뢰찾기";

static int g_mg_status;
static HANDLE g_mg_screen[100];
static int g_mg_cur_screen;
static char g_mg_sel_list[5][20] = { "쉬움", "보통", "어려움","돌아가기"};
static int g_mg_cur_sel;
static int g_mg_cur_UD;
static int g_mg_cur_LR;
static int g_cur_width;
static int g_cur_height;
static int mg_diff;
static int mgX_cnt;
static int mgY_cnt;
static int mg_cnt;
static int mg_set_cnt;
static int x;
static int y;
static int k;
static int l;
static int p;
static char mg_E_table[9][9];
static char state[10];
static char mg_M_table[16][16];
static char mg_H_table[16][30];
static int mg_inst;


void mg_init()  // 선택화면 구성
{
    g_mg_status = kStatus_Init;
    g_mg_cur_screen = 0;
    g_mg_cur_sel = 0;
    g_cur_width = kWidth;
    g_cur_height = kHeight;
    g_mg_cur_LR = 0;
    g_mg_cur_UD = 0;

    init_screen(g_mg_screen, g_cur_width, g_cur_height);
    hide_cursor(g_mg_screen);

    g_mg_status = kStatus_Select;
}



void mg_show_E_table()
{
    clear_screen(g_mg_screen, g_mg_cur_screen,100, 100);
    change_screen(g_mg_screen, 40, 18);
    for ( k = 0; k < 9; k++)
    {
        for ( l = 0; l< 9; l++)
        {
            if (g_mg_cur_UD == k&&g_mg_cur_LR==l)
                print_screen(g_mg_screen, g_mg_cur_screen, 10+l*2, 3 + k, "■");
            else
                print_screen(g_mg_screen, g_mg_cur_screen, 10+l*2, 3 + k, "□");
        }
    }
    print_screen(g_mg_screen, g_mg_cur_screen, 5, 1, "Easy mode 지뢰는 10개입니다!");
}

void mg_show_M_table()
{
    clear_screen(g_mg_screen, g_mg_cur_screen, 100, 100);
    change_screen(g_mg_screen, 60, 28);
    for ( k = 0; k < 16; k++)
    {
        for (int l = 0; l < 16; l++)
        {
            if (g_mg_cur_UD == k && g_mg_cur_LR == l)
                print_screen(g_mg_screen, g_mg_cur_screen, 13 + l * 2, 5 + k, "■");
            else
                print_screen(g_mg_screen, g_mg_cur_screen, 13 + l * 2, 5 + k, "□");
        }
    }
        print_screen(g_mg_screen, g_mg_cur_screen, 15, 3, "Medium mode 지뢰는 40개입니다!");
}

void mg_show_H_table()
{
    clear_screen(g_mg_screen, g_mg_cur_screen, 100, 100);
    change_screen(g_mg_screen, 80, 28);

    
    for (int k = 0; k < 16; k++)
    {
        for (int l = 0; l < 30; l++)
        {
            if (g_mg_cur_UD == k && g_mg_cur_LR == l)
                print_screen(g_mg_screen, g_mg_cur_screen, 10 + l * 2, 5 + k, "■");
            else
                print_screen(g_mg_screen, g_mg_cur_screen, 10 + l * 2, 5 + k, "□");
        }
    }
    print_screen(g_mg_screen, g_mg_cur_screen, 26, 3, "Hard mode 지뢰는 99개입니다!");
}

void mg_make_mine()
{

    for (y = 0; y < mgY_cnt; y++)
        for (x = 0; x < mgX_cnt; x++)
            mg_E_table[x][y] = 0;

    for (mg_cnt = 0; mg_cnt < mg_set_cnt; mg_cnt++)
    {
        x = rand() % mgX_cnt;
        y = rand() % mgY_cnt;
        if (mg_E_table[y][x] == 0)
            mg_E_table[y][x] = '*';
        else mg_cnt--;
    }

    for (y = 0; y < mgY_cnt; y++)
    {
        for (x = 0; x < mgX_cnt; x++)
        {
            if (mg_E_table[y][x] == 0)
            {
                /* 1 2 3
                   4 X 5
                   6 7 8*/
                mg_cnt = 0;
                if ((y - 1) >= 0)
                {
                    if ((x - 1) >= 0 && mg_E_table[y - 1][x - 1] == '*')
                        mg_cnt++;
                    if (mg_E_table[y - 1][x] == '*')
                        mg_cnt++;
                    if ((x + 1) < mgX_cnt && mg_E_table[y - 1][x + 1] == '*')
                        mg_cnt++;
                }
                if ((x - 1) >= 0 && mg_E_table[y][x - 1] == '*')
                    mg_cnt++;
                if ((x + 1) < mgX_cnt && mg_E_table[y][x + 1] == '*')
                    mg_cnt++;
                if ((y + 1) < mgY_cnt)
                {
                    if ((x - 1) >= 0 && mg_E_table[y + 1][x - 1] == '*')
                        mg_cnt++;
                    if (mg_E_table[y + 1][x] == '*')
                        mg_cnt++;
                    if ((x + 1) < mgX_cnt && mg_E_table[y + 1][x + 1] == '*')
                        mg_cnt++;
                }
                mg_E_table[y][x] = '0' + mg_cnt;

            }
        }
    }
}



void mg_easy_set(char mg_E_table[][9]) // 보이지 않는 (x,y)좌표상에 지뢰를 랜덤배치
{
    mg_diff = 0;
    mgX_cnt = 9;
    mgY_cnt = 9;
    mg_set_cnt = 10;
    mg_inst = 0;
    mg_make_mine();
}

void mg_medium_set(char mg_M_table[][16]) // 보이지 않는 (x,y)좌표상에 지뢰를 랜덤배치
{
    mg_diff = 1;
    mgX_cnt = 16;
    mgY_cnt = 16;
    mg_set_cnt = 40;
    mg_inst = 8;
    mg_make_mine();
}
void mg_hard_set(char mg_H_table[16][30]) // 보이지 않는 (x,y)좌표상에 지뢰를 랜덤배치
{
    mg_diff = 2;
    mgX_cnt = 30;
    mgY_cnt = 16;
    mg_set_cnt = 99;
    mg_inst = 8;
    mg_make_mine();
}

void mg_easy() // medium 난이도
{
    mg_easy_set(mg_E_table);
}

void mg_medium() // medium 난이도
{
    mg_medium_set(mg_M_table);
}
void mg_hard() // hard 난이도
{
    mg_hard_set(mg_H_table);
}




void mg_show_title() //선택화면 제목 출력
{
    print_screen(g_mg_screen, g_mg_cur_screen, g_cur_width / 2 - strlen(kMgTitle) / 2, g_cur_height / 4, kMgTitle);
}

void mg_show_list() //선택화면 선택 커서 출력
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

void mg_show_inst()
{
    print_screen(g_mg_screen, g_mg_cur_screen, 1+mg_inst, 15+mg_inst, "이동:방향키  선택:Enter  돌아가기:Esc");
}



void mg_update() //staus 상태
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
            else if (g_mg_cur_sel == 0)
            {
                g_mg_status = KStatus_E_Start;
                mg_easy();
            }
            else if (g_mg_cur_sel == 1)
            {
                g_mg_status = KStatus_M_Start;
                mg_medium();
            }
            else if (g_mg_cur_sel == 2)
            {
                g_mg_status = KStatus_H_Start;
                mg_hard();
            }
        }

        if (g_mg_cur_sel < 0)
            g_mg_cur_sel = 3;
        else if (g_mg_cur_sel > 3)
            g_mg_cur_sel = 0;

    }
    else if (g_mg_status == KStatus_E_Start)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001)
            g_mg_cur_UD -= 1;

        else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
            g_mg_cur_UD += 1;

        else if (GetAsyncKeyState(VK_LEFT) & 0x0001)
            g_mg_cur_LR -= 1;

        else if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
            g_mg_cur_LR += 1;
         
        else if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
            g_mg_status = kStatus_End;
          

        if (g_mg_cur_UD < 0)
            g_mg_cur_UD = 8;
        else if (g_mg_cur_UD > 8)
            g_mg_cur_UD = 0;
        else if (g_mg_cur_LR < 0)
            g_mg_cur_LR = 8;
        else if (g_mg_cur_LR > 8)
            g_mg_cur_LR = 0;
    }

    else if (g_mg_status == KStatus_M_Start)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001)
            g_mg_cur_UD -= 1;

        else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
            g_mg_cur_UD += 1;

        else if (GetAsyncKeyState(VK_LEFT) & 0x0001)
            g_mg_cur_LR -= 1;

        else if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
            g_mg_cur_LR += 1;

       

        else if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
            g_mg_status = kStatus_End;

        if (g_mg_cur_UD < 0)
            g_mg_cur_UD = 15;
        else if (g_mg_cur_UD > 15)
            g_mg_cur_UD = 0;
        else if (g_mg_cur_LR < 0)
            g_mg_cur_LR = 15;
        else if (g_mg_cur_LR > 15)
            g_mg_cur_LR = 0;
    }

    else if (g_mg_status == KStatus_H_Start)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001)
            g_mg_cur_UD -= 1;

        else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
            g_mg_cur_UD += 1;

        else if (GetAsyncKeyState(VK_LEFT) & 0x0001)
            g_mg_cur_LR -= 1;

        else if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
            g_mg_cur_LR += 1;



        else if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
            g_mg_status = kStatus_End;

        if (g_mg_cur_UD < 0)
            g_mg_cur_UD = 15;
        else if (g_mg_cur_UD > 15)
            g_mg_cur_UD = 0;
        else if (g_mg_cur_LR < 0)
            g_mg_cur_LR = 29;
        else if (g_mg_cur_LR > 29)
            g_mg_cur_LR = 0;
    }
}


void mg_render()
{
   

    if (g_mg_status == kStatus_Select)
    {
        mg_show_title();
        mg_show_list();
    }
    else if (g_mg_status == KStatus_E_Start)
    {
        mg_show_E_table();
        mg_show_inst();
    }

    else if (g_mg_status == KStatus_M_Start)
    {
        mg_show_M_table();
        mg_show_inst();
    }

    else if (g_mg_status == KStatus_H_Start)
    {
        mg_show_H_table();
        mg_show_inst();
    }


    flip_screen(g_mg_screen, &g_mg_cur_screen);
}

int mg_main()
{
    mg_init();
    srand((unsigned)time(NULL));

    while (g_mg_status != kStatus_End)
    {
        mg_update();
        mg_render();
    }
    release(g_mg_screen);
    return 0;
}

