#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "Board.hpp"
#include "jsoncpp/json.h"
#define UNUSED(x, y) (x) += (y)
//#include "Timer.hpp"
using namespace std;
struct Agent {
    // 搜索深度默认为 6
    const int searchDepth = 6;
    // 默认后手
    int color = WHITE;
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
    // 判断AI是否为先手
    void DetermineBlack(const Json::Value &, int);
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
        myBoard.PlaceAt(x, y, BLACK);
        if (myBoard.CheckFive(BLACK)) {
            myBoard.Show();
            cout << "you win" << endl;
            break;
        }
        cout << "Opponent: ";
        // auto [aix, aiy, ok] = myBoard.RandomPlace(color);
        auto [aix, aiy, ok] = myBoard.GreedyPlace(color);
        UNUSED(aix, aiy);
        myBoard.Show();
        if (!ok) {
            break;
        }
        if (myBoard.CheckFive(WHITE)) {
            cout << "you lose" << endl;
            break;
        }
    }
#else
    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    for (int i = 0; i < turnID; i++) {
        myBoard.PlaceAt(input["requests"][i]["x"].asInt(),
                        input["requests"][i]["y"].asInt(), BLACK);
        myBoard.PlaceAt(input["responses"][i]["x"].asInt(),
                        input["responses"][i]["y"].asInt(), WHITE);
    }
    myBoard.PlaceAt(input["requests"][turnID]["x"].asInt(),
                    input["requests"][turnID]["y"].asInt(), BLACK);
    DetermineBlack(input, turnID);
    auto [x, y, ok] = myBoard.GreedyPlace(color);
    if (ok) {
        Json::Value ret;
        ret["response"]["x"] = x;
        ret["response"]["y"] = y;
        if (color == BLACK) {
            ret["data"] = "black";
        } else {
            ret["data"] = "white";
        }
        Json::FastWriter writer;
        cout << writer.write(ret) << endl;
    }
#endif
}

void Agent::Init() {
    srand(time(0));
    // 根据json输入恢复棋盘状态
    // 最后一对坐标为 (-1,-1) 则 color 设为 1，我方先手
    // 否则，更新棋盘状态，我方后手
    // 本地测试不需要恢复，直接跳过
}

void Agent::DetermineBlack(const Json::Value &input, int turnID) {
    if ((turnID != 0 && input["data"].asString() == "black") ||
        (input["requests"][turnID]["x"].asInt() == -1 &&
         input["requests"][turnID]["y"].asInt() == -1)) {
        color = BLACK;
    }
}

#endif
