#include "game.h"
#include "term.h"

#include <stdlib.h>

int main(void)
{
	init_term();
	switch_to_alternate_buffer();
	switch_to_raw_mode();
	run_game();
	switch_to_cooked_mode();
	switch_to_normal_buffer();

	return EXIT_SUCCESS;
}
