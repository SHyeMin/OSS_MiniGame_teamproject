#pragma warning( disable : 4996 )

#include "game_api.h"
#include "setting.h"
#include "rhythm.h"
#include "mine.h"
#include "blackjack.h"

static int g_status;
static HANDLE g_screen[2];
static int g_cur_screen;
static char g_game_list[5][20] = { "지뢰찾기", "게임2", "블랙잭", "리듬게임", "종료하기"};
static int g_cur_game;

void init()
{
	g_status = kStatus_Init;
	g_cur_screen = 0;
	g_cur_game = 0;

	init_screen(g_screen, kWidth, kHeight);
	hide_cursor(g_screen);

	g_status = kStatus_Select;

	system("title C-Union MiniGame");
}

void update()
{
	if (g_status == kStatus_Select) {
		if (GetAsyncKeyState(VK_UP) & 0x0001)
		{
			g_cur_game -= 1;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x0001)
		{
			g_cur_game += 1;
		}
		else if (GetAsyncKeyState(VK_RETURN) & 0x0001)
		{
			if (g_cur_game == 0)
				mg_main();
			else if (g_cur_game == 3)
				rg_main();
			else if (g_cur_game == 4)
				g_status = kStatus_End;
		}

		if (g_cur_game < 0)
			g_cur_game = 4;
		else if (g_cur_game > 4)
			g_cur_game = 0;
	}
}

void render()
{
	clear_screen(g_screen, g_cur_screen, kWidth, kHeight);

	if (g_status == kStatus_Select) {
		print_screen(g_screen, g_cur_screen, kWidth / 2 - strlen(kTitle) / 2, kHeight / 4, kTitle);

		for (int i = 0; i < 5; i++)
		{
			if (g_cur_game == i)
				print_screen(g_screen, g_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▶");
			else
				print_screen(g_screen, g_cur_screen, kWidth / 2 - 5, kHeight / 2 + i, "▷");
			print_screen(g_screen, g_cur_screen, kWidth / 2 - 2, kHeight / 2 + i, g_game_list[i]);
		}
	}

	flip_screen(g_screen, &g_cur_screen);
}

int main()
{
	init();

	while (g_status != kStatus_End)
	{
		update();
		render();
	}

	release(g_screen);
	return 0;
}