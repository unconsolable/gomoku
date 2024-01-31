#ifndef HASH_H
#define HASH_H

#include "board.h"

class Zobrist {
   public:
    Zobrist();

    static unsigned long long generateRandomNumber();

    unsigned long long update(int x, int y, Board::CHESS_COLOR);

    unsigned long long m_hashTable[2][Board::BOARD_SIZE][Board::BOARD_SIZE];
    unsigned long long m_boardHash;
};

#endif