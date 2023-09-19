/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include "mm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Budura & Simon",
    /* First member's full name */
    "Budura Bogdan",
    /* First member's email address */
    "bogdan.budura@polytechnique.edu",
    /* Second member's full name (leave blank if none) */
    "Simon Sebastian",
    /* Second member's email address (leave blank if none) */
    "sebastian.simon@polytechnique.edu"};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE 4
#define DSIZE 8

#define RED 1
#define BLACK 0
#define LEFT 0
#define RIGHT 1

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) *((unsigned int *)(p)) = val

// The "p" stands for block ptr and therefore the structure is header, block
// ptr, footer

// parent(4),left_child(4), right_child(4), payload_size(4), Footer(4)
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Block Ptr ---- p
#define GET_PAYLOAD_SIZE(p) (*(unsigned int *)((char *)(p)-WSIZE))
#define GET_RIGHT_CHILD(p) *(void **)(((char *)(p)-WSIZE * 2))
#define GET_LEFT_CHILD(p) *(void **)(((char *)(p)-WSIZE * 3))
#define GET_PARENT(p) *(void **)(((char *)(p)-WSIZE * 4))

#define PUT_PAYLOAD_SIZE(p, val) *((unsigned int *)((char *)(p)-WSIZE)) = val

#define PUT_RIGHT_CHILD(p, val) \
    *(void **)(((char *)(p)-WSIZE * 2)) = (void *)val

#define PUT_LEFT_CHILD(p, val) *(void **)(((char *)(p)-WSIZE * 3)) = (void *)val

#define PUT_PARENT(p, val) *(void **)(((char *)(p)-WSIZE * 4)) = (void *)val

#define GET_COLOR(p) (p != NULL ? GET_PAYLOAD_SIZE(p) & 1 : BLACK)
#define SET_COLOR(p, color) \
    (PUT_PAYLOAD_SIZE(p, (GET_PAYLOAD_SIZE(p) & (-2)) + color))

#define OCCUPIED 1
#define FREE 0
#define SET_VACANCY(p, VACANCY) \
    PUT_PAYLOAD_SIZE(p, ((GET_PAYLOAD_SIZE(p) & (-3)) + (VACANCY << 1)))
#define GET_VACANCY(p) ((GET_PAYLOAD_SIZE(p) & 2) >> 1)

#define PUT_FOOTER(p, payload_size) \
    *(size_t *)((char *)(p) + payload_size - WSIZE) = payload_size
#define GET_FOOTER(p) GET(((char *)(p) + GET_PAYLOAD_SIZE(p) - WSIZE))
#define GET_PBLOCK(p) (char *)(p)-4 * WSIZE - GET((char *)(p)-5 * WSIZE)
#define GET_NBLOCK(p) (void *)((char *)(p) + GET_PAYLOAD_SIZE(p) + 4 * WSIZE)

#define PUT_NODE(p, parent, left_child, right_child, payload, color, vacancy) \
    (PUT_PAYLOAD_SIZE(p, ALIGN(payload + WSIZE)),                             \
     PUT_LEFT_CHILD(p, left_child), PUT_PARENT(p, parent),                    \
     SET_COLOR(p, color), PUT_FOOTER(p, GET_PAYLOAD_SIZE(p)),                 \
     SET_VACANCY(p, vacancy))

size_t *root;

/*
 *
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    // Initialization function returns 0 on success an -1 on failiure
    // printf("1 \n");

    if ((root = mem_heap_lo() + 4 * WSIZE) == NULL) {
        fprintf(stderr,
                "An error has occured in mm_init when calling mem_heap_lo");
        return -1;
    }
    // printf("2 \n");
    // printf("MAXHEAP= %d \n", MAX_HEAP);
    PUT_NODE(root, NULL, NULL, NULL, MAX_HEAP - 4 * WSIZE, BLACK, FREE);
    // da segmentation fault printf("3 \n");
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */

// tinem malloc-ul lor pentru debugging
void *mm_malloc(size_t size) {
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);

    if (p == (void *)-1)
        return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

void *mm_malloc_RBT(size_t size) {
    int newsize = ALIGN(
        size +
        SIZE_T_SIZE);  // we use the same allignment as in the example code
    size_t *p = find_best_fit(newsize);
    return p;  // (void *)p ??
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {}
void mm_free_RBT(void *ptr) {
    // cateodata e void* cateodata e size_t * ???
    ptr = coalesce(ptr);
    SET_VACANCY(ptr, FREE);
    insert_node(ptr);
}

void *coalesce(void *ptr) {
    void *nblock = GET_NBLOCK(ptr);
    void *pblock = GET_PBLOCK(ptr);
    if (GET_VACANCY(nblock) == FREE) {
        delete_node(nblock);
        PUT_PAYLOAD_SIZE(
            ptr, GET_PAYLOAD_SIZE(nblock) + GET_PAYLOAD_SIZE(ptr) + 4 * WSIZE);
        PUT_FOOTER(ptr, GET_PAYLOAD_SIZE(ptr));
    }
    if (GET_VACANCY(pblock) == FREE) {
        PUT_PAYLOAD_SIZE(pblock, GET_PAYLOAD_SIZE(ptr) +
                                     GET_PAYLOAD_SIZE(pblock) + 4 * WSIZE);
        PUT_FOOTER(pblock, GET_PAYLOAD_SIZE(pblock));
        ptr = pblock;
    }
    return ptr;
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);

    if (newptr == NULL) return NULL;

    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);

    if (size < copySize) copySize = size;

    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

void *mm_realloc_RBT(void *ptr, size_t size) {
    if (GET_PAYLOAD_SIZE(ptr) >= size) {
        // we don't need to reallocate
        return ptr;
    }

    // if there is not enough memory we do the same as in mm_realloc
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL) return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);

    if (size < copySize) copySize = size;

    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

void insert_node(size_t *node) {
    // we follow the cases in
    // https://www.lkouniv.ac.in/site/writereaddata/siteContent/202004061919579981anshu_singh_engg_Red_black_tree.pdf

    if (root == NULL) {
        fprintf(stderr, "Error: Root is never supposed to be NULL");
    }

    // we first do a normal BST insertion (1), note that payload ~ key
    size_t *current = root;  // not NULL so (2) does not happen

    int node_size = GET_PAYLOAD_SIZE(node);

    SET_COLOR(node, RED);
    while (1) {
        if (node_size < GET_PAYLOAD_SIZE(current)) {
            // we should go left
            if (GET_LEFT_CHILD(current) == NULL) {
                PUT_LEFT_CHILD(current, node);  // we add the node to the tree
                PUT_PARENT(node, current);  // add current as the parent of node
                break;
            }
            current = GET_LEFT_CHILD(current);

        } else {
            // we should go right
            if (GET_RIGHT_CHILD(current) == NULL) {
                PUT_RIGHT_CHILD(current, node);  // we add the node to the tree
                PUT_PARENT(node, current);  // add current as the parent of node
                break;
            }
            current = GET_RIGHT_CHILD(current);
        }
    }

    // now current is the father of node

    // note that the node added can't be the root since the tree is never empty

    fix_up(node);
}

void fix_up(size_t *node) {
    if (node == root) {
        // case 2
        SET_COLOR(node, BLACK);
        return;
    }

    size_t *parent = GET_PARENT(node);

    // if the color of current is black we still have a RBT so we are done
    if (GET_COLOR(parent) == BLACK) {
        return;
    }

    // if we are here the parent is red so we are in (3)

    size_t *grand_parent = GET_PARENT(parent), *uncle;

    // we need to indentify the uncle:
    if (GET_LEFT_CHILD(grand_parent) == parent) {
        uncle = GET_RIGHT_CHILD(grand_parent);
    } else {
        uncle = GET_LEFT_CHILD(grand_parent);
    }

    if (GET_COLOR(uncle) == RED) {
        // case 3.1
        SET_COLOR(parent, BLACK);
        SET_COLOR(uncle, BLACK);
        SET_COLOR(grand_parent, RED);
        fix_up(grand_parent);
    } else {
        // case 3.2
        // now we need to take 4 cases depending on if the links between node
        // parent and grandparent are left/right, which we will denote by
        // link_p, and link_gp

        int link_p, link_gp;
        if (GET_LEFT_CHILD(parent) == node) {
            link_p = LEFT;
        } else {
            link_p = RIGHT;
        }

        if (GET_LEFT_CHILD(grand_parent) == parent) {
            link_gp = LEFT;
        } else {
            link_gp = RIGHT;
        }

        if (link_gp == LEFT && link_p == LEFT) {
            // left-left case
            // swap colors of parent and grandparent
            SET_COLOR(parent, BLACK);
            SET_COLOR(grand_parent, RED);

            //
            rotate_right(grand_parent);
        }
        if (link_gp == LEFT && link_p == RIGHT) {
            rotate_left(parent);
            // now we do the same as in the previous case
            // swap colors of node and grandparent
            SET_COLOR(node, BLACK);
            SET_COLOR(grand_parent, RED);
            rotate_right(grand_parent);
        }
        if (link_gp == RIGHT && link_p == RIGHT) {
            // swap colors of parent and grandparent

            SET_COLOR(parent, BLACK);
            SET_COLOR(grand_parent, RED);
            rotate_left(grand_parent);
        }
        if (link_gp == RIGHT && link_p == LEFT) {
            rotate_right(parent);
            // swap colors of node and grandparent
            SET_COLOR(node, BLACK);
            SET_COLOR(grand_parent, RED);
            rotate_left(grand_parent);
        }
    }
}

void rotate_right(size_t *node) {
    size_t *left_son = GET_LEFT_CHILD(node);
    size_t *t3 = GET_RIGHT_CHILD(left_son);

    if (node != root) {
        // if node is not root we have to redirect the parent of node
        size_t *parent = GET_PARENT(node);
        PUT_PARENT(left_son, parent);
        if (GET_LEFT_CHILD(parent) == node) {
            PUT_LEFT_CHILD(parent, left_son);
        } else {
            PUT_RIGHT_CHILD(parent, left_son);
        }
    } else {
        root = left_son;
    }

    PUT_RIGHT_CHILD(left_son, node);
    PUT_PARENT(node, left_son);
    PUT_LEFT_CHILD(node, t3);
    PUT_PARENT(t3, node);
}

void rotate_left(size_t *node) {
    size_t *right_son = GET_RIGHT_CHILD(node);
    size_t *t2 = GET_LEFT_CHILD(right_son);

    if (node != root) {
        // if node is not root we have to redirect the parent of node
        size_t *parent = GET_PARENT(node);
        PUT_PARENT(right_son, parent);
        if (GET_LEFT_CHILD(parent) == node) {
            PUT_LEFT_CHILD(parent, right_son);
        } else {
            PUT_RIGHT_CHILD(parent, right_son);
        }
    } else {
        root = right_son;
    }

    PUT_LEFT_CHILD(right_son, node);
    PUT_PARENT(node, right_son);
    PUT_RIGHT_CHILD(node, t2);
    PUT_PARENT(t2, node);
}

// NOTE: WE DON'T ACCOUNT FOR THE CASE IN WHICH THE ROOT CHANGES!!!!!!!!!!!!
// 2 optini: ori facem un while dupa insert si delete si mergem in sus si
// reassignuim root ul la icne trebuie
//  ori facem niste if-uri sa vedem daca schimbam fix root ul

size_t *find_best_fit(unsigned int payload_size) {
    size_t *current = root;
    size_t *ans = NULL;
    while (current != NULL) {
        if (GET_PAYLOAD_SIZE(current) < payload_size) {
            // we have to go right, ans dosn't update
            current = GET_RIGHT_CHILD(current);
        } else {
            // we have to go left
            ans = current;
            current = GET_LEFT_CHILD(current);
        }
    }
    // now in ans we have a node that has the least payload larger than
    // payload_size if it exists

    if (ans == NULL) {
        return ans;
    }
    // WE DON't SPLIT IF:
    if (GET_PAYLOAD_SIZE(ans) - payload_size - 5 * WSIZE >=
        SPLIT_CONSTANT * payload_size) {
        // we do not split and keep the extra memory in case of a realloc, so we
        // allocate the whole block

        delete_node(ans);
        return ans;
    } else {
        // we need to split, but we also need to include the new header and
        // footer
        void *new_node_start = (char *)ans + payload_size + 4 * WSIZE;
        PUT_NODE(new_node_start, NULL, NULL, NULL,
                 GET_PAYLOAD_SIZE(ans) - payload_size - 4 * WSIZE, BLACK, FREE);

        PUT_PAYLOAD_SIZE(ans, payload_size);  // this nodes gets occupied so we
                                              // remove it from the tree
        PUT_FOOTER(ans, payload_size);
        delete_node(ans);
        insert_node(new_node_start);
        return ans;
    }
}

size_t *node_to_be_deleted(size_t *start_node, size_t *node) {
    if (start_node == NULL)
        fprintf(stderr, "Error: node_to_be_deleted callled with NULL start");

    if (GET_PAYLOAD_SIZE(node) < GET_PAYLOAD_SIZE(start_node)) {
        // recurse on the left subbranch
        return node_to_be_deleted(GET_LEFT_CHILD(start_node));
    }
    if (GET_PAYLOAD_SIZE(node) > GET_PAYLOAD_SIZE(start_node)) {
        // recurse on the right subbranh
        return node_to_be_deleted(GET_RIGHT_CHILD(start_node));
    }
    // if we are here start_node is the node to be deleted

    if (GET_LEFT_CHILD(node) == NULL || GET_RIGHT_CHILD == NULL) {
        // if it doesn't have 2 children node will be deleted
        return node;
    }

    // if we are here the node has 2 children and we want to find the smallest
    // succesor
    size_t *leftchild = GET_LEFT_CHILD(node);
}

void delete_node(size_t *node) {
    // we need to start by performing the standard delete of a BST

    return;
}

