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

static char aceKeymap[] = "[C] 1점  [V] 11점";
static char hitKeymap[] = "[Z] 히트  [X] 스탠드";
static char exitKeymap[] = "[ESC] 메인화면";
static char aceMsg[2][31] = { "A 카드가 나왔습니다.", "합산할 점수를 선택하세요." };
static char winMsg[2][31] = { "당신이 더 21점에 가깝습니다.", "축하합니다, 당신의 승리입니다." };
static char drawMsg[2][31] = { "서로 점수가 같습니다.", "무승부입니다." };
static char loseMsg[2][31] = { "딜러가 더 21점에 가깝습니다.", "아쉽네요, 당신의 패배입니다." };
static char plBjMsg[2][31] = { "블랙잭!", "대단하군요, 당신의 승리입니다!" };
static char dlBjMsg[2][31] = { "딜러가 블랙잭입니다.", "아쉽네요, 당신의 패배입니다." };
static char plBustMsg[2][31] = { "버스트!", "아쉽네요, 당신의 패배입니다." };
static char dlBustMsg[2][31] = { "딜러 버스트!", "축하합니다, 당신의 승리입니다." };

static int countList[2];
static int scoreList[2];
static int hidden[2];
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

void renderRuleScreen() {
	for (int i = 0; i < 2; i++) {
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 59 / 2, 1, "게임을 시작하면 딜러와 플레이어가 카드를 두 장씩 받습니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 41 / 2, 2, "이때 딜러의 첫 카드는 공개되지 않습니다.");

		print_screen(bjScreen, screenIndex, screenWidth / 2 - 35 / 2, 4, "플레이어가 먼저 게임을 진행합니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 64 / 2, 5, "플레이어는 점수가 21점 미만이면 히트 또는 스테이할 수 있습니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 50 / 2, 7, "히트   : 카드를 추가로 한 장 받습니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 50 / 2, 8, "스테이 : 카드를 더이상 받지 않고 차례를 마칩니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 74 / 2, 10, "점수가 21점이면 블랙잭으로 승리, 21점을 초과하면 버스트로 패배합니다.");

		print_screen(bjScreen, screenIndex, screenWidth / 2 - 49 / 2, 12, "점수는 카드마다 다음과 같이 계산되어 합산됩니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 58 / 2, 14, "2 ~ 10  : 각 숫자와 같은 점수");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 58 / 2, 15, "J, Q, K : 10점");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 58 / 2, 16, "A       : 플레이어는 1점과 11점 중 선택 가능, 딜러는 11점");

		print_screen(bjScreen, screenIndex, screenWidth / 2 - 58 / 2, 18, "플레이어의 차례가 끝나면 이어서 딜러가 게임을 진행합니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 41 / 2, 19, "딜러의 차례가 되면 첫 카드가 공개됩니다.");
		print_screen(bjScreen, screenIndex, screenWidth / 2 - 76 / 2, 21, "딜러의 차례가 끝나면 서로 점수를 비교하여 21점에 더 가까운 쪽이 승리합니다.");

		print_screen(bjScreen, screenIndex, screenWidth / 2 - 15 / 2, screenHeight - 2, "[ESC] 메인화면");

		flip_screen(bjScreen, &screenIndex);
	}
}

void renderGameScreen() {
	for (int it = 0; it < 2; it++) {
		for (int i = 1; i < screenWidth - 2; i++) {
			print_screen(bjScreen, screenIndex, i, 0, "━");

			if (i == 15) {
				print_screen(bjScreen, screenIndex, i, 6, "┳");
				print_screen(bjScreen, screenIndex, i, 15, "┻");
			}
			else {
				print_screen(bjScreen, screenIndex, i, 6, "━");
				print_screen(bjScreen, screenIndex, i, 15, "━");
			}

			print_screen(bjScreen, screenIndex, i, screenHeight - 1, "━");
		}

		for (int i = 0; i < 5; i++) {
			print_screen(bjScreen, screenIndex, 0, 1 + i, "┃");
			print_screen(bjScreen, screenIndex, screenWidth - 2, 1 + i, "┃");
			print_screen(bjScreen, screenIndex, 0, 16 + i, "┃");
			print_screen(bjScreen, screenIndex, screenWidth - 2, 16 + i, "┃");
		}

		for (int i = 1; i < 15; i++) {
			print_screen(bjScreen, screenIndex, i, 8, "━");
			print_screen(bjScreen, screenIndex, i, 13, "━");
		}

		print_screen(bjScreen, screenIndex, 0, 0, "┏");
		print_screen(bjScreen, screenIndex, 0, 6, "┣");
		print_screen(bjScreen, screenIndex, 0, 7, "┃");
		print_screen(bjScreen, screenIndex, 0, 8, "┗");
		print_screen(bjScreen, screenIndex, 0, 13, "┏");
		print_screen(bjScreen, screenIndex, 0, 14, "┃");
		print_screen(bjScreen, screenIndex, 0, 15, "┣");
		print_screen(bjScreen, screenIndex, 0, screenHeight - 1, "┗");

		print_screen(bjScreen, screenIndex, 15, 7, "┃");
		print_screen(bjScreen, screenIndex, 15, 8, "┛");
		print_screen(bjScreen, screenIndex, 15, 13, "┓");
		print_screen(bjScreen, screenIndex, 15, 14, "┃");

		print_screen(bjScreen, screenIndex, screenWidth - 2, 0, "┓");
		print_screen(bjScreen, screenIndex, screenWidth - 2, 6, "┛");
		print_screen(bjScreen, screenIndex, screenWidth - 2, 15, "┓");
		print_screen(bjScreen, screenIndex, screenWidth - 2, screenHeight - 1, "┛");

		print_screen(bjScreen, screenIndex, 2, 7, "딜러");
		print_screen(bjScreen, screenIndex, 2, 14, "플레이어");

		flip_screen(bjScreen, &screenIndex);
	}
}

void clearLine(HANDLE screen[], int index, int left, int right, int line) {
	COORD coord = { left, line };
	DWORD dw;
	FillConsoleOutputCharacter(screen[index], ' ', right - left, coord, &dw);
}

void clearSquare(HANDLE screen[], int index, int top, int left, int right, int bottom) {
	COORD coord = { left, top };
	DWORD dw;
	FillConsoleOutputCharacter(screen[index], ' ', (right - left) * (bottom - top), coord, &dw);
}

void printKeymap(char keymap[]) {
	for (int i = 0; i < 2; i++) {
		print_screen(bjScreen, screenIndex, 17, 14, keymap);
		flip_screen(bjScreen, &screenIndex);
	}
}

void clearKeymap() {
	for (int i = 0; i < 2; i++) clearLine(bjScreen, i, 16, screenWidth - 1, 14);
}

void printMessage(char msg[2][31]) {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			print_screen(bjScreen, i, screenWidth / 2 - strlen(msg[j]) / 2, 10 + j, msg[j]);
		}
	}
}

void clearMessage() {
	for (int i = 0; i < 2; i++) clearSquare(bjScreen, i, 9, 0, screenWidth - 1, 12);
}

void printScore() {
	char buffer[3];

	for (int i = 0; i < 2; i++) {
		print_screen(bjScreen, screenIndex, 12, 14, itoa(scoreList[PLAYER], buffer, 10));

		if (status == GAME_DEALER || status == RESULT) {
			print_screen(bjScreen, screenIndex, 12, 7, itoa(scoreList[DEALER], buffer, 10));
		}
		else {
			print_screen(bjScreen, screenIndex, 12, 7, "?");
		}

		flip_screen(bjScreen, &screenIndex);
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

	if (id == DEALER && status == GAME_INIT && count == 0) {
		char* center = strstr(card3, " ");
		strncpy(suit, "  ", 2);
		strncpy(num, "  ", 2);
		strncpy(center, "  ??  ", 6);
		hidden[0] = suitId;
		hidden[1] = numId;
	}
	else {
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
	}

	if (id == PLAYER) {
		for (int i = 0; i < 2; i++) {
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 16, card1);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 17, card2);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 18, card3);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 19, card4);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 20, card5);
			flip_screen(bjScreen, &screenIndex);
		}
	}
	else if (id == DEALER) {
		for (int i = 0; i < 2; i++) {
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 1, card1);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 2, card2);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 3, card3);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 4, card4);
			print_screen(bjScreen, screenIndex, 1 + (count * 8), 5, card5);
			flip_screen(bjScreen, &screenIndex);
		}
	}
}

void openCard() {
	printCard(DEALER, hidden[0], hidden[1], 0);
}

int selectAceScore() {
	clearKeymap();
	clearMessage();
	printKeymap(aceKeymap);
	printMessage(aceMsg);

	while (1) {
		if (_kbhit()) {
			switch (_getch()) {
			case 'c':
				return 1;
			case 'v':
				return 11;
			}
		}

		flip_screen(bjScreen, &screenIndex);
	}
}

void hit(int id) {
	int suitId, numId, score;

	while (1) {
		srand((unsigned int)time(NULL));
		suitId = rand() % 4;

		srand((unsigned int)time(NULL));
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

	printScore();
	clearKeymap();
	clearMessage();
	Sleep(1000);

	if (scoreList[PLAYER] >= 21) {
		clearKeymap();
		openCard();

		if (scoreList[PLAYER] == 21) printMessage(plBjMsg);
		else printMessage(plBustMsg);

		status = RESULT;
	}
}

void bjUpdate() {
	if (status == TITLE_SELECT) {
		if (GetAsyncKeyState(VK_UP) & 0x0001) titleCursor--;
		else if (GetAsyncKeyState(VK_DOWN) & 0x0001) titleCursor++;
		else if (GetAsyncKeyState(VK_RETURN) & 0x0001) {
			if (titleCursor == 0) {
				for (int i = 0; i < 2; i++) clear_screen(bjScreen, i, screenWidth, screenHeight);
				screenWidth = 100, screenHeight = 22;
				change_screen(bjScreen, screenWidth, screenHeight);

				for (int i = 0; i < 2; i++) scoreList[i] = 0, countList[i] = 0;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 13; j++) cardList[i][j] = 1;
				}

				status = GAME_INIT;
			}

			if (titleCursor == 1) {
				for (int i = 0; i < 2; i++) clear_screen(bjScreen, i, screenWidth, screenHeight);
				screenWidth = 100, screenHeight = 27;
				change_screen(bjScreen, screenWidth, screenHeight);
				status = RULE;
			}
			else if (titleCursor == 2) status = TITLE_END;
		}

		if (titleCursor < 0) titleCursor = 2;
		else if (titleCursor > 2) titleCursor = 0;
	}
	else if (status == RULE) {
		if (_kbhit()) {
			if (_getch() == 27) {
				for (int i = 0; i < 2; i++) clear_screen(bjScreen, i, screenWidth, screenHeight);
				screenWidth = kWidth, screenHeight = kHeight;
				change_screen(bjScreen, screenWidth, screenHeight);
				status = TITLE_SELECT;
			}
		}
	}
	else if (status == GAME_INIT) {
		for (int i = 0; i < 2; i++) hit(DEALER);
		for (int i = 0; i < 2; i++) hit(PLAYER);

		if (scoreList[PLAYER] < 21) status = GAME_PLAYER;
		else if (scoreList[PLAYER] >= 21) {
			if (scoreList[PLAYER] == 21) printMessage(plBjMsg);
			else printMessage(plBustMsg);

			status = RESULT;
		}
	}
	else if (status == GAME_PLAYER) {
		printKeymap(hitKeymap);

		if (_kbhit()) {
			switch (_getch()) {
			case 'z':
				hit(PLAYER);
				break;
			case 'x':
				clearKeymap();

				status = GAME_DEALER;

				openCard();
				printScore();
				Sleep(1000);
				break;
			}
		}
	}
	else if (status == GAME_DEALER) {
		if (scoreList[DEALER] < 17) hit(DEALER);
		else if (17 <= scoreList[DEALER] && scoreList[DEALER] < 21) status = GAME_COMPARE;
		else if (scoreList[DEALER] >= 21) {
			if (scoreList[DEALER] == 21) printMessage(dlBjMsg);
			else printMessage(dlBustMsg);
			status = RESULT;
		}
	}
	else if (status == GAME_COMPARE) {
		if (scoreList[PLAYER] > scoreList[DEALER]) printMessage(winMsg);
		else if (scoreList[PLAYER] == scoreList[DEALER]) printMessage(drawMsg);
		else printMessage(loseMsg);
		status = RESULT;
	}
	else if (status == RESULT) {
		if (_getch() == 27) {
			for (int i = 0; i < 2; i++) clear_screen(bjScreen, i, screenWidth, screenHeight);
			screenWidth = kWidth, screenHeight = kHeight;
			change_screen(bjScreen, screenWidth, screenHeight);
			status = TITLE_SELECT;
		}
	}
}

void bjRender() {
	if (status == TITLE_SELECT) {
		renderTitleScreen();
		flip_screen(bjScreen, &screenIndex);
	}
	else if (status == RULE) {
		renderRuleScreen();
	}
	else if (status == GAME_INIT || status == GAME_PLAYER || status == GAME_DEALER) {
		renderGameScreen();
	}
	else if (status == RESULT) {
		openCard();
		printScore();
		printKeymap(exitKeymap);
		renderGameScreen();
		flip_screen(bjScreen, &screenIndex);
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