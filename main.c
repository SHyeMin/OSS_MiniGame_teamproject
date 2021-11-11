﻿#pragma warning( disable : 4996 )

#include "game_api.h"

static int g_status;
static HANDLE g_screen[2];
static int g_cur_screen;
static char g_game_list[5][20] = { "게임1", "게임2", "게임3", "게임4", "종료하기"};
static int g_cur_game;

void init()
{
	g_status = kStatus_Init;
	g_cur_screen = 0;
	g_cur_game = 0;

	init_screen(g_screen, 40, 15);
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
		}

		if (g_cur_game < 0)
			g_cur_game = 4;
		else if (g_cur_game > 4)
			g_cur_game = 0;
	}
}

void render()
{
	clear_screen(g_screen, g_cur_screen, 40, 15);

	if (g_status == kStatus_Select) {
		char title[] = "미니게임 모음";
		print_screen(g_screen, g_cur_screen, 40 / 2 - strlen(title) / 2, 15 / 4, title);

		for (int i = 0; i < 5; i++)
		{
			if (g_cur_game == i)
				print_screen(g_screen, g_cur_screen, 40 / 2 - 5, 15 / 2 + i, "▶");
			else
				print_screen(g_screen, g_cur_screen, 40 / 2 - 5, 15 / 2 + i, "▷");
			print_screen(g_screen, g_cur_screen, 40 / 2 - 2, 15 / 2 + i, g_game_list[i]);
		}
	}

	flip_screen(g_screen, &g_cur_screen);
}

int main()
{
	init();

	while (1)
	{
		update();
		render();
	}

	release(g_screen);
	return 0;
}