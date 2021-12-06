#pragma once
#include "game_api.h"
#include "setting.h"

enum bjStatusCode {
	TITLE_INIT, TITLE_SELECT, TITLE_END, RULE, 
	GAME_INIT, GAME_PLAYER, GAME_DEALER, GAME_COMPARE, RESULT
};
enum charId { PLAYER, DEALER };
int bj_main();