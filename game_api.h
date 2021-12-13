#pragma once
#include <Windows.h>

enum eStatusCode
{
    kStatus_Init,
    kStatus_Select,
    kStatus_Diff,
    kStatus_Play,
    kStatus_Save,
    kStatus_Rank,
    kStatus_End,
    KStatus_E_Start,
    KStatus_M_Start,
    KStatus_H_Start,
    KStatus_Set,
    KStatus_Result,
    KStatus_Change
};
typedef int StatusCode;

enum eKeyCode
{
    kKey_Q = 81,
    kKey_W = 87,
    kKey_E = 69,
    kKey_R = 82,
    kKey_T = 84
};
typedef int KeyCode;

void init_screen(HANDLE screen[], int width, int height);
void change_screen(HANDLE screen[], int width, int height);
void hide_cursor(HANDLE screen[]);
void set_color(HANDLE screen[], int index, int text_color, int back_color);
void print_screen(HANDLE screen[], int index, int x, int y, char* str);
void print_screen_with_color(HANDLE screen[], int index, int x, int y, char* str, int color);
void flip_screen(HANDLE screen[], int* index);
void clear_screen(HANDLE screen[], int index, int width, int height);
void release(HANDLE screen[]);