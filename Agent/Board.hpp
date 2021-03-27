#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>

#include "Const.h"

using namespace std;

struct Board {
    // 每个位置得分，用于MinMaxSearch对选择位置进行排序，得分高的位置先选
    int posValue[SIZE][SIZE];
    // 每个位置落子类型：-1 无子，0 白，1 黑
    int boardState[SIZE][SIZE];

    // 构造函数
    Board();
    // 输出棋盘
    void Show();
    // 落子
    void PlaceAt(int, int, bool);
    // 随机落子
    bool RandomPlace(bool);
    // 取消落子
    void UnPlaceAt(int, int);
    // 更新每个位置得分
    void CalcValue();
};

Board::Board() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            posValue[i][j] = 0;
            boardState[i][j] = -1;
        }
}

void Board::Show() {
    cout << "  ";
    for (int i = 0; i < SIZE; i++)
        cout << char(i < 10 ? (int)i + '0' : 'A' + (int)i - 10) << " ";
    cout << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << char(i < 10 ? (int)i + '0' : 'A' + (int)i - 10) << "|";
        for (int j = 0; j < SIZE; j++) {
            if (boardState[i][j] == -1)
                cout << "* ";
            else
                cout << (int)boardState[i][j] << " ";
        }
        cout << endl;
    }
}

void Board::PlaceAt(int x, int y, bool color) {
    boardState[(int)x][(int)y] = color;

#ifndef ONLINE_JUDGE
    cout << "Chess Placed at: "
         << "(" << (int)x << ", " << (int)y << ")" << endl;
#endif
}

void Board::UnPlaceAt(int x, int y) { boardState[x][y] = -1; }

bool Board::RandomPlace(bool color) {
    vector<int> v;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            if (boardState[i][j] == -1) v.push_back(i * 15 + j);
    }
    if (!v.size()) return false;
    int id = v[rand() % v.size()];
    PlaceAt(id / 15, id % 15, color);
    return true;
}
#endif