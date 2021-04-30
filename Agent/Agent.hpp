#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "Board.hpp"
#include "Position.hpp"
#include "Timer.hpp"
// #include "jsoncpp/json.h"
#define UNUSED(x, y) (x) += (y)

using namespace std;
struct Agent {
    // 默认后手
    int color = WHITE;
    //计时器
    Timer* myTimer = nullptr;
    // 棋盘
    Board myBoard;
    // 最优落子
    int bestDropId = -1;
    // 地方上一次落子位置
    int lastDropId = -1;
    // 最高得分
    int bestScore = -INF;
    // 落子位置和对应的权重, 用于在set中查找.
    // -1表示有棋子的点
    int weight[2][225];
    // 按照权排序的落子位置集合
    set<Position> nextPos[2];

    // 运行
    void Run();
    // 初始化局面
    void Init();
    // 判断AI是否为先手
    // void DetermineBlack(const Json::Value &, int);
    // 局面估值
    int Evaluate(int);
    // 更改一个点的颜色, 同时更新附近的9*9格子内的空闲点权值信息
    // 后续优化可以尝试减少更改的范围
    void Update(int x, int y, int color);
    // 局面预处理, 初始化nextPos和weight数组
    void Preplay();
    // 搜素
    int MinMaxSearch(int, int, int, bool);
};

int Agent::MinMaxSearch(int depth, int alpha, int beta, bool curColor) {
    // 对手五连应该直接返回 -INF
    if (lastDropId != -1 &&
        myBoard.CheckFive(lastDropId / 15, lastDropId % 15, curColor ^ 1))
        return -INF;
    // 层数用完，估值返回
    if (depth <= 0) return Evaluate(curColor);
    // v 存储所有可落子位置
    int v[SIZE * SIZE];
    // 落子位置总数
    int cnt = -1;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (myBoard.boardState[r][c] == UNPLACE) v[++cnt] = r * 15 + c;
    // 无子可走
    if (cnt == -1) return -INF;
    // 按优先级排序
    auto cmp = [&](int& a, int& b) -> bool {
        // 排序方式欠考虑
        return weight[curColor][a] > weight[curColor][b];
    };
    sort(v, v + cnt + 1, cmp);
    // 对所有可能局面进行搜索
    for (size_t i = 0; i <= min(32, cnt); i++) {
        // 落子 更新得分
        Update(v[i] / 15, v[i] % 15, curColor);
        // 更改上一次落子位置
        int tmpId = lastDropId;
        lastDropId = v[i];
        // 继续搜索
        int val = -MinMaxSearch(depth - 1, -beta, -alpha, curColor);
        // 取消落子 更新得分
        Update(v[i] / 15, v[i] % 15, UNPLACE);
        // 恢复上一次落子位置
        lastDropId = tmpId;
        if (val >= beta) {
            if (depth == SEARCH_DEPTH) bestScore = val, bestDropId = v[i];
            return val;
        }
        if (val > alpha) alpha = val;  // bestDropId
        if (depth == SEARCH_DEPTH &&
            (val > bestScore || bestDropId == BEST_UNDEFINED))
            bestScore = val, bestDropId = v[i];
    }
    return alpha;
}

void Agent::Run() {
    Init();

#ifndef ONLINE_JUDGE
    myBoard.Show();
    while (true) {
        Preplay();
        int x, y;
        cout << "Your drop position: ";
        cin >> x >> y;
        myBoard.PlaceAt(x, y, BLACK);
        if (myBoard.CheckFive(BLACK)) {
            myBoard.Show();
            cout << "you win" << endl;
            break;
        }

        myTimer = new Timer;
        myTimer->prepare(__LINE__);
        // auto [aix, aiy, ok] = myBoard.RandomPlace(color);
        // auto [aix, aiy, ok] = myBoard.GreedyPlace(color);
        MinMaxSearch(SEARCH_DEPTH, -INF, INF, WHITE);
        myTimer->getTimePass(__LINE__);
        cout << "Opponent: ";
        myBoard.PlaceAt(bestDropId / 15, bestDropId % 15, WHITE);
        cout << "Score: " << bestScore << endl;
        // UNUSED(aix, aiy);
        myBoard.Show();
        // if (!ok) {
        //    break;
        //}
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
    DetermineBlack(input, turnID);
    for (int i = 0; i < turnID; i++) {
        myBoard.PlaceAt(input["requests"][i]["x"].asInt(),
                        input["requests"][i]["y"].asInt(), !color);
        myBoard.PlaceAt(input["responses"][i]["x"].asInt(),
                        input["responses"][i]["y"].asInt(), color);
    }
    myBoard.PlaceAt(input["requests"][turnID]["x"].asInt(),
                    input["requests"][turnID]["y"].asInt(), !color);
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

/*
void Agent::DetermineBlack(const Json::Value &input, int turnID) {
    if ((turnID != 0 && input["data"].asString() == "black") ||
        (input["requests"][turnID]["x"].asInt() == -1 &&
         input["requests"][turnID]["y"].asInt() == -1)) {
        color = BLACK;
    }
}
*/

void Agent::Preplay() {
    // 初始化weight和nextPos
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // 如果位置上已有棋子, 标记权重为-1
            // 否则计算权重, 并分别存放在weight和nextPos中
            if (myBoard.boardState[i][j] != UNPLACE) {
                weight[BLACK][i * 15 + j] = weight[WHITE][i * 15 + j] = -1;
            } else {
                weight[BLACK][i * 15 + j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i * 15 + j] = myBoard.MarkOfPoint(i, j, WHITE);
                nextPos[BLACK].insert(
                    Position{i, j, weight[BLACK][i * 15 + j]});
                nextPos[WHITE].insert(
                    Position{i, j, weight[WHITE][i * 15 + j]});
            }
        }
    }
}

void Agent::Update(int x, int y, int color) {
    // 检查是否均为或均不为未放置
    assert(!(weight[BLACK][x * 15 + y] == -1) ^
           (weight[WHITE][x * 15 + y] == -1));
    // 原始为未放置=>清除点在weight和nextPos中的记录
    if (weight[BLACK][x * 15 + y] != -1 && weight[WHITE][x * 15 + y] != -1) {
        nextPos[BLACK].erase(Position{x, y, weight[BLACK][x * 15 + y]});
        nextPos[WHITE].erase(Position{x, y, weight[WHITE][x * 15 + y]});
        weight[BLACK][x * 15 + y] = weight[WHITE][x * 15 + y] = -1;
    }
    myBoard.boardState[x][y] = color;
    // 更改为未放置=>增加点在weight和nextPos中的记录
    if (color == UNPLACE) {
        weight[BLACK][x * 15 + y] = myBoard.MarkOfPoint(x, y, BLACK);
        weight[WHITE][x * 15 + y] = myBoard.MarkOfPoint(x, y, WHITE);
        nextPos[BLACK].insert(Position{x, y, weight[BLACK][x * 15 + y]});
        nextPos[WHITE].insert(Position{x, y, weight[WHITE][x * 15 + y]});
    }
    // 修改完成后, 在9*9范围内修改空闲点的权值
    for (int i = max(0, x - 4); i < min(15, x + 4); i++) {
        for (int j = max(0, y - 4); j < min(15, y + 4); j++) {
            if (i != x && j != y && myBoard.boardState[i][j] == UNPLACE) {
                // 删除现存权值记录
                nextPos[BLACK].erase(Position{x, y, weight[BLACK][x * 15 + y]});
                nextPos[WHITE].erase(Position{x, y, weight[WHITE][x * 15 + y]});
                // 求出新权值记录并保存
                weight[BLACK][x * 15 + y] = myBoard.MarkOfPoint(x, y, BLACK);
                weight[WHITE][x * 15 + y] = myBoard.MarkOfPoint(x, y, WHITE);
                nextPos[BLACK].insert(
                    Position{x, y, weight[BLACK][x * 15 + y]});
                nextPos[WHITE].insert(
                    Position{x, y, weight[WHITE][x * 15 + y]});
            }
        }
    }
}

int Agent::Evaluate(int color) {
    // 下面两种方法效果都不好

    //int tot = 0;
    //for (auto& pos : nextPos[color]) tot += pos.w;
    return nextPos[color].begin()->w;
    // return tot;
}

#endif
