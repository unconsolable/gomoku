#ifndef POSITION_H
#include "Const.h"

struct Position {
    int x, y, w;
};

bool operator<(const Position& lhs, const Position& rhs) {
    int lhsx = abs(SIZE / 2 - lhs.x), rhsx = abs(SIZE / 2 - rhs.x);
    int lhsy = abs(SIZE / 2 - lhs.y), rhsy = abs(SIZE / 2 - rhs.y);
    return (lhs.w == rhs.w)? ((lhsx == rhsx) ? (lhsy < rhsy) : (lhsx < rhsx)): (lhs.w > rhs.w);
}
#endif