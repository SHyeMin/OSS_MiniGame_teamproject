#pragma warning( disable : 4996 )
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "blackjack.h"

const char kBjTitle[] = "블랙잭";
static int titleStatus;
static HANDLE bjTitleScreen[2];
static HANDLE bjRuleScreen[2];
static int titleScreenIndex;
static char titleMenuList[4][20] = { "시작", "규칙", "돌아가기" };
static int titleMenuIndex;
static int screenWidth;
static int screenHeight;

void initTitleScreen() {
    titleStatus = kStatus_Init;
    titleScreenIndex = 0;
    titleMenuIndex = 0;
    screenWidth = kWidth;
    screenHeight = kHeight;

    init_screen(bjTitleScreen, screenWidth, screenHeight);
    hide_cursor(bjTitleScreen);

    titleStatus = kStatus_Select;
}

void showRuleScreen() {
    init_screen(bjRuleScreen, screenWidth, screenHeight);
    hide_cursor(bjRuleScreen);
    print_screen(bjRuleScreen, 0, 0, 0, "블랙잭 미니게임에 오신 것을 환영합니다!");
	SetConsoleActiveScreenBuffer(bjRuleScreen[0]);

    while (1) {
		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001) {
            release(bjRuleScreen);
            break;
        }
    }
}

void updateCursor() {
    if (titleStatus == kStatus_Select)
    {
        if (GetAsyncKeyState(VK_UP) & 0x0001) {
            titleMenuIndex -= 1;
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001) {
            titleMenuIndex += 1;
        }
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
            if (titleMenuIndex == 1) {
                showRuleScreen();
            }
            else if (titleMenuIndex == 2) {
                titleStatus = kStatus_End;
            }
        }

		if (titleMenuIndex < 0) { titleMenuIndex = 2; }
		else if (titleMenuIndex > 2) { titleMenuIndex = 0; }
    }
}

void printTitleScreenCursor() {
    print_screen(bjTitleScreen, titleScreenIndex, screenWidth / 2 - strlen(kBjTitle) / 2, screenHeight / 4, kBjTitle);
    
    for (int i = 0; i < 3; i++)
    {
        if (titleMenuIndex == i) {
            print_screen(bjTitleScreen, titleScreenIndex, kWidth / 2 - 5, kHeight / 2 + i, "▶");
        }
        else {
            print_screen(bjTitleScreen, titleScreenIndex, kWidth / 2 - 5, kHeight / 2 + i, "▷");
		}
        print_screen(bjTitleScreen, titleScreenIndex, screenWidth / 2 - 2, screenHeight / 2 + i, titleMenuList[i]);
    }
}

void renderCursor()
{
    clear_screen(bjTitleScreen, titleScreenIndex, screenWidth, screenHeight);

    if (titleStatus == kStatus_Select)
    {
        printTitleScreenCursor();
    }

    flip_screen(bjTitleScreen, &titleScreenIndex);
}

int bj_main() {
	initTitleScreen();

	while (titleStatus != kStatus_End) {
        updateCursor();
        renderCursor();
    }

    release(bjTitleScreen);
	return 0;
}