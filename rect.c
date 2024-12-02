#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "rect.h"

MovingRect moved_rect(MovingRect oldRect, float delta) {
    MovingRect newRect;
    newRect.dir = oldRect.dir;
    newRect.pos.x = oldRect.pos.x + oldRect.dir.x * delta;
    newRect.pos.y = oldRect.pos.y + oldRect.dir.y * delta;
    newRect.w = oldRect.w;
    newRect.h = oldRect.h;
    return newRect;
}

void move_rect(MovingRect* rect, float delta) {
    *rect = moved_rect(*rect, delta);
}

OrderedPair relative_pos(OrderedPair pos, OrderedPair relativeTo) {
    pos.x -= relativeTo.x;
    pos.y -= relativeTo.y;
    return pos;
}

OrderedPair scaled_vector(OrderedPair vec, float desiredMagnitude) {
    float curMagnitude = sqrt(pow(vec.x, 2) + pow(vec.y, 2));
    float ratio = desiredMagnitude / curMagnitude;
    vec.x *= ratio;
    vec.y *= ratio;
    return vec;
}

float get_edge(MovingRect rect, Edge edge) {
    switch (edge)
    {
    case EDGE_TOP:
        return rect.pos.y;
    case EDGE_BOTTOM:
        return rect.pos.y + rect.h;
    case EDGE_LEFT:
        return rect.pos.x;
    case EDGE_RIGHT:
        return rect.pos.x + rect.w;
    case EDGE_NONE:
        // Shouldn't get here
        exit(EXIT_FAILURE);
    }
}

float get_dir_component(OrderedPair dir, Edge edge) {
    if (edge % 2 == 0) return dir.y;
    return dir.x;
}

int8_t sign(float num) {
    if (num > 0) return 1;
    if (num < 0) return -1;
    return 0;
}

float abs_float(float num) {
    if (num < 0) return -num;
    return num;
}

// True if num between a and b
bool between(float num, float a, float b) {
    return (a < num && num < b) || (b < num && num < a);
}

Edge would_collide(MovingRect rect1, MovingRect rect2, float delta) {
    MovingRect rect1new = moved_rect(rect1, delta);
    MovingRect rect2new = moved_rect(rect2, delta);
    for (Edge edge = 0; edge < 4; edge++)
    {
        Edge oppositeEdge = (edge + 2) % 4;
        float oldEdge1 = get_edge(rect1, edge);
        float newEdge1 = get_edge(rect1new, edge);
        float oldEdge2 = get_edge(rect2, oppositeEdge);
        float newEdge2 = get_edge(rect2new, oppositeEdge);
        bool edgesOverlap = (
            (oldEdge1 > oldEdge2) == (edge < 2) 
            && (newEdge1 > newEdge2) != (edge < 2)
        );
        if (!edgesOverlap)
        {
            continue;
        }

        float delta = abs_float(oldEdge1 - oldEdge2) / abs_float(get_dir_component(rect1.dir, edge) - get_dir_component(rect2.dir, edge));
        MovingRect rect1collision = moved_rect(rect1, delta);
        MovingRect rect2collision = moved_rect(rect2, delta);
        Edge adjEdgeA = (edge + 1) % 4;
        Edge adjEdgeB = (adjEdgeA + 2) % 4;
        float boundA = get_edge(rect2collision, adjEdgeA);
        float boundB = get_edge(rect2collision, adjEdgeB);
        float colEdgeA = get_edge(rect1collision, adjEdgeA);
        float colEdgeB = get_edge(rect1collision, adjEdgeB);
        if (between(colEdgeA, boundA, boundB) || between(colEdgeB, boundA, boundB)) {
            return edge;
        }
    }
    return EDGE_NONE;
}
