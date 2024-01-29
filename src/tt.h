#ifndef TT_H
#define TT_H

#include "board.h"

/** Transposition Table */
class TT {
    enum Flag { EMPTY, EXACT, LOWER, UPPER };

    struct Item {
        int depth;
        unsigned long long hash;
        int value;
        Flag flag = EMPTY;
    };

    TT();
    ~TT();

    int getHashIndex(unsigned long long hash) { return hash & 0xFFFFF; }

    int find(int depth, unsigned long long hash, int alpha, int beta,
             Board::CHESS_COLOR);
    void insert(int depth, unsigned long long hash, int value, Flag,
                Board::CHESS_COLOR);

    const static int LENGTH = 1 << 20;
    const static int TT_NOT_HIT = __INT32_MAX__ - 1;

    Item *m_pTable[2] = {nullptr, nullptr};
};

#endif