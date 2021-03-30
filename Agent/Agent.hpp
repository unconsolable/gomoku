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
    const int SEARCH_DEPTH = 6;
    // 默认后手
    int myColor = WHITE;
    // 计时器
    // Timer* myTimer = nullptr;
    // 棋盘
    Board myBoard;
    // 最优落子
    int bestDropId;
    // 搜索时上一步落子
    int lastDropId;
    // 最高得分
    int bestScore;

    // 运行
    void Run();
    // 从jason读入并初始化棋盘
    void Init();
    // 按 json 格式输出
    void Output();
    // 判断AI是否为先手
    void DetermineBlack(const Json::Value&, int);
    // 搜索
    int MinMaxSearch(int, int, int);
    // 局面估值
    int Evaluate(int);
    // 对某个位置八个方向检查连五
    bool CheckFive(int, int);
};

bool Agent::CheckFive(int color, int posId) {
    int r = posId / 15, c = posId % 15;
    // 遍历 4 个方向
    for (int k = 0; k < 4; k++) {
        // 遍历每个棋子
        int tr, tc, cnt(1);
        for (int s = 1; s <= 4; s++) {
            tr = r + dr[k] * s;
            tc = c + dc[k] * s;
            if (tr < 0 || tr >= SIZE || tc < 0 || tc >= SIZE) break;
            if (myBoard.boardState[tr][tc] != color) break;
            cnt++;
        }
        for (int s = 1; s <= 4; s++) {
            tr = r - dr[k] * s;
            tc = c - dc[k] * s;
            if (tr < 0 || tr >= SIZE || tc < 0 || tc >= SIZE) break;
            if (myBoard.boardState[tr][tc] != color) break;
            cnt++;
        }
        if (cnt >= 5) return true;
    }
    return false;
}

int Agent::MinMaxSearch(int depth, int alpha, int beta) {
    bool curColor = (depth & 1) ? myColor : myColor ^ 1;
    if (CheckFive(curColor, lastDropId)) return INF;
    if (depth <= 0) return Evaluate(curColor);
    // 所有可能位置
    vector<int> v;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (myBoard.boardState[r][c] == UNPLACE) v.push_back(r * 15 + c);
    if (!v.size()) return -INF;  //无子可走
    // 按优先级排序
    auto cmp = [&](int& a, int& b) -> bool {
        return myBoard.posValue[curColor][a] > myBoard.posValue[curColor][b];
    };
    sort(v.begin(), v.end(), cmp);
    // 对所有可能局面进行搜索
    for (int i = 0; i < v.size(); i++) {
        // 记录落子位置
        myBoard.PlaceAt(v[i] / 15, v[i] % 15, curColor);
        int tmpId = lastDropId;
        lastDropId = v[i];
        // 继续搜索
        int val = -MinMaxSearch(depth - 1, -beta, -alpha);
        // 取消落子
        myBoard.UnPlaceAt(v[i] / 15, v[i] % 15);
        lastDropId = tmpId;
        if (val >= beta) {
            if (depth == SEARCH_DEPTH) bestScore = val, bestDropId = v[i];
            return val;
        }
        if (val > alpha) alpha = val;  // bestDropId
        if (depth == SEARCH_DEPTH && (val > bestScore || bestDropId == -1))
            bestScore = val, bestDropId = v[i];
    }
    return alpha;
}

void Agent::Run() {
    Init();

#ifndef ONLINE_JUDGE
    myBoard.Show();
    while (true) {
        int x, y;
        cout << "Your drop position: ";
        cin >> x >> y;
        myBoard.PlaceAt(x, y, BLACK);
        if (CheckFive(BLACK, x * 15 + y)) {
            myBoard.Show();
            cout << "you win" << endl;
            break;
        }
        cout << "Opponent: ";
        // 估值函数写好再用！
        // MinMaxSearch(SEARCH_DEPTH, -INF, INF);
        // myBoard.PlaceAt(bestDropId / 15, bestDropId % 15, myColor);
        myBoard.Show();
        if (!bestDropId) break;
        if (CheckFive(WHITE, bestDropId)) {
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
    // 根据json输入恢复棋盘状态
    // 最后一对坐标为 (-1,-1) 则 color 设为 1，我方先手
    // 否则，更新棋盘状态，我方后手
    // 本地测试不需要恢复，直接跳过
}

void Agent::DetermineBlack(const Json::Value& input, int turnID) {
    if ((turnID != 0 && input["data"].asString() == "black") ||
        (input["requests"][turnID]["x"].asInt() == -1 &&
         input["requests"][turnID]["y"].asInt() == -1)) {
        myColor = BLACK;
    }
}

#endif
