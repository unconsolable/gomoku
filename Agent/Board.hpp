#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>

#include "Const.h"

using namespace std;

struct Board {
    /* 
     * 两个选手未落子位置得分，用于MinMaxSearch对选择位置进行排序，得分高的位置先选
     * 可能不需要两个，之后根据需求调整
     */
    int posValue[2][SIZE][SIZE];
    // 每个位置落子类型：-1 无子，0 白，1 黑
    int boardState[SIZE][SIZE];

    // 构造函数
    Board();
    // 输出棋盘
    void Show();
    // 落子
    void PlaceAt(int, int, int);
    // 随机落子
    tuple<int, int, bool> RandomPlace(int);
    // 贪心落子
    tuple<int, int, bool> GreedyPlace(int);
    // 取消落子
    void UnPlaceAt(int, int);
    // 检查是否为五子
    bool CheckFive(int color);
    // 更新每个位置得分
    void CalcValue();
    // 和当前点相对位置的格子值
    int RelativePosVal(int, int, int, int);
    // 判断当前点的得分情况, 详见函数处注释
    int MarkOfPoint(int, int, int, int);
};

Board::Board() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            posValue[0][i][j] = posValue[1][i][j] = 0;
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

void Board::PlaceAt(int x, int y, int color) {
    if ((x >= 0 && x < SIZE) && (y >= 0 && y < SIZE))
        boardState[(int)x][(int)y] = color;

#ifndef ONLINE_JUDGE
    cout << "Chess Placed at: "
         << "(" << (int)x << ", " << (int)y << ")" << endl;
#endif
}

void Board::UnPlaceAt(int x, int y) { boardState[x][y] = -1; }

tuple<int, int, bool> Board::RandomPlace(int color) {
    vector<int> v;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            if (boardState[i][j] == -1)
                v.push_back(i * 15 + j);
    }
    if (!v.size())
        return {-1, -1, false};
    int id = v[rand() % v.size()];
    PlaceAt(id / 15, id % 15, color);
    return {id / 15, id % 15, true};
}

tuple<int, int, bool> Board::GreedyPlace(int color) {
    vector<int> v;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            if (boardState[i][j] == UNPLACE)
                v.push_back(i * 15 + j);
    }
    if (!v.size())
        return {-1, -1, false};
    // 鼓励在中心位置放子
    auto CentralMark = [](int x, int y) { return 14 - abs(x - 7) - abs(y - 7); };
    // 考虑放子对自己和对手的影响
    int maxMark = MarkOfPoint(v[0] / 15, v[0] % 15, color, color) +
                  CentralMark(v[0] / 15, v[0] % 15),
        maxPoint = v[0];
    for (size_t id = 1; id < v.size(); id++) {
        auto curMark = MarkOfPoint(v[id] / 15, v[id] % 15, color, color) +
                       CentralMark(v[id] / 15, v[id] % 15);
        // cout << "curMark: " << curMark << "x: " << v[id] / 15 << "y: " << v[id] % 15 << "\n";
        if (curMark > maxMark) {
            maxMark = curMark;
            maxPoint = v[id];
        }
    }
    PlaceAt(maxPoint / 15, maxPoint % 15, color);
    return {maxPoint / 15, maxPoint % 15, true};
}

bool Board::CheckFive(int color) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (boardState[i][j] != color)
                continue;
            // 遍历每个位置
            for (int k = 0; k < 4; k++) {
                int ti = i, tj = j;
                // 遍历每个棋子
                for (int s = 1; s <= 4; s++) {
                    ti += dr[k];
                    tj += dc[k];
                    if (ti < 0 || ti >= SIZE || tj < 0 || tj >= SIZE)
                        continue;
                    if (boardState[ti][tj] != color)
                        break;
                    if (s == 4)
                        return true;
                }
            }
        }
    return false;
}

int Board::RelativePosVal(int curX, int curY, int direction, int offset) {
    int unitOffsetX[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int unitOffsetY[] = {1, 1, 0, -1, -1, -1, 0, 1};
    curX = curX + unitOffsetX[direction] * offset;
    curY = curY + unitOffsetY[direction] * offset;
    if (curX < 0 || curX > SIZE || curY < 0 || curY > SIZE) {
        return INVALID;
    }
    return boardState[curX][curY];
}

int Board::MarkOfPoint(int curX, int curY, int playerColor, int aiColor) {
    /*
     * 8个方向枚举
     * 以下注释以#表示(curX, curY), 1表示playerColor, 2表示otherColor
     * 0表示无子, 3表示非法下标, 未注明的表示任意
     * 活的情况必须**中间同色棋子连续**, 中间有间断一定为眠
     * playerColor与aiColor相同分数为正, 类型相反分数为负
     */
    int total = 0;
    for (int i = 0; i < 8; i++) {
        // 活四LIVEFOUR, #11110
        if (RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor &&
            RelativePosVal(curX, curY, i, 5) == UNPLACE) {
            // cout << "LIVEFOURMARK ";
            total += (playerColor == aiColor) ? LIVEFOURMARK : -LIVEFOURMARK;
        }
        // 眠四SLEEPFOUR, #11112(3), 1#111, 11#11
        if (RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor &&
            (RelativePosVal(curX, curY, i, 5) == !playerColor ||
             RelativePosVal(curX, curY, i, 5) == INVALID)) {
            // cout << "SLEEPFOURMARK ";
            total += (playerColor == aiColor) ? SLEEPFOURMARK : -SLEEPFOURMARK;
        }
        if (RelativePosVal(curX, curY, i, -1) == playerColor &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += (playerColor == aiColor) ? SLEEPFOURMARK : -SLEEPFOURMARK;
        }
        if (RelativePosVal(curX, curY, i, -2) == playerColor &&
            RelativePosVal(curX, curY, i, -1) == playerColor &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += (playerColor == aiColor) ? SLEEPFOURMARK : -SLEEPFOURMARK;
        }
        // 活三LIVETHREE, #0111, 0#111
        if (RelativePosVal(curX, curY, i, 1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "LIVETHREEMARK ";
            total += (playerColor == aiColor) ? LIVETHREEMARK : -LIVETHREEMARK;
        }
        if (RelativePosVal(curX, curY, i, -1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "LIVETHREEMARK ";
            total += (playerColor == aiColor) ? LIVETHREEMARK : -LIVETHREEMARK;
        }
        // 眠三SLEEPTHREE, 1#11
        if (RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == UNPLACE &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "SLEEPTHREEMARK ";
            total += (playerColor == aiColor) ? SLEEPTHREEMARK : -SLEEPTHREEMARK;
        }
        // 活二 #0011
        if (RelativePosVal(curX, curY, i, 1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 2) == UNPLACE &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += (playerColor == aiColor) ? LIVETWOMARK : -LIVETWOMARK;
        }
        // 眠二 1#1
        if (RelativePosVal(curX, curY, i, -1) == playerColor &&
            RelativePosVal(curX, curY, i, 1) == playerColor) {
            // cout << "SLEEPTWOMARK ";
            total += (playerColor == aiColor) ? SLEEPTWOMARK : -SLEEPTWOMARK;
        }
        // 剩余情况, 以加和为1代替
        if ((RelativePosVal(curX, curY, i, 1) == playerColor) +
                (RelativePosVal(curX, curY, i, 2) == playerColor) +
                (RelativePosVal(curX, curY, i, 3) == playerColor) +
                (RelativePosVal(curX, curY, i, 4) == playerColor) ==
            1) {
            // cout << "ONEMARK ";
            total += (playerColor == aiColor) ? ONEMARK : -ONEMARK;
        }
    }
    return total;
}

#endif