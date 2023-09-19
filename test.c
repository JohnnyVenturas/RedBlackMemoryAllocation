#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

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
#define GET_RIGHT_CHILD(p) *(unsigned int **)(((char *)(p)-WSIZE * 2))
#define GET_LEFT_CHILD(p) *(unsigned int **)(((char *)(p)-WSIZE * 3))
#define GET_PARENT(p) *(unsigned int **)(((char *)(p)-WSIZE * 4))

#define PUT_PAYLOAD_SIZE(p, val) *((unsigned int *)((char *)(p)-WSIZE)) = val

#define PUT_RIGHT_CHILD(p, val) \
    *(unsigned int **)(((char *)(p)-WSIZE * 2)) = (unsigned int *)val

#define PUT_LEFT_CHILD(p, val) \
    *(unsigned int **)(((char *)(p)-WSIZE * 3)) = (unsigned int *)val

#define PUT_PARENT(p, val) *(unsigned int **)(((char *)(p)-WSIZE * 4)) = val

#define GET_COLOR(p) (p != NULL ? GET_PAYLOAD_SIZE(p) & 1 : BLACK)
#define SET_COLOR(p, color) \
    (PUT_PAYLOAD_SIZE(p, (GET_PAYLOAD_SIZE(p) & (-2)) + color))

#define OCCUPIED 1
#define FREE 0
#define SET_VACANCY(p, VACANCY) \
    PUT_PAYLOAD_SIZE(p, ((GET_PAYLOAD_SIZE(p) & (-3)) + (VACANCY << 1)))
#define GET_VACANCY(p) ((GET_PAYLOAD_SIZE(p) & 2) >> 1)

#define PUT_NODE(p, parent, left_child, right_child, payload, color, vacancy) \
    (PUT_PAYLOAD_SIZE(p, ALIGN(payload + WSIZE)),                             \
     PUT_LEFT_CHILD(p, left_child), PUT_PARENT(p, parent),                    \
     SET_COLOR(p, color), SET_FOOTER(p, GET_PAYLOAD_SIZE(p)),                 \
     SET_VACANCY(p, vacancy))

#define SET_FOOTER(p, payload_size) \
    *(size_t *)((char *)(p) + payload_size - WSIZE) = payload_size
#define GET_FOOTER(p) GET(((char *)(p) + GET_PAYLOAD_SIZE(p) - WSIZE))
#define GET_PBLOCK(p) (char *)(p)-4 * WSIZE - GET((char *)(p)-5 * WSIZE)
#define GET_NBLOCK(p) (void *)((char *)(p) + GET_PAYLOAD_SIZE(p) + 4 * WSIZE)

int main() {
    unsigned int *p = malloc(1000 * sizeof(int));

    PUT_NODE(p + 4, NULL, NULL, NULL, 124 * sizeof(int), BLACK, OCCUPIED);
    printf("%d\n", GET_COLOR(p + 4));
    printf("%d\n", GET_VACANCY(p + 4));
    SET_VACANCY(p + 4, FREE);
    SET_COLOR(p + 4, RED);

    // printf("%d\n", GET_PAYLOAD_SIZE(p + 4));
    // printf("%d\n", GET_FOOTER(p + 4));
    // printf("%d\n", ALIGN(123 + 4));
    // unsigned int *next_ptr = GET_NBLOCK(p + 4);
    // PUT_NODE(next_ptr, p + 4, NULL, NULL, 312 * sizeof(int), BLACK, FREE);
    // printf("%p == %p\n", GET_PBLOCK(next_ptr), p + 4);
    // printf("%p == %p\n", GET_LEFT_CHILD(next_ptr), p + 4);
    // printf("%p == %p\n", GET_RIGHT_CHILD(next_ptr), p + 4);
    // printf("%p == %p\n", GET_NBLOCK(p + 4), next_ptr);
    // for (int i = 0; i < 500; i++) {
    // printf("%d ", p[i]);
    //}
    // printf("\n");

    printf("%d\n", GET_COLOR(p + 4));
    printf("%d\n", GET_VACANCY(p + 4));
}
