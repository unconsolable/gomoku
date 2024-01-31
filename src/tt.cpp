#include "tt.h"

TT::TT() {
    m_pTable[0] = new Item[LENGTH];
    m_pTable[1] = new Item[LENGTH];
}

TT::~TT() {
    delete[] m_pTable[0];
    delete[] m_pTable[1];
}

int TT::find(unsigned long long hash, int depth, int alpha, int beta,
             Board::CHESS_COLOR color) {
    int index = getHashIndex(hash);

    Item &item = m_pTable[color][index];

    if (item.flag == EMPTY) return TT_NOT_HIT;

    if (item.hash == hash && item.depth >= depth) {
        switch (item.flag) {
            case EXACT:
                return item.value;
            case LOWER:
                if (item.value >= beta) return item.value;
                break;
            case UPPER:
                if (item.value <= alpha) return item.value;
                break;
            default:
                break;
        }
    }

    return TT_NOT_HIT;
}

void TT::insert(unsigned long long hash, int depth, int value, Flag flag,
                Board::CHESS_COLOR color) {
    int index = getHashIndex(hash);

    Item &item = m_pTable[color][index];

    item.depth = depth;
    item.hash = hash;
    item.value = value;
    item.flag = flag;
}