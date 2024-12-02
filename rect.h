#ifndef RECT_H
#define RECT_H

#include <math.h>

typedef struct OrderedPair {
    float x, y;
} OrderedPair;

// A moving rectangle
typedef struct MovingRect {
    OrderedPair pos;
    OrderedPair dir;
    // Width, height
    float w, h;
} MovingRect;

// Return the rectangle but moved in the direction
MovingRect moved_rect(MovingRect oldRect, float delta);

// Move the rectangle in the direction
void move_rect(MovingRect* rect, float delta);

// Ordered pair relative to another (vector subtraction)
OrderedPair relative_pos(OrderedPair pos, OrderedPair relativeTo);

// Scale vector to given desired magnitude
OrderedPair scaled_vector(OrderedPair vec, float desiredMagnitude);

#endif // !RECT_H
