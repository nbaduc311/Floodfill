#include <stdlib.h>
#include <stdio.h>
#include "Maze.h"
#include "API.h"

void logmess(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}
void lognum(short i) {
    fprintf(stderr, "%d\n", i);
    fflush(stderr);
}
/* helper function for flood_fill 
   checks if this node already fulfills flood value requirements*/
short floodval_check(Node * this_node) {
	if (get_smallest_neighbor(this_node) + 1 == this_node->floodval){
		return TRUE;
	}
	return FALSE;
}
/* helper fuction for flood_fill 
   updates this node's flood value to 1 greater than the smallest neighbor*/
void update_floodval(Node * this_node) {
	/* set this node's value to 1 + min open adjascent cell */
	this_node->floodval = get_smallest_neighbor(this_node) + 1;
	// logmess("Floodfilling");
	API_setNumber(this_node->column, MAZE_SIZE - 1 - this_node->row, this_node->floodval);
}
/*** Constructors and Destructors for: Node, Maze, Stack ***/
/* pushes the open neighboring cells of this node to the stack */
void push_neighbors(Maze * this_maze,Node * this_node, Stack *this_stack) {
	/* push open neighbors to stack */
	short x = this_node->column;
	short y = this_node->row;

	if (x > 0 ) push(this_stack, this_maze->map[y][x-1]);
	if (x < MAZE_SIZE - 1) push(this_stack, this_maze->map[y][x+1]);
	if (y > 0) push(this_stack, this_maze->map[y-1][x]);
	if (y < MAZE_SIZE - 1) push(this_stack, this_maze->map[y+1][x]);
}
//Node constructor
Node *new_Node(const short x, const short y) {
	Node *this_node;	//initialize a Node *
	short halfsize;		//initialize the middle point of the size of maze
	
	this_node = (Node *)malloc(sizeof(Node)); //allocate memory for the node pointer
	halfsize = MAZE_SIZE/2;

	//initialize the node's x, y coordinates and the 'VISITED' flag to false
	this_node->row = y;
	this_node->column = x;
	this_node->visited = FALSE;
	//initializing flood value at coord
	//only works when SIZE is even -- okay, but can we do better?
	if (x < halfsize && y < halfsize)
		this_node->floodval = (halfsize - 1 - x) + (halfsize - 1 - y);
	else if (x < halfsize && y >= halfsize)
		this_node->floodval = (halfsize - 1 - x) + (y - halfsize);
	else if (x >= halfsize && y < halfsize)
		this_node->floodval = (x - halfsize) + (halfsize - 1 - y);
	else
		this_node->floodval = (x - halfsize) + (y - halfsize);

	return this_node;	//return the initialized node
}

//Maze constructor
Maze *new_Maze() {
	Maze *this_maze;		//intialize maze *
	short x, y;				//initialize x,y coords

	this_maze = (Maze *)malloc(sizeof(Maze));	//allocate memory for maze*

	//allocate new Node for each coord of the maze
	for(x = 0; x < MAZE_SIZE; ++x)
		for(y = 0; y < MAZE_SIZE; ++y)
			this_maze->map[y][x] = new_Node(x, y);

	//set neighborhood pointers: up, down, right, left
	for(y = 0; y < MAZE_SIZE; y++){
		for(x = 0; x < MAZE_SIZE; x++) {
			if (y == 0) {
				this_maze->map[y][x]->up = NULL;
				API_setWall(x, MAZE_SIZE - 1 - y, 'n');
			} else {
				this_maze->map[y][x]->up = this_maze->map[y-1][x];
			}

			if (y == MAZE_SIZE-1) {
				this_maze->map[y][x]->down = NULL;
				API_setWall(x, MAZE_SIZE - 1 - y, 's');
			} else {
				this_maze->map[y][x]->down = this_maze->map[y+1][x];
			}

			if (x == 0) {
				this_maze->map[y][x]->left = NULL;
				API_setWall(x, MAZE_SIZE - 1 - y, 'w');
			} else {
				this_maze->map[y][x]->left = this_maze->map[y][x-1];
			}

			if (x == MAZE_SIZE-1) {
				API_setWall(x, MAZE_SIZE - 1 - y, 'e');
				this_maze->map[y][x]->right = NULL;
			} else {
				this_maze->map[y][x]->right = this_maze->map[y][x+1];
			}
		}
	}

	return this_maze;
}

//Node destructor
void delete_Node(Node **npp) {
	free(*npp);		//free memory
	*npp = NULL;	//set node pointers to null
}

//Maze destructor
void delete_Maze(Maze **mpp) {
	short x, y;
	for (x = 0; x < MAZE_SIZE; ++x)
		for(y = 0; y < MAZE_SIZE; ++y)
			delete_Node(&((*mpp)->map[x][y]));	//pass by reference the nodes in the maze 1by1

	free(*mpp);	//free up the memory from malloc
	*mpp = NULL;	//set maze ptr to null
}


/*** Node Functions ***/

/* function for obtaining this_node's smallest neighbor's floodval */
short get_smallest_neighbor(Node * this_node) {
	//the Node's floodval will be 1 higher than neighboring cell
	short smallestneighbor = LARGEVAL;

	//check if left is available and left's right pointer is available 
	if (this_node->left != NULL && (this_node->left->right != NULL) && (this_node->left->floodval) < smallestneighbor){
		smallestneighbor = this_node->left->floodval;
	}
	//check right
	if (this_node->right != NULL && (this_node->right->left != NULL) && (this_node->right->floodval) < smallestneighbor){
		smallestneighbor = this_node->right->floodval;
	}
	//check down
	if (this_node->down != NULL && (this_node->down->up != NULL) && (this_node->down->floodval) < smallestneighbor){
		smallestneighbor = this_node->down->floodval;
	}
	//check up
	if (this_node->up != NULL && (this_node->up->down != NULL) && (this_node->up->floodval) < smallestneighbor){
		smallestneighbor = this_node->up->floodval;
	}
	// API_setNumber(this_node->column, MAZE_SIZE - 1 - this_node->row, smallestneighbor);
	return smallestneighbor;
}

/* function for obtaining this nodes's smallest neighbor's direction */
short get_smallest_neighbor_dir(Node *this_node, const short preferred_dir) {
    short smallestval; /* smallest neighbor value */

    /* Lấy giá trị nhỏ nhất từ các ô lân cận */
    smallestval = get_smallest_neighbor(this_node);

	/* Nếu không có ô lân cận nào mở, trả về -1 */
    /* Ưu tiên hướng đi theo preferred_dir trước nếu giá trị bằng smallestval */
    if ((preferred_dir == NORTH) && (this_node->up != NULL) && (this_node->up->floodval == smallestval))
        return NORTH;
    else if ((preferred_dir == EAST) && (this_node->right != NULL) && (this_node->right->floodval == smallestval))
        return EAST;
    else if ((preferred_dir == SOUTH) && (this_node->down != NULL) && (this_node->down->floodval == smallestval))
        return SOUTH;
    else if ((preferred_dir == WEST) && (this_node->left != NULL) && (this_node->left->floodval == smallestval))
        return WEST;

    /* Nếu không cần ưu tiên, chỉ cần trả về hướng có giá trị nhỏ nhất */
    if ((this_node->up != NULL) && (this_node->up->floodval == smallestval))
        return NORTH;
    else if ((this_node->right != NULL) && (this_node->right->floodval == smallestval))
        return EAST;
    else if ((this_node->down != NULL) && (this_node->down->floodval == smallestval))
        return SOUTH;
    else
        return WEST;
}

/* main function for updating the flood values of this node */
void flood_fill(Maze * this_maze,Node * this_node, Stack * this_stack,const short reflood_flag) {
	short status = FALSE; 
	if (!reflood_flag){
		if (this_node->row == MAZE_SIZE / 2 || this_node->row == MAZE_SIZE / 2 - 1){
    		if (this_node->column == MAZE_SIZE / 2 || this_node->column == MAZE_SIZE / 2 - 1) return;
		}
	}
    /* we want to avoid flooding the goal values - this is reverse */
	if (reflood_flag) {
		if (this_node->row == START_Y && this_node->column == START_X)	return;
	}

	status = floodval_check(this_node);			
	/* if no, update curent cell's flood values, 
	   then push open adjascent neighbors to stack. */
	
	if (status == FALSE) {
		update_floodval(this_node); /* Update floodval to 1 + min open neighbor */
		push_neighbors(this_maze,this_node, this_stack); /* pushed, to be called later */
	}
}
/* Function for setting this node's floodval to a specific value */
void set_value(Node * this_node, const short value) {
	this_node->floodval = value;
	API_setNumber(this_node->column, MAZE_SIZE - 1 - this_node->row, this_node->floodval);
}

void set_visited(Node *this_node) {
	//set visited flag flood value to specified value
	this_node->visited = TRUE;
	API_setColor(this_node->column, MAZE_SIZE - 1 - this_node->row, 'R');
}

/* Function for setting the walls of this node */
void set_wall(Node * this_node, const short dir) {
	/* set a wall, of this node, of the specified direction  */
	switch (dir) {
	case NORTH :
		if (this_node->row != 0) {
			this_node->up = NULL;
			API_setWall(this_node->column, MAZE_SIZE - 1 - this_node->row, 'n');
		}
		break;
	case EAST :
		if (this_node->column != MAZE_SIZE - 1) {
			this_node->right = NULL;
			API_setWall(this_node->column, MAZE_SIZE - 1 - this_node->row, 'e');
		}
		break;
	case SOUTH :
		if (this_node->row != MAZE_SIZE - 1) {
			this_node->down = NULL;
			API_setWall(this_node->column, MAZE_SIZE - 1 - this_node->row, 's');
		}
		break;
	case WEST :
		if (this_node->column != 0) {
			this_node->left = NULL;
			API_setWall(this_node->column, MAZE_SIZE - 1 - this_node->row, 'w');
		}
		break;
	}

}

void print_maze(const Maze * this_maze) {
	short i, j;

	for (i = 0; i < MAZE_SIZE; ++i) {
		for (j = 0; j < MAZE_SIZE; ++j) {
			// API_setColor(i, j, 'R');
			API_setNumber(j, i, this_maze->map[MAZE_SIZE -1-i][j]->floodval);
		} 
	}
}



