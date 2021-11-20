#pragma once
#include <Windows.h>

enum eStatusCode
{
    kStatus_Init,
    kStatus_Select,
    kStatus_Diff,
    kStatus_Play,
    kStatus_End
};
typedef int StatusCode;

void init_screen(HANDLE screen[], int width, int height);
void change_screen(HANDLE screen[], int width, int height);
void hide_cursor(HANDLE screen[]);
void set_color(HANDLE screen[], int index, int text_color, int back_color);
void print_screen(HANDLE screen[], int index, int x, int y, char* str);
void print_screen_with_color(HANDLE screen[], int index, int x, int y, char* str, int color);
void flip_screen(HANDLE screen[], int* index);
void clear_screen(HANDLE screen[], int index, int width, int height);
void release(HANDLE screen[]);