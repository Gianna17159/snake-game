#include "game.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "linked_list.h"
#include "mbstrings.h"

/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 3!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // `update` should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (`g_score`)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.

    // if game is over, do not update
    if (g_game_over == 1) {
        return;
    }
    // current pos of snake head
    int* old_p = (int*)(get_first(snake_p->snake_pos));
    int old_pos = *old_p;

    // set new snake dir based on key input
    switch (input) {
        case INPUT_NONE:
            break;
        case INPUT_RIGHT:
            snake_p->snake_dir = RIGHT;
            break;
        case INPUT_LEFT:
            snake_p->snake_dir = LEFT;
            break;
        case INPUT_UP:
            snake_p->snake_dir = UP;
            break;
        case INPUT_DOWN:
            snake_p->snake_dir = DOWN;
            break;
    }

    // find new pos based on new dir
    // also find where new snake cell would be in case of growth
    int new_pos = 0;
    int end_snake_offset = 0;
    switch (snake_p->snake_dir) {
        case UP:
            new_pos = old_pos - width;
            end_snake_offset = width;
            break;
        case DOWN:
            new_pos = old_pos + width;
            end_snake_offset = -width;
            break;
        case RIGHT:
            new_pos = old_pos + 1;
            end_snake_offset = -1;
            break;
        case LEFT:
            new_pos = old_pos - 1;
            end_snake_offset = 1;
            break;
    }

    // if snake head collides with wall, end game, exit
    if (cells[new_pos] == FLAG_WALL) {
        g_game_over = 1;
        return;
    }

    // find the current end of the snake and remove from its current cell
    int* end_snake = (int*)get_last(snake_p->snake_pos);
    int end_snake_pos = *end_snake;
    cells[end_snake_pos] = cells[end_snake_pos] ^ FLAG_SNAKE;

    // update cells with new snake head pos
    cells[new_pos] = cells[new_pos] | FLAG_SNAKE;

    // update snake_pos linked list
    remove_last(&(snake_p->snake_pos));
    insert_first(&(snake_p->snake_pos), &new_pos, sizeof(int));

    // handle colliding with food cells
    if (cells[new_pos] == (FLAG_FOOD | FLAG_SNAKE) ||
        cells[new_pos] == (FLAG_FOOD | FLAG_GRASS | FLAG_SNAKE)) {
        cells[new_pos] = cells[new_pos] ^ FLAG_FOOD;
        g_score += 1;

        // re insert removed snake cell if snake is set to grow
        if (growing == 1) {
            int new_end_pos = end_snake_pos;
            cells[new_end_pos] = cells[new_end_pos] | FLAG_SNAKE;
            insert_last(&(snake_p->snake_pos), &new_end_pos, sizeof(int));
        }
        place_food(cells, width, height);
    }
}

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    // check that the cell is empty or only contains grass
    if ((*(cells + food_index) == PLAIN_CELL) ||
        (*(cells + food_index) == FLAG_GRASS)) {
        *(cells + food_index) |= FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - `write_into`: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    printf("Name > ");
    fflush(stdout);
    read(0, write_into, 1000);
    char* check_enter = strtok(write_into, "\n\t");
    while (check_enter == NULL) {
        printf("Name Invalid: must be longer than 0 characters.\n");
        fflush(stdout);
        printf("Name > ");
        fflush(stdout);
        read(0, write_into, 1000);
        check_enter = strtok(write_into, "\n\t ");
    }
}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 3)
 */
void teardown(int* cells, snake_t* snake_p) {
    free(cells);
    while (snake_p->snake_pos != NULL) {
        remove_last(&snake_p->snake_pos);
    }
}
