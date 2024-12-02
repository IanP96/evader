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
