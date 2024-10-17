#include <stdlib.h>
#include <stdio.h>
#include "Maze.h"
#include "API.h"


// void logmess(char* text) {
//     fprintf(stderr, "%s\n", text);
//     fflush(stderr);
// }
/* update flag for whether goal cell was reached */
void check_start_reached(short * x, short * y, short * found_start) {
    if (*x == START_X && *y == START_Y) {
        *(found_start) = TRUE;
    }
}
/* update flag for whether goal cell was reached */
void check_goal_reached(short * x, short * y, short * found_goal) {
    if (*x == MAZE_SIZE / 2 || *x == MAZE_SIZE / 2 - 1) {
        if (*y == MAZE_SIZE / 2 || *y == MAZE_SIZE / 2 - 1) {
            *(found_goal) = TRUE;
        }
    }
}

/* function for updating the location and direction of mouse 
   the actual "move" function */
void move_dir(Maze * this_maze, short * x, short * y, short * dir) {
    /* x, y are current positions, dir is current directions
        these output params may be updated at the exit of this function */ 
    Node * this_node;   /* the node at this position x, y */ 
    short next_dir;       /* will hold the next direction to go */

    this_node = this_maze->map[(*y)][(*x)];
    next_dir = get_smallest_neighbor_dir(this_node, *dir);
    
    /* update the appropriate location value x or y */
    if (next_dir == NORTH){
        (*y) = (*y) - 1;
        if (*dir == NORTH) {
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == EAST) {
            API_turnLeft(); logmess("Turning Left");
            API_moveForward(); 
        } else if (*dir == SOUTH) {
            API_turnRight(); 
            API_turnRight(); logmess("Turning Back");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == WEST) {
            API_turnRight(); logmess("Turning Right");
            API_moveForward(); logmess("Moving Forward");
        }
    } 
    else if (next_dir == EAST){
        (*x) = (*x) + 1;
        if (*dir == NORTH) {
            API_turnRight(); logmess("Turning Right");
            API_moveForward(); logmess("Moving Forward"); 
        } else if (*dir == EAST) {
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == SOUTH) {
            API_turnLeft(); logmess("Turning Left");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == WEST) {
            API_turnRight(); 
            API_turnRight(); logmess("Turning Back");
            API_moveForward(); logmess("Moving Forward");
        }
    } 
    else if (next_dir == SOUTH){
        (*y) = (*y) + 1;
        if (*dir == NORTH) {
            API_turnRight(); 
            API_turnRight(); logmess("Turning Back");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == EAST) {
            API_turnRight(); logmess("Turning Right");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == SOUTH) {
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == WEST) {
            API_turnLeft();
            API_moveForward(); logmess("Moving Forward");
        }
    } 
    else if (next_dir == WEST){
        (*x) = (*x) - 1;
        if (*dir == NORTH) {
            API_turnLeft(); logmess("Turning Left");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == EAST) {
            API_turnRight(); 
            API_turnRight(); logmess("Turning Back");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == SOUTH) {
            API_turnRight(); logmess("Turning Right");
            API_moveForward(); logmess("Moving Forward");
        } else if (*dir == WEST) {
            API_moveForward(); logmess("Moving Forward");
        }
    }
    /* update the direction */
    *dir = next_dir;
}
/* the function that represents a visiting of a node 
   walls will be checked, and flood fill called apprepriately */
void visit_Node(Maze *this_maze, Stack *this_stack, short x, short y, 
                short dir, char flag) {
    Node *this_node;   /* holds current node at x, y; also for reading from stack */
    int northwall, eastwall, southwall, westwall;  /* for reading in wall data */

    this_node = this_maze->map[y][x];
    northwall = eastwall = southwall = westwall = 0;
    /* reading the existence of walls in each direction, according to wall data 
        in real mouse, walls will be checked by sensor */
    if (API_wallFront()) {
        if (dir == NORTH && y != 0) northwall = TRUE;
        else if (dir == EAST && x != MAZE_SIZE-1 ) eastwall = TRUE;
        else if (dir == SOUTH && y != MAZE_SIZE-1) southwall = TRUE;
        else if (dir == WEST && x != 0) westwall = TRUE;
    }
    if (API_wallRight()) {
        if (dir == NORTH && x != MAZE_SIZE-1) eastwall = TRUE;
        else if (dir == EAST && y != MAZE_SIZE-1) southwall = TRUE;
        else if (dir == SOUTH && x != 0) westwall = TRUE;
        else if (dir == WEST && y != 0) northwall = TRUE;
    }
    if (API_wallLeft()) {
        if (dir == NORTH && x != 0) westwall = TRUE;
        else if (dir == EAST && y != 0) northwall = TRUE;
        else if (dir == SOUTH && x != MAZE_SIZE-1) eastwall = TRUE;
        else if (dir == WEST && y != MAZE_SIZE-1) southwall = TRUE;
    }
    /* push to stack the cell on other side of wall if valid 
        sets the walls as specified by the values checked above */
    if (northwall) {
        if (this_node->row != 0){
            set_wall(this_node, NORTH);
        }
    }
    if (eastwall) {
        if (this_node->column != MAZE_SIZE-1){
            set_wall(this_node, EAST);
        }
    }
    if (southwall) {
        if (this_node->row != MAZE_SIZE-1){
            set_wall(this_node, SOUTH);
        }
    }
    if (westwall) {
        if (this_node->column != 0){
            set_wall(this_node, WEST);
        }
    }
    
    push(this_stack, this_node);
        
    /* pop until the stack is empty, and call flood_fill on that node */  
    while (!is_empty_Stack(this_stack)) {
        pop(this_stack, &this_node);
        flood_fill(this_maze,this_node, this_stack,flag);
    }

    set_visited(this_node);
}



/* MAIN METHOD */
int main(int argc, char ** argv){
    logmess("Running...");
    // API_setColor(0, 0, 'G');
    // API_setText(0, 0, "NBD");

    int exit_solver_loop;
    int trip_count;       /* counts the number of runs from start->goal->start */

    /* solver variables
        these are essential variables for solving the maze.. so will be used on mouse */
    Maze * my_maze;    /* maze for keeping track of flood values and walls */
    Stack * my_stack;  /* stack used for flood fill */
    short found_dest;   /* flag if goal is reached */
    short direction;    /* keeps track of direction that mouse is moving in */
    short x, y; /* keeps track of current row, col value mouse is in within maze */
    short goal_x, goal_y; /* keeps track of goal's x, y, once found */
    Node * temp;  /* used for in-between start->goal, goal->start transition */

    /* allocating maze solving resources */
    my_maze = new_Maze();    /* Initialize new maze */
    my_stack = new_Stack();  /* Initialize new stack */
    
    /* Initialize variables */
    x = START_X;
    y = START_Y;
    direction = NORTH;
    found_dest = FALSE;
    exit_solver_loop = FALSE;
    trip_count = 0;
    /* Solver Loop */
    while (exit_solver_loop == FALSE) {
        // API_moveForward();
        found_dest = FALSE;
        direction = NORTH;
        trip_count++;
        print_maze(my_maze);
        while (found_dest == FALSE) {
            visit_Node(my_maze, my_stack, x, y, direction,  FALSE);
            move_dir(my_maze, &x, &y, &direction);
            check_goal_reached(&x, &y, &found_dest);
            /* negative coord check ... for errors */
            if (x < 0 || y < 0) {
                return FALSE;
            }
        }
        goal_x = x;
        goal_y = y;

        // /*** Reading the walls of the CENTER GOAL CELLS ***/  
        // for (int i = 0; i < 4; i++) {
        //     visit_Node(my_maze, my_stack, x, y, direction, FALSE);  
        //     if (x == MAZE_SIZE / 2 - 1 && y == MAZE_SIZE / 2 - 1 )
        //         x++;
        //     else if ( x == MAZE_SIZE / 2 && y == MAZE_SIZE / 2 - 1 ) 
        //         y++;
        //     else if ( x == MAZE_SIZE / 2 && y == MAZE_SIZE / 2 ) 
        //         x--;
        //     else 
        //         y--;
        // }

        // /*** Reflooding process from start to goal */
        // /* Set everything to 255 ! */
        // for (int i = 0; i < MAZE_SIZE; i++) 
        //     for (int j = 0; j < MAZE_SIZE; j++)
        //         my_maze->map[i][j]->floodval = LARGEVAL;
        
        // /* set the start value to zero */
        // set_value(my_maze->map[START_X][START_Y], 0);

        // /* push the neighbors of start cell to stack 
        // then pop everything until all cells updated*/
        // push_open_neighbors(my_maze->map[START_X][START_Y], my_stack);
        // while(!is_empty_Stack(my_stack)) {
        //     pop(my_stack, &temp);
        //     if (!(temp->row == 15 && temp->column == 0))
        //         flood_fill(my_maze,temp, my_stack, TRUE);
        // }

        // /* reset destination found flag */
        // found_dest = FALSE;

        // /*** TRIP FROM GOAL TO START ***/

        // while (found_dest == FALSE) {
        //     visit_Node(my_maze, my_stack, x, y, direction,  TRUE);
        //     move_dir(my_maze, &x, &y, &direction);
        //     check_start_reached(&x, &y, &found_dest);
        //     if (x < 0 || y < 0) {
        //         return FALSE;
        //     }
        // }

        // /*** Reflooding process from start to goal */
        // /* Set everything to 0 ! */
        // for (int i = 0; i < MAZE_SIZE; i++) 
        //     for (int j = 0; j < MAZE_SIZE; j++)
        //         my_maze->map[i][j]->floodval = 0;
        
        // /* with start as zero, update everycell's floodval */
        // push_open_neighbors(my_maze->map[goal_x][goal_y], my_stack);
        // while(!is_empty_Stack(my_stack)) {
        //     pop(my_stack, &temp);
        //     flood_fill(my_maze,temp, my_stack, FALSE);
        // }

        // /* prompt for another round of exploring the maze */
        // // do {
        // //     ch = getchar();
        // // } while (ch != YES && ch != NO);

        // // if (ch == NO)
        exit_solver_loop = TRUE;

    }



    /* Deallocate Maze and Stack */
    delete_Maze(&my_maze);
    delete_Stack(&my_stack);

    return TRUE;
}

