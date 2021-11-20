#pragma once
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include <process.h>
#include "setting.h"
#include "game_api.h"

typedef struct
{
    int x;
    int y;

    int visible;
} Note;

int rg_main();