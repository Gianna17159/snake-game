#include "game_setup.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "game.h"

// Some handy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define G_CAP_HEX 0x47
#define G_LOW_HEX 0x67
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 20;
    *height_p = 10;
    int* cells = malloc(20 * 10 * sizeof(int));
    *cells_p = cells;
    for (int i = 0; i < 20 * 10; i++) {
        cells[i] = PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 20; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (20 * (10 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 10; ++i) {
        cells[i * 20] = FLAG_WALL;
        cells[i * 20 + 20 - 1] = FLAG_WALL;
    }

    // Set grass cells!
    // Top and bottom edges:
    for (int i = 1; i < 19; ++i) {
        cells[i + 20] = FLAG_GRASS;
        cells[i + (20 * (9 - 1))] = FLAG_GRASS;
    }
    // Left and right edges:
    for (int i = 1; i < 9; ++i) {
        cells[i * 20 + 1] = FLAG_GRASS;
        cells[i * 20 + 19 - 1] = FLAG_GRASS;
    }

    // Add snake
    cells[20 * 2 + 2] = FLAG_SNAKE;

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 3!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    //initialize default board if no user input
    enum board_init_status status = INIT_UNIMPLEMENTED;
    if (board_rep == NULL) {
        status = initialize_default_board(cells_p, width_p, height_p);

        // initialize snake data
        int init_pos = 42;
        snake_p->snake_pos = NULL;
        insert_first(&(snake_p->snake_pos), &init_pos, sizeof(int));
    } else {
        //create user-inputted board w/ custom snake position
        snake_p->snake_pos = NULL;
        status = decompress_board_str(cells_p, width_p, height_p, snake_p,
                                      board_rep);
    }
    //continue setup if custom board is valid
    if (status == INIT_SUCCESS) {
        place_food(*cells_p, *width_p, *height_p);
        g_game_over = 0;
        g_score = 0;
        snake_p->snake_dir = RIGHT;
    }

    return status;
}

/* Takes in a char * representing a string, a pointer to a char* (which is
   empty) and a delimiter. Uses strtok to parse string and store resulting
   tokens in tokens. Returns the number of tokens-1, which represents the number
   of rows stored in tokens
    */
int parse(char* string, char** tokens, char* delim) {
    char* token = strtok(string, delim);
    // board string is invalid
    if (token == NULL) {
        return 1;
    }

    int row_index = 0;
    // fill tokens
    while (token != NULL) {
        tokens[row_index] = token;
        token = strtok(NULL, delim);
        row_index++;
    }
    // number of rows (not including dimensions token)
    return row_index - 1;
}

/* Checks if inputted char c is a valid case for input. If so, flag is set to
    previously defined bit flag. If not, return -1.
*/
int check_row_char(char c) {
    int flag = -1;
    switch (c) {
        case 'E':
            flag = PLAIN_CELL;
            break;
        case 'W':
            flag = FLAG_WALL;
            break;
        case 'G':
            flag = FLAG_GRASS;
            break;
        case 'S':
            flag = FLAG_SNAKE;
            break;
    }
    return flag;
}

/* Calculates cells_pos number based on current row and column number
 */
int cells_pos(int row_num, int col_num, int width) {
    return ((row_num * width) + col_num);
}

/*Returns 1 if inputted char c represents a digit; otherwise, returns 0
 */
int is_a_num(char c) {
    if (c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

/* Checks if inputted char c is a letter, capitalized or lowercase. Otherwise,
    returns 0.
*/
int is_a_let(char c) {
    if (c >= 'A' && c <= 'Z') {
        return 1;
    } else if (c >= 'a' && c <= 'z') {
        return 1;
    }
    return 0;
}
/* Takes in cells pointer and fills a range of cells with given flag
    Arguments:
        -cells_p: pointer to the pointer representing cells array
        -start_pos: the first cell needing to be filled
        -num_cells: the number of cells to fill
        -flag: the flag to set each cell to

*/
void fill_cells(int** cells_p, int start_pos, int num_cells, int flag) {
    int* cells = *cells_p;
    for (int i = 0; i < num_cells; i++) {
        cells[start_pos + i] = flag;
    }
}

/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 3!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p,
                                            size_t* height_p, snake_t* snake_p,
                                            char* compressed) {
    // stores rows after parsing
    char* rows[2048];
    // stores dimension data after parsing
    char* dimensions[3];
    // parsing rows
    char* delim1 = "|";
    // for parsing dimensions
    char* delim2 = "Bx";
    size_t num_rows = parse(compressed, rows, delim1);
    // parse to store dimensions
    parse(rows[0], dimensions, delim2);
    *height_p = atoi(dimensions[0]);
    *width_p = atoi(dimensions[1]);

    int* cells = malloc(*height_p * *width_p * sizeof(int));
    *cells_p = cells;
    int curr_flag = -1;
    int check_snake = 0;

    // dimension check
    if (num_rows != *height_p) {
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    int row_index = 0;
    // iterate through each row string stored in rows
    for (int i = 1; i < (int)num_rows + 1; i++) {
        char* row = rows[i];
        int col_index = 0;

        // iterate through each char in a row
        for (int j = 0; j < (int)strlen(row); j++) {
            char* c = row + j;

            if (is_a_let(*c) == 1) {
                curr_flag = check_row_char(*c);
                // checking for valid letter input
                if (curr_flag == -1) {
                    return INIT_ERR_BAD_CHAR;
                }

            }
            // increase index to keep numbers together
            else if (is_a_num(*c) == 1) {
                if (j + 1 < (int)strlen(row)) {
                    char* next = row + j + 1;
                    while (j < (int)strlen(row) && is_a_num(*next) == 1) {
                        j++;
                        next++;
                    }
                }
                // num cells to mark with current flag
                int num_cells = atoi(c);
                int start_pos = cells_pos(row_index, col_index, *width_p);

                // check that only one snake cell is added
                if (curr_flag == FLAG_SNAKE) {
                    check_snake += num_cells;
                    if (check_snake != 1) {
                        return INIT_ERR_WRONG_SNAKE_NUM;
                    }
                    // initialize snake data
                    snake_p->snake_pos = NULL;
                    node_t** head = &(snake_p->snake_pos);
                    insert_first(head, &start_pos, sizeof(int));
                }
                fill_cells(cells_p, start_pos, num_cells, curr_flag);
                col_index += num_cells;
            }
        }
        // check at the end of every row string for correct num of columns
        if (col_index != (int)*width_p) {
            return INIT_ERR_INCORRECT_DIMENSIONS;
        }
        row_index++;
    }
    // check if there was no snake inputted
    if (check_snake != 1) {
        return INIT_ERR_WRONG_SNAKE_NUM;
    }
    return INIT_SUCCESS;
}
