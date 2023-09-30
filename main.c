#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <process.h>
#include <Windows.h>
#include <conio.h>

#define 	_SCREEN_WIDTH_ 		800
#define 	_SCREEN_HEIGHT_ 	450
#define 	_ESCAPE_			27
#define 	false				0
#define		true				1

void initScreen(HANDLE*);
void renderScreen(HANDLE*, int, char*);
void printScreen(HANDLE*, int);
void swapScreen(int*);
unsigned _stdcall getKeyInput(void*);

int main(int argc, char *argv[]) {
	HANDLE screen[2];
	int screenIndex = 0;
	char bufferString[(_SCREEN_WIDTH_*2)*_SCREEN_HEIGHT_+1];
	char input = '\0';
	
	initScreen(screen);
	
	_beginthreadex(NULL, 0, getKeyInput, (void*)&input, 0, NULL);
	
	while (1)
	{
		renderScreen(screen, screenIndex, &input);
		printScreen(screen, screenIndex);
		swapScreen(&screenIndex);
		if (input == _ESCAPE_) return 0;
		Sleep(500);		// 0.5sec
	}
	
	return 0;
}

void initScreen(HANDLE* screen)
{
	char screenInitCommand[64] = "";
	sprintf(screenInitCommand, "mode con:cols=%d lines=%d", _SCREEN_WIDTH_, _SCREEN_HEIGHT_);
	system(screenInitCommand);
	
	screen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	screen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	
	/* Cursor */
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 1;
	cursor.bVisible = false;
	SetConsoleCursorInfo(screen[0], &cursor);
	SetConsoleCursorInfo(screen[1], &cursor);
	
	/* Font */
	CONSOLE_FONT_INFOEX font = {sizeof(font)};
	swprintf(font.FaceName, LF_FACESIZE, L"%s", "±¼¸²Ã¼");
    font.dwFontSize.X = 2;
    font.dwFontSize.Y = 4;
    
    SetCurrentConsoleFontEx(screen[0], 0, &font);
    SetCurrentConsoleFontEx(screen[1], 0, &font);
}
	

void renderScreen(HANDLE* screen, int screenIndex, char* c)
{
	COORD zero = { 0, 0 };
	DWORD dw;
	char bufferString[(_SCREEN_WIDTH_*2)*_SCREEN_HEIGHT_+1] = "";
	struct _CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(screen[screenIndex], &screenInfo);
	sprintf(bufferString, "%s\n{ %d, %d }\n", bufferString, screenInfo.dwMaximumWindowSize.X, screenInfo.dwMaximumWindowSize.Y);
	
	if (*c == 0)
	{
		sprintf(bufferString, "%s\nNo input captured.\n", bufferString);
		SetConsoleCursorPosition(screen[screenIndex], zero);
		SetConsoleTextAttribute(screen[screenIndex], 15 | (0 << 4));
		WriteFile(screen[screenIndex], bufferString, strlen(bufferString), &dw, NULL);
		return;
	}
	
	sprintf(bufferString, "%s\nInput captured : %c\n", bufferString, *c);
	SetConsoleCursorPosition(screen[screenIndex], zero);
	SetConsoleTextAttribute(screen[screenIndex], 15 | (0 << 4));
	WriteFile(screen[screenIndex], bufferString, strlen(bufferString), &dw, NULL);
	
}

void printScreen(HANDLE* screen, int screenIndex)
{
	SetConsoleActiveScreenBuffer(screen[screenIndex]);
}

void swapScreen(int* i)
{
	*i = !(*i);
}

unsigned _stdcall getKeyInput(void* pInput)
{
	char* input = (char*)pInput;
	
	while (1)
	{
		if (_kbhit())
		{
			*input = _getch();
			if (*input == _ESCAPE_) 
				return;
		}
	}
}
