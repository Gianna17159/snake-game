CSCI 0300 Project 1 - Snake
===========================

## Design Overview:
I implemented mostly everything in accordance with the handout. However, my game does not end when the snake runs into itself. I don't think there
are any other bugs.

Helper functions in game_setup.c:

int parse(char *string, char **tokens, char *delim)
int check_row_char(char c)
int cells_pos(int row_num, int col_num, int width)
int is_a_num(char c)
int is_a_let(char c)
void fill_cells(int **cells_p, int start_pos, int num_cells, int flag)

## Collaborators:
none
## Responsible Computing:
Partner CS login: xjquan

## Extra Credit attempted:
none
## How long did it take to complete Snake?
 15 hours
