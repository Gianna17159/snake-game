#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#include "linked_list.h"


// Bitflags enable us to store cell data in integers!
#define PLAIN_CELL 0b0000  // equals 0
#define FLAG_SNAKE 0b0001  // equals 1
#define FLAG_WALL 0b0010   // equals 2
#define FLAG_FOOD 0b0100   // equals 4
#define FLAG_GRASS 0b1000  // equals 8

/**
 * This enum represents the different possible inputs in our snake
 * game. The type corresponding to this enum is `enum input_key` and variables
 * of this type can take on the following values:
 * INPUT_UP, INPUT_DOWN, INPUT_LEFT, INPUT_RIGHT, and INPUT_NONE.
 */
enum input_key { INPUT_UP, INPUT_DOWN, INPUT_LEFT, INPUT_RIGHT, INPUT_NONE };
enum direction { UP, DOWN, LEFT, RIGHT };

/** Global variables for game status.
 *
 * `g_` prefix used by convention to emphasize that these are global.
 *
 *
 * Variables:
 *  - g_game_over: 1 if game is over, 0 otherwise
 *  - g_score: current game score. Starts at 0. 1 point for every food eaten.
 */
extern int g_game_over;  // 1 if game is over, 0 otherwise
extern int g_score;      // game score: 1 point for every food eaten
extern int g_name_len;
extern char* g_name;

/** Snake struct. This struct is not needed until part 3!
 * Fields:
 *  - snake_dir: direction head is moving in
 * -snake_pos: tile snake head currently occupies
 * -snake_len: length of snake
 */

typedef struct snake {
    enum direction snake_dir;
    node_t* snake_pos;
    int snake_len;
} snake_t;

void set_seed(unsigned seed);
unsigned generate_index(unsigned size);

#endif
