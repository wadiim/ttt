#include "game.h"
#include "term.h"

#include <ctype.h>
#include <string.h>

void run_game(void)
{
	char board[9] = { '\0' };

	size_t win_height, win_width;
	get_window_size(&win_height, &win_width);

	size_t cursor_row, cursor_col;
	hide_cursor();
	generate_grid(win_height, win_width);
	cursor_row = win_height/2 - 2;
	cursor_col = win_width/2 - 4;
	set_cursor_position(cursor_row, cursor_col);
	show_cursor();

	int key;
	int turn = 1;
	char winner = '\0';
	size_t cell_row = 0, cell_col = 0;
	while (tolower(key = get_key()) != 'q')
	{
		switch (key)
		{
			case 'h':
			case 'H':
			case ARROW_LEFT:
				if (cell_col > 0)
				{
					set_cursor_position(cursor_row,
							cursor_col - 4);
					cursor_col -= 4;
					--cell_col;
				}
				break;
			case 'j':
			case 'J':
			case ARROW_DOWN:
				if (cell_row < 2)
				{
					set_cursor_position(
							cursor_row + 2,
							cursor_col);
					cursor_row += 2;
					++cell_row;
				}
				break;
			case 'k':
			case 'K':
			case ARROW_UP:
				if (cell_row > 0)
				{
					set_cursor_position(
							cursor_row - 2,
							cursor_col);
					cursor_row -= 2;
					--cell_row;
				}
				break;
			case 'l':
			case 'L':
			case ARROW_RIGHT:
				if (cell_col < 2)
				{
					set_cursor_position(cursor_row,
							cursor_col + 4);
					cursor_col += 4;
					++cell_col;
				}
				break;
			case ' ':
			case ENTER:
				if (board[3*cell_row + cell_col]
						== '\0')
				{
					hide_cursor();
					write_term("X");
					set_cursor_position(cursor_row,
							cursor_col);
					show_cursor();
					board[3*cell_row + cell_col]
						= 'X';

					if ((winner = check_winner(board)) == 'X')
					{
						goto SHOW_RESULTS;
					}
					else if (turn == 9)
					{
						winner = '\0';
						goto SHOW_RESULTS;
					}
					++turn;

					/* Opponent's turn */
					size_t o_row, o_col;
					make_opponent_turn(board, turn,
							&o_row, &o_col);
					hide_cursor();
					set_cursor_position(win_height/2 - 2 + 2*o_row,
							win_width/2 - 4 + 4*o_col);
					write_term("O");
					set_cursor_position(cursor_row,
							cursor_col);
					show_cursor();

					if ((winner = check_winner(board)) == 'O')
					{
						goto SHOW_RESULTS;
					}

					++turn;
				}
				break;
		}
		continue;

SHOW_RESULTS:
		if (winner == '\0')
		{
			show_message("Draw", win_height, win_width);
		}
		else if (winner == 'X')
		{
			show_message("You win", win_height, win_width);
		}
		else
		{
			show_message("Game over", win_height, win_width);
		}

		if (tolower(get_key()) == 'q')
		{
			return;
		}
		else
		{
			for (size_t i = 0; i < 9; ++i)
			{
				board[i] = '\0';
			}
			turn = 1;
			winner = '\0';
			cell_row = 0;
			cell_col = 0;

			clear_screen();
			hide_cursor();
			generate_grid(win_height, win_width);
			cursor_row = win_height/2 - 2;
			cursor_col = win_width/2 - 4;
			set_cursor_position(cursor_row, cursor_col);
			show_cursor();
		}
	}
}

void generate_grid(size_t win_height, size_t win_width)
{
	size_t row = win_height/2 - 2;
	size_t col = win_width/2 - 5;
	set_cursor_position(row, col);
	write_term("   |   |   ");
	set_cursor_position(row + 1, col);
	write_term("---+---+---");
	set_cursor_position(row + 2, col);
	write_term("   |   |   ");
	set_cursor_position(row + 3, col);
	write_term("---+---+---");
	set_cursor_position(row + 4, col);
	write_term("   |   |   ");
}

char check_winner(char board[9])
{
	/* Check horizontally */
	for (size_t r = 0; r < 3; ++r)
	{
		if (board[3*r] == board[3*r + 1] &&
				board[3*r] == board[3*r + 2])
		{
			return board[3*r];
		}
	}

	/* Check vertically */
	for (size_t c = 0; c < 3; ++c)
	{
		if (board[c] == board[3 + c] &&
				board[c] == board[6 + c])
		{
			return board[c];
		}
	}

	/* Check diagonally */
	if (board[0] == board[4] && board[0] == board[8])
	{
		return board[0];
	}
	else if (board[2] == board[4] && board[2] == board[6])
	{
		return board[2];
	}

	return '\0';
}

void make_opponent_turn(char board[9], int turn, size_t *o_row, size_t *o_col)
{
	if (turn == 2 && board[4] == 'X')
	{
		*o_row = 0;
		*o_col = 0;
	}
	else if (turn == 2)
	{
		*o_row = 1;
		*o_col = 1;
	}
	else
	{
		for (size_t row = 0; row < 3; ++row)
		{
			for (size_t col = 0; col < 3; ++col)
			{
				if (board[3*row + col] == '\0')
				{
					*o_row = row;
					*o_col = col;
					board[3*row + col] = 'X';
					if (check_winner(board) == 'X')
					{
						board[3*row + col] = '\0';
						goto MAKE_MOVE;
					}
					board[3*row + col] = '\0';
				}
			}
		}
	}

MAKE_MOVE:
	board[(*o_row)*3 + *o_col] = 'O';
}

void show_message(const char *msg, size_t win_height, size_t win_width)
{
	hide_cursor();
	set_cursor_position(win_height/2 - 4,
			win_width/2 - strlen(msg)/2);
	write_term(msg);
	show_cursor();
}
