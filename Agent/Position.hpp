#ifndef POSITION_H
#include "Const.h"

struct Position {
    int x, y;
    LL w;
};

bool operator<(const Position &lhs, const Position &rhs) {
    LL lhsw = BASE_MARK[lhs.x][lhs.y] + lhs.w;
    LL rhsw = BASE_MARK[rhs.x][rhs.y] + rhs.w;
    return (lhsw == rhsw)
               ? ((lhs.x == rhs.x) ? (lhs.y < rhs.y) : (lhs.x < rhs.x))
               : (lhsw > rhsw);
}
#endif