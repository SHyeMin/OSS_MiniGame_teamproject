#pragma once
#include <Windows.h>

enum eStatusCode
{
    kStatus_Init,
    kStatus_Select,
    kStatus_End
};
typedef int StatusCode;

void init_screen(HANDLE screen[], int width, int height);
void hide_cursor(HANDLE screen[]);
void print_screen(HANDLE screen[], int index, int x, int y, char* str);
void flip_screen(HANDLE screen[], int* index);
void clear_screen(HANDLE screen[], int index, int width, int height);
void release(HANDLE screen[]);