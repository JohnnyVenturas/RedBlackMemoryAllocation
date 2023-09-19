#include <stdio.h>

#define REALLOC_CONSTANT 0.21312
#define SPLIT_CONSTANT REALLOC_CONSTANT
extern int mm_init(void);
extern void *mm_malloc(size_t size);
extern void mm_free(void *ptr);
extern void *mm_realloc(void *ptr, size_t size);

/*
 * Students work in teams of one or two.  Teams enter their team name,
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 */
typedef struct {
    char *teamname; /* ID1+ID2 or ID1 */
    char *name1;    /* full name of first member */
    char *id1;      /* login ID of first member */
    char *name2;    /* full name of second member (if any) */
    char *id2;      /* login ID of second member */
} team_t;

extern team_t team;

// This part of the code will be dedicated for the managing of the red black
// tree

void insert_node(size_t *node);
// insert node is not responsible for creating the node. It shall only add it.
// Creation is taken place somewhere else. Mainly, the only way to create a
// block is by a call to find best find. Another way is to free another block
// already createad by find_best_find. It is very important to follow this rule
// to avoid any leakeage of data!

void fix_up(size_t *node);
//recursive helper function for insert_node that fixes the colors of the nodes



size_t *find_best_fit( unsigned int payload_size);
// This function will perform a search in the tree and consequently find the
// best fit free node. In the case the block is too large it shall be split. The
// size ratio is determined by the SPLIT_CONSTANT Which is the same as the
// Realloc Constant

// Return pointer to allocated memory block. In case of split insert the new
// node in the tree.

void delete_node( size_t *node);
// This shall remove the node from the tree. This does not free the node. It
// just Removes it from the three. Freeing the node is the responsibility of the
// user.

size_t *node_to_be_deleted(size_t* start_node,size_t *node);
//in the classical BST deletion we always end up deleting a node that has only one son 
//or is a leaf so we this function will return the address of that node 
//it only looks for node in the subtree starting with node start



int search_node( size_t *node);
//Searches for the node

void rotate_right( size_t *node);
// Takes care of rotating right a node

void rotate_left(size_t *node);
// Takes care of rotating left a node

void* coalesce(void* ptr);
