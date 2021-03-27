#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "Board.hpp"
#include "Const.h"
//#include "Timer.hpp"
using namespace std;

struct Agent {
    // 搜索深度默认为 6
    const int searchDepth = 6;
    // 默认后手
    const bool color = 0;
    // 计时器
    // Timer* myTimer = nullptr;
    // 棋盘
    Board myBoard;
    // 最优落子
    int bestDropId;

    // 运行
    void Run();
    // 初始化局面
    void Init();
    // 本地测试需要
    bool CheckFive(int);
    // MinMaxSearch()
    // SimpleEvaluate() 用于启发式搜索计算得分
};

void Agent::Run() {
    Init();

#ifndef ONLINE_JUDGE
    myBoard.Show();
    while (true) {
        int x, y;
        cout << "Your drop position: ";
        cin >> x >> y;
        myBoard.PlaceAt(x, y, 1);
        if (CheckFive(1)) {
            myBoard.Show();
            cout << "you win" << endl;
            break;
        }
        cout << "Opponent: ";
        if (!myBoard.RandomPlace(0)) {
            myBoard.Show();
            break;
        }
        if (CheckFive(0)) {
            myBoard.Show();
            cout << "you lose" << endl;
            break;
        }
        myBoard.Show();
    }
#endif
}

void Agent::Init() {
    srand(time(0));
    // 根据jason输入恢复棋盘状态
    // 最后一对坐标为 (-1,-1) 则 color 设为 1，我方先手
    // 否则，更新棋盘状态，我方后手
    // 本地测试不需要恢复，直接跳过
}

bool Agent::CheckFive(int color) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < 4; k++) {
                if (myBoard.boardState[i][j] != color) continue;
                int ti = i, tj = j;
                for (int s = 1; s <= 4; s++) {
                    ti += dr[k];
                    tj += dc[k];
                    if (ti < 0 || ti >= SIZE || tj < 0 || tj >= SIZE) continue;
                    if (myBoard.boardState[ti][tj] != color) break;
                    if (s == 4) return true;
                }
            }
        }
    return false;
}

#endif
