#include "config.h"
#include "term.h"
#include "utils.h"

#include <stdio.h>
#include <windows.h>

#define MAX_TITLE_LEN 260

HANDLE hIn, hOut, original_hOut;
TCHAR original_title[MAX_TITLE_LEN];
CONSOLE_SCREEN_BUFFER_INFO csbi;
DWORD mode;

/*
 * Copies a string to an array of type TCHAR.
 */
static void string_to_tchar_array(TCHAR t[], const char *s);

/*
 * Restores the original window title.
 */
static void restore_window_title(void);

/*
 * Initializes the 'cursor_info' structure.
 */
static void get_cursor_info(CONSOLE_CURSOR_INFO *cursor_info);

/*
 * Updates the 'csbi' global variable.
 */
static void update_buffer_info(void);

void init_term(void)
{
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = original_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (original_hOut == INVALID_HANDLE_VALUE
			|| hIn == INVALID_HANDLE_VALUE)
	{
		die("Failed to initialize IO handlers");
	}

	if (!GetConsoleTitle(original_title, MAX_PATH))
	{
		die("Failed to save the window current title");
	}
}

void set_window_title(const char *title)
{
	TCHAR new_title[MAX_TITLE_LEN];
	string_to_tchar_array(new_title, title);
	if (!SetConsoleTitle(new_title))
	{
		die("Failed to set the window title");
	}
	atexit(restore_window_title);
}

void get_window_size(size_t *rows, size_t *cols)
{
	if (!GetConsoleScreenBufferInfo(hOut, &csbi))
	{
		die("Failed to get the window size");
	}
	if (rows != NULL)
	{
		*rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}
	if (cols != NULL)
	{
		*cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}
}

void clear_line(void)
{
	update_buffer_info();
	DWORD cCharsWritten;
	COORD coord = { 0, csbi.dwCursorPosition.Y };
	if (!FillConsoleOutputCharacter(
				hOut,
				(TCHAR)' ',
				csbi.dwSize.X,
				coord,
				&cCharsWritten)
			|| !FillConsoleOutputAttribute(
				hOut,
				csbi.wAttributes,
				csbi.dwSize.X,
				coord,
				&cCharsWritten)
			|| !SetConsoleCursorPosition(
				hOut,
				coord))
	{
		die("Failed to clear a line");
	}
}

void clear_screen(void)
{
	COORD coord = { 0, 0 };
	DWORD cCharsWritten, dwConSize;

	update_buffer_info();
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	if (!FillConsoleOutputCharacter(
				hOut,
				(TCHAR)' ',
				dwConSize,
				coord,
				&cCharsWritten)
			|| !FillConsoleOutputAttribute(
				hOut,
				csbi.wAttributes,
				dwConSize,
				coord,
				&cCharsWritten)
			|| !SetConsoleCursorPosition(
				hOut,
				coord))
	{
		die("Failed to clear the screen");
	}
}

void switch_to_raw_mode(void)
{
	if (!GetConsoleMode(hIn, &mode) || !SetConsoleMode(hIn,
				mode & ~(ENABLE_LINE_INPUT
					| ENABLE_ECHO_INPUT
					| ENABLE_PROCESSED_INPUT
					| ENABLE_PROCESSED_OUTPUT
					| ENABLE_WRAP_AT_EOL_OUTPUT)))
	{
		die("Failed to switch to raw mode");
	}
}

void switch_to_cooked_mode(void)
{
	if (!SetConsoleMode(hIn, mode))
	{
		die("Failed to switch to cooked mode");
	}
}

void switch_to_normal_buffer(void)
{
	if (original_hOut == NULL) return;
	if (!SetConsoleActiveScreenBuffer(original_hOut))
	{
		fprintf(stderr, "%s: %s\n", PROGRAM_NAME,
				"Failed to switch to normal buffer");
		exit(EXIT_FAILURE);
	}
}

void switch_to_alternate_buffer(void)
{
	hOut = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL);
	if (hOut == INVALID_HANDLE_VALUE
			|| !SetConsoleActiveScreenBuffer(hOut))
	{
		die("Failed to switch to alternate buffer");
	}
}

void hide_cursor(void)
{
	CONSOLE_CURSOR_INFO cursor_info;
	get_cursor_info(&cursor_info);
	cursor_info.bVisible = 0;
	if (!SetConsoleCursorInfo(hOut, &cursor_info))
	{
		die("Failed to hide the cursor");
	}
}

void show_cursor(void)
{
	CONSOLE_CURSOR_INFO cursor_info;
	get_cursor_info(&cursor_info);
	cursor_info.bVisible = 1;
	if (!SetConsoleCursorInfo(hOut, &cursor_info))
	{
		die("Failed to show the cursor");
	}
}

void set_cursor_position(size_t row, size_t col)
{
	COORD coord;
	coord.X = (SHORT)col;
	coord.Y = (SHORT)row;
	if (!SetConsoleCursorPosition(hOut, coord))
	{
		die("Failed to set the cursor position");
	}
}

void get_cursor_position(size_t *row, size_t *col)
{
	if (!GetConsoleScreenBufferInfo(hOut, &csbi))
	{
		die("Failed to get the cursor position");
	}
	if (row != NULL)
	{
		*row = csbi.dwCursorPosition.Y;
	}
	if (col != NULL)
	{
		*col = csbi.dwCursorPosition.X;
	}
}

int get_key(void)
{
	INPUT_RECORD input;
	DWORD nread;

	do
	{
		if (!ReadConsoleInput(hIn, &input, 1, &nread))
		{
			die("Failed to get the keycode");
		}
	} while (!(input.EventType == KEY_EVENT
				&& input.Event.KeyEvent.bKeyDown
				&& input.Event.KeyEvent.wVirtualKeyCode));

	return input.Event.KeyEvent.wVirtualKeyCode;
}

void write_term(const char *output)
{
	DWORD len = strlen(output);
	if (!WriteConsole(hOut, output, len, NULL, NULL))
	{
		die("Failed to write a string to the terminal");
	}
}

static void string_to_tchar_array(TCHAR t[], const char *s)
{
	while (*t++ = *s++);
}

static void restore_window_title(void)
{
	if (!SetConsoleTitle(original_title))
	{
		die("Failed to restore the original window title");
	}
}

static void get_cursor_info(CONSOLE_CURSOR_INFO *cursor_info)
{
	if (!GetConsoleCursorInfo(hOut, cursor_info))
	{
		die("Failed to get console cursor information");
	}
}

static void update_buffer_info(void)
{
	if (!GetConsoleScreenBufferInfo(hOut, &csbi))
	{
		die("failed to get the console screen buffer info");
	}
}
