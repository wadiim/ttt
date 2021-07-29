#include "game.h"
#include "term.h"

#include <ctype.h>
#include <string.h>

void init_game(Game *game)
{
	game->turn = 1;
	game->winner = '\0';
	game->cell.row = 0;
	game->cell.col = 0;
	for (size_t i = 0; i < 3; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			game->board[i][j] = '\0';
		}
	}
	get_window_size(&game->window.height, &game->window.width);
	hide_cursor();
	clear_screen();
	generate_grid(game);
	game->cursor.row = game->window.height/2 - 2;
	game->cursor.col = game->window.width/2 - 4;
	set_cursor_position(game->cursor.row, game->cursor.col);
	show_cursor();
}

void run_game(void)
{
	Game game;
	init_game(&game);

	int key;
	while (tolower(key = get_key()) != 'q')
	{
		switch (key)
		{
			case 'h':
			case 'H':
			case ARROW_LEFT:
				move_cell_left(&game);
				break;
			case 'j':
			case 'J':
			case ARROW_DOWN:
				move_cell_down(&game);
				break;
			case 'k':
			case 'K':
			case ARROW_UP:
				move_cell_up(&game);
				break;
			case 'l':
			case 'L':
			case ARROW_RIGHT:
				move_cell_right(&game);
				break;
			case ' ':
			case ENTER:
				if (game.board[game.cell.row][game.cell.col] != '\0')
				{
					continue;
				}
				hide_cursor();
				write_term("X");
				set_cursor_position(game.cursor.row,
						game.cursor.col);
				show_cursor();
				game.board[game.cell.row][game.cell.col]
					= 'X';

				if ((game.winner = check_winner(&game)) == 'X')
				{
					goto SHOW_RESULTS;
				}
				else if (game.turn == 9)
				{
					game.winner = '\0';
					goto SHOW_RESULTS;
				}
				++game.turn;

				make_opponent_turn(&game);
				if ((game.winner = check_winner(&game)) == 'O')
				{
					goto SHOW_RESULTS;
				}
				++game.turn;
				break;
		}
		continue;

SHOW_RESULTS:
		if (game.winner == '\0')
		{
			show_message("Draw", &game);
		}
		else if (game.winner == 'X')
		{
			show_message("You win", &game);
		}
		else
		{
			show_message("Game over", &game);
		}

		if (tolower(get_key()) == 'q')
		{
			return;
		}
		else
		{
			init_game(&game);
		}
	}
}

void move_cell_left(Game *game)
{
	if (game->cell.col > 0)
	{
		set_cursor_position(game->cursor.row,
				game->cursor.col - 4);
		game->cursor.col -= 4;
		--game->cell.col;
	}
}

void move_cell_down(Game *game)
{
	if (game->cell.row < 2)
	{
		set_cursor_position(game->cursor.row + 2,
				game->cursor.col);
		game->cursor.row += 2;
		++game->cell.row;
	}
}

void move_cell_up(Game *game)
{
	if (game->cell.row > 0)
	{
		set_cursor_position(game->cursor.row - 2,
				game->cursor.col);
		game->cursor.row -= 2;
		--game->cell.row;
	}
}

void move_cell_right(Game *game)
{
	if (game->cell.col < 2)
	{
		set_cursor_position(game->cursor.row,
				game->cursor.col + 4);
		game->cursor.col += 4;
		++game->cell.col;
	}
}

void generate_grid(Game *game)
{
	size_t row = game->window.height/2 - 2;
	size_t col = game->window.width/2 - 5;
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

char check_winner(Game *game)
{
	/* Check horizontally */
	for (size_t r = 0; r < 3; ++r)
	{
		if (game->board[r][0] == game->board[r][1] &&
				game->board[r][0] == game->board[r][2])
		{
			return game->board[r][0];
		}
	}

	/* Check vertically */
	for (size_t c = 0; c < 3; ++c)
	{
		if (game->board[0][c] == game->board[1][c] &&
				game->board[0][c] == game->board[2][c])
		{
			return game->board[0][c];
		}
	}

	/* Check diagonally */
	if (game->board[0][0] == game->board[1][1]
			&& game->board[0][0] == game->board[2][2])
	{
		return game->board[0][0];
	}
	else if (game->board[0][2] == game->board[1][1]
			&& game->board[0][2] == game->board[2][0])
	{
		return game->board[0][2];
	}

	return '\0';
}

void make_opponent_turn(Game *game)
{
	size_t o_row, o_col;

	if (game->turn == 2 && game->board[1][1] == 'X')
	{
		o_row = 0;
		o_col = 0;
	}
	else if (game->turn == 2)
	{
		o_row = 1;
		o_col = 1;
	}
	else
	{
		for (size_t r = 0; r < 3; ++r)
		{
			for (size_t c = 0; c < 3; ++c)
			{
				if (game->board[r][c] == '\0')
				{
					o_row = r;
					o_col = c;
					game->board[r][c] = 'X';
					if (check_winner(game) == 'X')
					{
						game->board[r][c] = '\0';
						goto MAKE_MOVE;
					}
					game->board[r][c] = '\0';
				}
			}
		}
	}

MAKE_MOVE:
	game->board[o_row][o_col] = 'O';
	hide_cursor();
	set_cursor_position(game->window.height/2 - 2 + 2*o_row,
			game->window.width/2 - 4 + 4*o_col);
	write_term("O");
	set_cursor_position(game->cursor.row,
			game->cursor.col);
	show_cursor();
}

void show_message(const char *msg, Game *game)
{
	hide_cursor();
	set_cursor_position(game->window.height/2 - 4,
			game->window.width/2 - strlen(msg)/2);
	write_term(msg);
	show_cursor();
}
