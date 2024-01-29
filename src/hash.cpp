#include "hash.h"

#include <ctime>
#include <random>

Zobrist::Zobrist() {
    for (int i = 0; i < 2; i++) {
        for (int x = 0; x < Board::BOARD_SIZE; x++) {
            for (int y = 0; y < Board::BOARD_SIZE; y++) {
                m_hashTable[i][x][y] = generateRandomNumber();
            }
        }
    }
    m_hashBoard = generateRandomNumber();
}

unsigned long long Zobrist::generateRandomNumber() {
    static std::mt19937 mt(123456);
    static std::uniform_int_distribution<unsigned long long> dist(0,
                                                                  UINT64_MAX);
    return dist(mt);
}

unsigned long long Zobrist::update(int x, int y, Board::CHESS_COLOR color) {
    m_hashBoard ^= m_hashTable[color][x][y];
    return m_hashBoard;
}
