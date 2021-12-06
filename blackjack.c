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

static char aceKeymap[18] = "[C] 1점  [V] 11점";
static char aceMsg[2][26] = { "A 카드가 나왔습니다.", "합산할 점수를 선택하세요." };

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

void clearLine(HANDLE screen[], int index, int left, int right, int line) {
    COORD coord = { left, line };
    DWORD dw;
	FillConsoleOutputCharacter(screen[index], ' ', right - left, coord, &dw);
}

void clearSquare(HANDLE screen[], int index, int top, int left, int right, int bottom)
{
    COORD coord = { left, top };
    DWORD dw;
	FillConsoleOutputCharacter(screen[index], ' ', (right - left) * (bottom - top), coord, &dw);
}

void printKeymap(char keymap[]) {
    for (int i = 0; i < 2; i++) print_screen(bjScreen, i, 16, 14, keymap);
}

void clearKeymap() {
    for (int i = 0; i < 2; i++) clearLine(bjScreen, i, 16, screenWidth - 1, 14);
}

void printMessage(char msg[2][26]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
			print_screen(bjScreen, i, screenWidth / 2 - strlen(msg[j]) / 2, 10 + j, msg[j]);
        }
    }
}

void clearMessage() {
	for (int i = 0; i < 2; i++) clearSquare(bjScreen, i, 9, 0, screenWidth - 1, 12);
}

void clearScore(int id) {
    if (id == PLAYER) {
        for (int i = 0; i < 2; i++) clearLine(bjScreen, i, 12, 13, 14);
    }
    else if (id == DEALER) {
        for (int i = 0; i < 2; i++) clearLine(bjScreen, i, 12, 13, 7);
    }
}

void printScore(int id) {
    char buffer[3];
    clearScore(id);
    if (id == PLAYER) {
        for (int i = 0; i < 2; i++) print_screen(bjScreen, i, 12, 14, itoa(scoreList[PLAYER], buffer, 10));
    }
    else if (id == DEALER) {
        if (status != GAME_DEALER) {
            for (int i = 0; i < 2; i++) print_screen(bjScreen, i, 12, 7, "??");
        }
        else {
            for (int i = 0; i < 2; i++) print_screen(bjScreen, i, 12, 7, itoa(scoreList[DEALER], buffer, 10));
        }
    }
}

void printCard(int id, int suitId, int numId, int count) {
    char card1[17] = "┌──────┐";
    char card2[12] = "│??    │";
    char card3[11] = "│      │";
    char card4[11] = "│    00│";
    char card5[17] = "└──────┘";
    char* suit = strstr(card2, "??");
    char* num = strstr(card4, "00");

    if (suitId == 0) strncpy(suit, "♠", 2);
	else if (suitId == 1) strncpy(suit, "◆", 2);
    else if (suitId == 2) strncpy(suit, "♥", 2);
    else if (suitId == 3) strncpy(suit, "♣", 2);

    if (numId == 0) strncpy(num, " A", 2);
    else if (numId == 1) strncpy(num, " 2", 2);
    else if (numId == 2) strncpy(num, " 3", 2);
    else if (numId == 3) strncpy(num, " 4", 2);
    else if (numId == 4) strncpy(num, " 5", 2);
    else if (numId == 5) strncpy(num, " 6", 2);
    else if (numId == 6) strncpy(num, " 7", 2);
    else if (numId == 7) strncpy(num, " 8", 2);
    else if (numId == 8) strncpy(num, " 9", 2);
    else if (numId == 9) strncpy(num, "10", 2);
    else if (numId == 10) strncpy(num, " J", 2);
    else if (numId == 11) strncpy(num, " Q", 2);
    else if (numId == 12) strncpy(num, " K", 2);

    if (id == PLAYER) {
        for (int i = 0; i < 2; i++) {
            print_screen(bjScreen, i, 1 + (count * 8), 16, card1);
            print_screen(bjScreen, i, 1 + (count * 8), 17, card2);
            print_screen(bjScreen, i, 1 + (count * 8), 18, card3);
            print_screen(bjScreen, i, 1 + (count * 8), 19, card4);
            print_screen(bjScreen, i, 1 + (count * 8), 20, card5);
        }
    }
    else if (id == DEALER) {
        for (int i = 0; i < 2; i++) {
            print_screen(bjScreen, i, 1 + (count * 8), 1, card1);
            print_screen(bjScreen, i, 1 + (count * 8), 2, card2);
            print_screen(bjScreen, i, 1 + (count * 8), 3, card3);
            print_screen(bjScreen, i, 1 + (count * 8), 4, card4);
            print_screen(bjScreen, i, 1 + (count * 8), 5, card5);
        }
    }
}

int selectAceScore() {
    clearKeymap();
    clearMessage();
    printKeymap(aceKeymap);
    printMessage(aceMsg);
    while (1) {
        if (_kbhit) {
            if (_getch() == 'c') return 1;
            else if (_getch() == 'v') return 11;
        }
    }
}

void hit(int id) {
    int suitId, numId, score;
    
    while (1){
		srand((unsigned int)time(NULL));
        suitId = rand() % 4;
		numId = rand() % 13;
        if (cardList[suitId][numId] == 0) continue;
        else if (cardList[suitId][numId] == 1) {
            cardList[suitId][numId] = 0;
            break;
        }
    }

    printCard(id, suitId, numId, countList[id]);

    if (numId == 0) {
        if (id == PLAYER) {
            score = selectAceScore();
        }
        else if (id == DEALER) score = 11;
    }
    else if (0 < numId && numId < 10) score = numId + 1;
    else if (numId >= 10) score = 10;

	countList[id]++;
    scoreList[id] += score;
    printScore(id);
    clearKeymap();
    clearMessage();
}

void bjUpdate() {
    if (status == TITLE_SELECT) {
        if (GetAsyncKeyState(VK_UP) & 0x0001) titleCursor--;
        else if (GetAsyncKeyState(VK_DOWN) & 0x0001) titleCursor++;
        else if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
            if (titleCursor == 0) {
                for (int i = 0; i < 2; i++) clear_screen(bjScreen, i, screenWidth, screenHeight);
                screenWidth = 99, screenHeight = 22;
                change_screen(bjScreen, screenWidth, screenHeight);

                for (int i = 0; i < 2; i++) scoreList[i] = 0, countList[i] = 0;
                for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 13; j++) cardList[i][j] = 1;
                }
                status = GAME_INIT;
            }
            if (titleCursor == 1) status = RULE;
            else if (titleCursor == 2) status = TITLE_END;
        }

        if (titleCursor < 0) titleCursor = 2;
        else if (titleCursor > 2) titleCursor = 0;
    }
    else if (status == GAME_INIT) {

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
    }
    else if (status == GAME_INIT) {
        renderGameScreen();
        printScore(PLAYER);
        printScore(DEALER);
    }
    else if (status == GAME_PLAYER) {
    
    }
    else if (status == GAME_DEALER) {

    }
    else if (status == RESULT) {
    
	}

    flip_screen(bjScreen, &screenIndex);
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