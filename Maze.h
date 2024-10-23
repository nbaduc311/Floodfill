#ifndef Floodfill_H_
#define Floodfill_H_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAZE_SIZE 16	//the size of one dimension of Map

//define directions
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

//define stack structure constants
#define SPI 1 	//stack pointer index
#define SSI 0	//stack size index (points to size)
#define STACK_OFFSET 2
#define STACKSIZE 80	//defines the size of the stack

// Solver Constants used on mouse
#define LARGEVAL 255
#define START_X 0
#define START_Y 15

// Solver Constants - for command line simulation only
#define NEWLINE 13
#define YES 'y'
#define NO 'n'

/* Main template constants */
#define ONECELL 61
#define LEFT_WALL_SENSED 1700
#define FRONT_WALL_SENSED 2200
#define RIGHT_WALL_SENSED 1700
#define LEFT_BASE_SPEED 23000
#define RIGHT_BASE_SPEED 23000
#define P_VAL 7
#define D_VAL 0
#define TURN_LEFT_COUNT 17
#define TURN_RIGHT_COUNT 18
#define ABOUT_FACE_COUNT 39
#define CENTER 2000
#define LEFT_TWO_AWAY 570
#define RIGHT_TWO_AWAY 630

/* 
Defines the node for the floodfill algorithm to fill up
*/
typedef struct Node {
	/* data fields */
	short floodval;
	short row;
	short column;
	short visited;

	/* pointers to neighbors */
	struct Node *left;
	struct Node *right;
	struct Node *up;
	struct Node *down;
} Node;

/*
Defines the Maze struct that holds a 2D array of Node pointers
*/
typedef struct Maze {
	Node *map[MAZE_SIZE][MAZE_SIZE];
} Maze;

typedef struct Stack {
	short properties[STACK_OFFSET]; //creates a short array of size stack_offset
	Node *the_stack[STACKSIZE];	//creates a Node pointer array of size stacksize
} Stack;

//Node functions
Node *new_Node(const short x, const short y);
void delete_Node(Node **npp);
void flood_fill(Maze * this_maze,Node *this_node, Stack *this_stack,const short reflood_flag);
void set_value(Node *this_node, const short value);
void set_wall(Node * this_node, const short dir);
void set_visited(Node * this_node);

// Floodfill Helper Functions
short get_smallest_neighbor(Node * this_node);
short get_smallest_neighbor_dir(Node * this_node, const short preferred_dir);
short floodval_check(Node * this_node) ;
void update_floodval(Node * this_node);
void push_neighbors(Maze * this_maze,Node * this_node, Stack * this_stack);

// Maze Functions
Maze * new_Maze();
void delete_Maze(Maze ** mpp);
void print_maze(const Maze * this_maze);

//Stack functions
Stack *new_Stack();
void delete_Stack(Stack **spp);
int is_empty_Stack(Stack *this_stack);
void pop(Stack *this_stack, Node **npp);
void push(Stack *this_stack, Node *this_node);

void logmess(char* text);
void lognum(short i);

#endif /* Floodfill_H_ */