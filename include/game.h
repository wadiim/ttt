#ifndef GAME_H
#define GAME_H

#include <stddef.h>

typedef struct
{
	int turn;
	char winner;
	char board[3][3];
	struct
	{
		size_t height;
		size_t width;
	} window;
	struct
	{
		size_t row;
		size_t col;
	} cursor, cell;
} Game;

/*
 * Initializes a new round of the tic-tac-toe game.
 */
void init_game(Game *game);

/*
 * Runs the tic-tac-toe game.
 */
void run_game(void);

/*
 * Moves to the cell to the left of the current one.
 */
void move_cell_left(Game *game);

/*
 * Moves to the cell below from the current one.
 */
void move_cell_down(Game *game);

/*
 * Moves to the cell above from the current one.
 */
void move_cell_up(Game *game);

/*
 * Moves to the cell to the right of the current one.
 */
void move_cell_right(Game *game);

/*
 * Generates a grid in the center of the screen.
 */
void generate_grid(Game *game);

/*
 * Checks if there is a winner. If so, it returns the symbol used by the
 * winning player ('X' or 'O'). Otherwise, it returns '\0'.
 */
char check_winner(Game *game);

/*
 * Makes an opponent's turn.
 */
void make_opponent_turn(Game *game);

/*
 * Shows a message above the board.
 */
void show_message(const char *msg, Game *game);

#endif /* GAME_H */
