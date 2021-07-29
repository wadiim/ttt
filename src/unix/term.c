#include "config.h"
#include "term.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

/*
 * Reads a single character from input and stores it in the 'c'
 * parameter.
 */
static int get_char(int *c);

void init_term(void)
{
}

void set_window_title(const char *title)
{
	char buff[64] = "\0";
	if (sprintf(buff, "\x1b]0;%s\x7", title) < 0
			|| write(STDOUT_FILENO, buff, sizeof(buff)) == -1)
	{
		die("Failed to set the window title");
	}
}

void get_window_size(size_t *rows, size_t *cols)
{
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        {
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
		{
			die("Failed to get the window size");
		}
		get_cursor_position(rows, cols);
		if (rows != NULL)
		{
			++(*rows);
		}
		if (cols != NULL)
		{
			++(*cols);
		}
	}
        else
        {
		if (rows != NULL)
		{
			*rows = ws.ws_row;
		}
		if (cols != NULL)
		{
			*cols = ws.ws_col;
		}
        }
}

void clear_line(void)
{
	if (write(STDOUT_FILENO, "\x1b[2K", 4) != 4)
	{
		die("Failed to clear a line");
	}
}

void clear_screen(void)
{
	if (write(STDOUT_FILENO, "\x1b[2J", 4) != 4
			|| write(STDOUT_FILENO, "\x1b[H", 3) != 3)
	{
		die("Failed to clear the screen");
	}
}

void switch_to_raw_mode(void)
{
	struct termios raw = orig_termios;
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
	{
		die("Failed to get the terminal attributes");
	}
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
	{
		die("Failed to switch to raw mode");
	}
}

void switch_to_cooked_mode(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
	{
		die("Failed to switch to cooked mode");
	}
}

void switch_to_normal_buffer(void)
{
	if (write(STDOUT_FILENO, "\x1b[?1049l", 8) != 8)
	{
		fprintf(stderr, "%s: %s\n", PROGRAM_NAME,
				"Failed to switch to normal buffer");
		exit(EXIT_FAILURE);
	}
}

void switch_to_alternate_buffer(void)
{
	if (write(STDOUT_FILENO, "\x1b[?1049h\x1b[H", 11) != 11)
	{
		die("Failed to switch to alternate buffer");
	}
}

void hide_cursor(void)
{
	if (write(STDOUT_FILENO, "\x1b[?25l", 6) != 6)
	{
		die("Failed to hide the cursor");
	}
}

void show_cursor(void)
{
	if (write(STDOUT_FILENO, "\x1b[?25h", 6) != 6)
	{
		die("Failed to show the cursor");
	}
}

void set_cursor_position(size_t row, size_t col)
{
	char buff[32] = "\0";
	if (sprintf(buff, "\x1b[%zu;%zuH", row + 1, col + 1) < 0
			|| write(STDOUT_FILENO, buff, sizeof(buff)) == -1)
	{
		die("Failed to set the cursor position");
	}
}

void get_cursor_position(size_t *row, size_t *col)
{
	char buff[32];
	unsigned i = 0;
	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
	{
		die("Failed to get the cursor position");
	}
	while (i < sizeof(buff) - 1)
	{
		if (read(STDIN_FILENO, &buff[i], 1) != 1)
		{
			break;
		}
		if (buff[i] == 'R')
		{
			break;
		}
		++i;
	}
	buff[i] = '\0';
	if (buff[0] != '\x1b' || buff[1] != '[')
	{
		die("Failed to get the cursor position");
	}
	if (row != NULL)
	{
		if (sscanf(&buff[2], "%zu", row) != 1)
		{
			die("Failed to get the cursor row number");
		}
		--(*row);
	}
	if (col != NULL)
	{
		if (sscanf(strchr(buff, ';') + 1, "%zu", col) != 1)
		{
			die("Failed to get the cursor column number");
		}
		--(*col);
	}
}

int get_key(void)
{
	int key = 0;

	while (get_char(&key) != 1)
		;

	if (key == ESC)
	{
		int buff[2] = { 0 };

		if (get_char(&buff[0]) == -1)
		{
			return ESC;
		}
		if (get_char(&buff[1]) == -1)
		{
			return ESC;
		}

		switch (buff[0])
		{
			case '[':
			case 'O':
				switch (buff[1])
				{
					case 'A': return ARROW_UP;
					case 'B': return ARROW_DOWN;
					case 'C': return ARROW_RIGHT;
					case 'D': return ARROW_LEFT;
				}
				break;
		}
	}
	return key;
}

void write_term(const char *output)
{
	if (write(STDOUT_FILENO, output, strlen(output)) == -1)
	{
		die("Failed to write a string to the terminal");
	}
}

static int get_char(int *c)
{
	int nread;
	nread = read(STDIN_FILENO, c, 1);
	if (nread == -1 && errno != EAGAIN)
	{
		die("Failed to read input");
	}
	return nread;
}
