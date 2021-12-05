#pragma warning( disable : 4996 )
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include "blackjack.h"

static char kBjTitle[7] = "블랙잭";
static int status;

static HANDLE bjScreen[2];
static int screenIndex;

static char titleMenuList[4][9] = { "시작", "규칙", "돌아가기" };
static int titleCursor;
static int screenWidth;
static int screenHeight;

static int countList[2];
static int scoreList[2];
static int cardList[4][13];

void initTitleScreen() {
    status = TITLE_INIT;
    screenIndex = 0;
    titleCursor = 0;
    screenWidth = kWidth;
    screenHeight = kHeight;

    init_screen(bjScreen, screenWidth, screenHeight);
    hide_cursor(bjScreen);

    status = TITLE_SELECT;
}

void renderTitleScreen() {
    print_screen(bjScreen, screenIndex, screenWidth / 2 - strlen(kBjTitle) / 2, screenHeight / 4, kBjTitle);

    for (int i = 0; i < 3; i++) {
        if (titleCursor == i) print_screen(bjScreen, screenIndex, kWidth / 2 - 5, kHeight / 2 + i, "▶");
        else print_screen(bjScreen, screenIndex, kWidth / 2 - 5, kHeight / 2 + i, "▷");

        print_screen(bjScreen, screenIndex, screenWidth / 2 - 2, screenHeight / 2 + i, titleMenuList[i]);
    }
}

void renderGameScreen() {
    for (int index = 0; index < 2; index++) {
        for (int i = 1; i < screenWidth - 2; i++) {
            print_screen(bjScreen, index, i, 0, "━");
            if (i == 15) {
                print_screen(bjScreen, index, i, 6, "┳");
                print_screen(bjScreen, index, i, 15, "┻");
            }
            else {
                print_screen(bjScreen, index, i, 6, "━");
                print_screen(bjScreen, index, i, 15, "━");
            }
			print_screen(bjScreen, index, i, screenHeight - 1, "━");
        }
        for (int i = 0; i < 5; i++) {
			print_screen(bjScreen, index, 0, 1 + i, "┃");
			print_screen(bjScreen, index, screenWidth - 2, 1 + i, "┃");
			print_screen(bjScreen, index, 0, 16 + i, "┃");
			print_screen(bjScreen, index, screenWidth - 2, 16 + i, "┃");
        }
        for (int i = 1; i < 15; i++) {
            print_screen(bjScreen, index, i, 8, "━");
            print_screen(bjScreen, index, i, 13, "━");
        }
        print_screen(bjScreen, index, 0, 0, "┏");
        print_screen(bjScreen, index, 0, 6, "┣");
        print_screen(bjScreen, index, 0, 7, "┃");
        print_screen(bjScreen, index, 0, 8, "┗");
        print_screen(bjScreen, index, 0, 13, "┏");
        print_screen(bjScreen, index, 0, 14, "┃");
        print_screen(bjScreen, index, 0, 15, "┣");
		print_screen(bjScreen, index, 0, screenHeight - 1, "┗");
        print_screen(bjScreen, index, 15, 7, "┃");
        print_screen(bjScreen, index, 15, 8, "┛");
        print_screen(bjScreen, index, 15, 13, "┓");
        print_screen(bjScreen, index, 15, 14, "┃");
        print_screen(bjScreen, index, screenWidth - 2, 0, "┓");
        print_screen(bjScreen, index, screenWidth - 2, 6, "┛");
        print_screen(bjScreen, index, screenWidth - 2, 15, "┓");
        print_screen(bjScreen, index, screenWidth - 2, screenHeight-1, "┛");

        print_screen(bjScreen, index, 2, 7, "딜러");
        print_screen(bjScreen, index, 2, 14, "플레이어");
    }
}

void printScore() {

}

void bjUpdate() {
    if (status == TITLE_SELECT) {
        if (GetAsyncKeyState(VK_UP) & 0x0001) titleCursor--;
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001) titleCursor++;
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
            if (titleCursor == 0) {
                for (int i = 0; i < 2; i++) clear_screen(bjScreen, i, screenWidth, screenHeight);
                screenWidth = 111, screenHeight = 22;
                change_screen(bjScreen, screenWidth, screenHeight);
                status = GAME_INIT;
            }
            if (titleCursor == 1) status = RULE;
            else if (titleCursor == 2) status = TITLE_END;
        }

        if (titleCursor < 0) titleCursor = 2;
        else if (titleCursor > 2) titleCursor = 0;
    }
    else if (status == GAME_INIT) {
        status = GAME_PLAYER;
    }
    else if (status == GAME_PLAYER) {

    }
    else if (status == GAME_DEALER) {

    }
    else if (status == RESULT) {

    }
}

void bjRender() {
    if (status == TITLE_SELECT) {
        renderTitleScreen();
        flip_screen(bjScreen, &screenIndex);
    }
    else if (status == GAME_INIT) {
        renderGameScreen();
        printScore();
    }
    else if (status == GAME_PLAYER) {
    
    }
    else if (status == GAME_DEALER) {

    }
    else if (status == RESULT) {
    
	}
}

int bj_main() {
	initTitleScreen();

	while (status != TITLE_END) {
        bjUpdate();
		bjRender();
    }

    release(bjScreen);
	return 0;
}