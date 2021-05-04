#ifndef POSITION_H
#include "Const.h"

struct Position {
    int x, y;
    LL w;
};

bool operator<(const Position& lhs, const Position& rhs) {
    LL lhsw = 14 - abs(SIZE / 2 - lhs.x) - abs(SIZE / 2 - lhs.y) + lhs.w;
    LL rhsw = 14 - abs(SIZE / 2 - rhs.x) - abs(SIZE / 2 - rhs.y) + rhs.w;
    return (lhsw == rhsw)? ((lhs.x == rhs.x) ? (lhs.y < rhs.y) : (lhs.x < rhs.x)): (lhsw > rhsw);
}
#endif