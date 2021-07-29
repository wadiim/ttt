#ifndef TERM_H
#define TERM_H

#include <stddef.h>

enum Key
{
	ENTER = 13,
	ESC = 27,
	ARROW_LEFT = 37,
	ARROW_UP,
	ARROW_RIGHT,
	ARROW_DOWN
};

/*
 * Performs an initialization necessary for other functions from this
 * header to work properly. Needs to be invoked before any of the
 * functions below.
 */
void init_term(void);

/*
 * Sets the title of the terminal window to 'title'.
 * The original title is restored at the exit of the program.
 */
void set_window_title(const char *title);

/*
 * Sets the 'rows' and 'cols' parameters to the corresponding
 * dimensions of the terminal window.
 */
void get_window_size(size_t *rows, size_t *cols);

/*
 * Clears the line containing the cursor.
 */
void clear_line(void);

/*
 * Clears the whole screen.
 */
void clear_screen(void);

/*
 * Sets the terminal to raw mode, i.e. disables the interpretation of
 * control characters and enables char-by-char input processing.
 */
void switch_to_raw_mode(void);

/*
 * Sets the terminal to cooked mode, i.e. enables the interpretation of
 * control characters (e.g. Control-D) and line-by-line input processing.
 */
void switch_to_cooked_mode(void);

/*
 * Switches to normal screen buffer.
 */
void switch_to_normal_buffer(void);

/*
 * Switches to alternate screen buffer.
 */
void switch_to_alternate_buffer(void);

/*
 * Hides the cursor.
 */
void hide_cursor(void);

/*
 * Shows the cursor.
 */
void show_cursor(void);

/*
 * Moves the cursor to the position specified by 'row' and 'col'
 * parameters.
 */
void set_cursor_position(size_t row, size_t col);

/*
 * Stores the cursor position in the 'row' and 'col' parameters.
 */
void get_cursor_position(size_t *row, size_t *col);

/*
 * Returns the code of the key that is currently being pressed.
 */
int get_key(void);

/*
 * Writes 'output' to the terminal.
 */
void write_term(const char *output);

#endif /* TERM_H */
