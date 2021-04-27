#ifndef POSITION_H
#include "Const.h"

struct Position {
    int x, y, w;
};

bool operator<(const Position& lhs, const Position& rhs) {
    int lhsx = abs(SIZE - lhs.x), rhsx = abs(SIZE - rhs.x);
    int lhsy = abs(SIZE - lhs.y), rhsy = abs(SIZE - rhs.y);
    return (lhs.w == rhs.w)? ((lhsx == rhsx) ? (lhsy < rhsy) : (lhsx < rhsx)): (lhs.w < rhs.w);
}
#endif