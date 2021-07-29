#ifndef GAME_H
#define GAME_H

#include <stddef.h>

/*
 * Runs the tic-tac-toe game.
 */
void run_game(void);

/*
 * Generates a grid in the center of the screen.
 */
void generate_grid(size_t win_height, size_t win_width);

/*
 * Checks if there is a winner. If so, it returns the symbol used by the
 * winning player ('X' or 'O'). Otherwise, it returns '\0'.
 */
char check_winner(char board[9]);

/*
 * Makes an opponent's turn.
 */
void make_opponent_turn(char board[9], int turn,
		size_t *o_row, size_t *o_col);

/*
 * Shows a message above the board.
 */
void show_message(const char *msg, size_t win_height, size_t win_width);

#endif /* GAME_H */
