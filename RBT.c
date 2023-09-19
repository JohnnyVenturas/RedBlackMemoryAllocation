#include <stdlib.h>
#include <stdio.h>
#include "RBT.h"

// The "p" stands for block ptr and therefore the structure is header, block
// ptr, footer

// parent(4),left_child(4), right_child(4), payload_size(4)
// Block Ptr ---- p
#define GET_PAYLOAD_SIZE(p) (*(size_t *)((char *)(p)-WSIZE))
#define GET_RIGHT_CHILD(p) *(size_t **)(((char *)(p)-WSIZE * 2))
#define GET_LEFT_CHILD(p) *(size_t **)(((char *)(p)-WSIZE * 3))
#define GET_PARENT(p) *(size_t **)(((char *)(p)-WSIZE * 4))

#define PUT_PAYLOAD_SIZE(p, val) (*(size_t *)((char *)(p)-WSIZE)) = val

#define PUT_RIGHT_CHILD(p, val) \
    *(size_t **)(((char *)(p)-WSIZE * 2)) = (size_t *)val

#define PUT_LEFT_CHILD(p, val) \
    *(size_t **)(((char *)(p)-WSIZE * 3)) = (size_t *)val

#define PUT_PARENT(p, val) *(size_t **)(((char *)(p)-WSIZE * 4)) = val

#define PUT_NODE(p, left_child, right_child, parent, payload)       \
    (PUT_PAYLOAD_SIZE(p, payload), PUT_RIGHT_CHILD(p, right_child), \
     PUT_LEFT_CHILD(p, left_child), PUT_PARENT(p, parent))

#define GET_COLOR(p) (GET_PAYLOAD_SIZE(p) & 1)
#define SET_COLOR(p, color) \
    (PUT_PAYLOAD_SIZE(p, (GET_PAYLOAD_SIZE(p) & (-2)) + color))
