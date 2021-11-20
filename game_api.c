#include "game_api.h"

void init_screen(HANDLE screen[], int width, int height)
{
    COORD coord_size;
    SMALL_RECT rect_size;

    rect_size.Left = rect_size.Top = 0;
    rect_size.Right = width - 1;
    rect_size.Bottom = height - 1;

    coord_size.X = width;
    coord_size.Y = height;

    for (int i = 0; i < 2; i++) {
        screen[i] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleWindowInfo(screen[i], TRUE, &rect_size);
        SetConsoleScreenBufferSize(screen[i], coord_size);
    }
}

void change_screen(HANDLE screen[], int width, int height)
{
    COORD coord_size;
    SMALL_RECT rect_size;

    rect_size.Left = rect_size.Top = 0;
    rect_size.Right = width - 1;
    rect_size.Bottom = height - 1;

    coord_size.X = width;
    coord_size.Y = height;


    for (int i = 0; i < 2; i++) {
        SetConsoleWindowInfo(screen[i], TRUE, &rect_size);
        SetConsoleScreenBufferSize(screen[i], coord_size);
        SetConsoleWindowInfo(screen[i], TRUE, &rect_size);
    }
}

void hide_cursor(HANDLE screen[])
{
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = 1;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(screen[0], &cci);
    SetConsoleCursorInfo(screen[1], &cci);
}

void print_screen(HANDLE screen[], int index, int x, int y, char* str)
{
    DWORD dw;
    COORD cursor_position = { x, y };
    SetConsoleCursorPosition(screen[index], cursor_position);
    WriteFile(screen[index], str, strlen(str), &dw, NULL);
}

void flip_screen(HANDLE screen[], int* index)
{
    SetConsoleActiveScreenBuffer(screen[*index]);
    *index = !*index;
}

void clear_screen(HANDLE screen[], int index, int width, int height)
{
    COORD coord = { 0, 0 };
    DWORD dw;
    FillConsoleOutputCharacter(screen[index], ' ', width * height, coord, &dw);
}

void release(HANDLE screen[])
{
    CloseHandle(screen[0]);
    CloseHandle(screen[1]);
}
