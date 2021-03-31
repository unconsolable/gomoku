#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "Board.hpp"
#include "Position.hpp"
#include "Timer.hpp"
#include "jsoncpp/json.h"
#define UNUSED(x, y) (x) += (y)

using namespace std;
#ifdef DEBUG
extern int markcallcnt;
#endif
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
    LL bestScore = -INF;
    // 落子位置和对应的权重, 用于在set中查找.
    // -1表示有棋子的点
    int weight[2][225];
    // 按照权排序的落子位置集合
    set<Position> nextPos[3];

    // 运行
    void Run();
    // 从jason读入并初始化棋盘
    void Init();
    // 按 json 格式输出
    void Output();
    // 判断AI是否为先手
    void DetermineBlack(const Json::Value &);
    // 局面估值
    LL Evaluate(int);
    // 更改一个点的颜色, 同时更新附近的9*9格子内的空闲点权值信息
    // 后续优化可以尝试减少更改的范围
    void Update(int x, int y, int color);
    // 局面预处理, 初始化nextPos和weight数组
    void Preplay();
    // 搜素
    LL MinMaxSearch(int, LL, LL, bool);
};

LL Agent::MinMaxSearch(int depth, LL alpha, LL beta, bool curColor) {
    // 对手五连应该直接返回 -INF
    if (lastDropId != -1 &&
        myBoard.CheckFive(lastDropId / 15, lastDropId % 15, curColor ^ 1))
        return -INF;
    // 层数用完，估值返回
    if (depth <= 0) return Evaluate(curColor);
    // 无子可走
    if(!nextPos[MAX].size()) return -INF;
    // 对所有可能局面进行搜索
    set<Position> tmp;
    auto pos = nextPos[MAX].begin();
    // 部分拷贝
    for(int i = 0; i < SEARCHCNT[depth] && pos != nextPos[MAX].end(); i++) {
        tmp.insert(*pos);
        pos++;
    }
    // 全拷贝
    /*for(int i = 0; i < nextPos[MAX].size(); i++) {
        tmp.insert(*pos);
        pos++;
    }*/
    for(auto& pos: tmp) {
        int x = pos.x, y = pos.y;
        Update(x, y, curColor);
        // 更改上一次落子位置
        int tmpId = lastDropId;
        lastDropId = x * 15 + y;
        // 继续搜索
        LL val = -MinMaxSearch(depth - 1, -beta, -alpha, !curColor);
        // 取消落子 更新得分
        Update(x, y, UNPLACE);
        // 恢复上一次落子位置
        lastDropId = tmpId;
        if (val >= beta) {
            if (depth == SEARCH_DEPTH) bestScore = val, bestDropId = x * 15 + y;
            return val;
        }
        if (val > alpha) alpha = val;  // bestDropId
        if (depth == SEARCH_DEPTH &&
            (val > bestScore || bestDropId == BEST_UNDEFINED))
            bestScore = val, bestDropId = x * 15 + y;

    }
    return alpha;
}

void Agent::Run() {
    Init();

#ifndef ONLINE_JUDGE
    myBoard.Show();
    while (true) {
        Preplay();
        lastDropId = bestDropId = -1;
        bestScore = -INF;
        int x, y;
        cout << "Your drop position: ";
        cin >> x >> y;
        Update(x, y, BLACK);
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
        cout << "Opponent: " << bestDropId / 15 << " " << bestDropId % 15 << endl;
        // 落子
        Update(bestDropId / 15, bestDropId % 15, WHITE);
        debug(nextPos[WHITE].size());
        debug(nextPos[BLACK].size());
        debug(nextPos[MAX].size());
        debug(bestScore);
        myBoard.Show();
        // if (!ok) {
        //    break;
        //}
        if (myBoard.CheckFive(WHITE)) {
            cout << "you lose" << endl;
            break;
        }
    }
#endif
    Output();
}

void Agent::Init() {
    // 根据json输入恢复棋盘状态
    // 最后一对坐标为 (-1,-1) 则 color 设为 1，我方先手
    // 否则，更新棋盘状态，我方后手
    // 本地测试不需要恢复，直接跳过
    // 读入JSON
#ifdef ONLINE_JUDGE
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    DetermineBlack(input);
    Preplay();
    for (int i = 0; i < turnID; i++) {
        Update(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), !color);
        Update(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), color);
    }
    Update(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), !color);
    MinMaxSearch(SEARCH_DEPTH, -INF, INF, color);
    Json::Value ret;
    ret["response"]["x"] = bestDropId / 15;
    ret["response"]["y"] = bestDropId % 15;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
#endif
}

void Agent::Output() {
#ifdef ONLINE_JUDGE
    Json::Value ret;
    ret["response"]["x"] = bestDropId / 15;
    ret["response"]["y"] = bestDropId % 15;
    if (myColor == BLACK) {
        ret["data"] = "black";
    } else {
        ret["data"] = "white";
    }
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
#endif
}

void Agent::DetermineBlack(const Json::Value &input) {
    if (input["requests"][0]["x"].asInt() == -1 && input["requests"][0]["y"].asInt() == -1) {
        color = BLACK;
    } else {
        color = WHITE;
    }
}

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
                nextPos[MAX].insert(
                    Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].insert(
                    Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].insert(
                    Position{i, j, weight[BLACK][i * 15 + j]});
            }
        }
    }
}

void Agent::Update(int x, int y, int color) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return;
    }
    // 检查是否均为或均不为未放置
    assert(!(weight[BLACK][x * 15 + y] == -1) ^ (weight[WHITE][x * 15 + y] == -1));
    // 原始为未放置=>清除点在weight和nextPos中的记录
    if (weight[BLACK][x * 15 + y] != -1 && weight[WHITE][x * 15 + y] != -1) {
#ifndef ONLINE_JUDGE
        assert(nextPos[MAX].count(
                   {x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])}) == 1);
        assert(nextPos[WHITE].count(Position{x, y, weight[WHITE][x * 15 + y]}) == 1);
        assert(nextPos[MAX].count(Position{x, y, weight[WHITE][x * 15 + y]}) == 1);
#endif
        nextPos[MAX].erase(Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])});
        nextPos[WHITE].erase(Position{x, y, weight[WHITE][x * 15 + y]});
        nextPos[BLACK].erase(Position{x, y, weight[BLACK][x * 15 + y]});
        weight[BLACK][x * 15 + y] = weight[WHITE][x * 15 + y] = -1;
    }
    myBoard.boardState[x][y] = color;
    // 更改为未放置=>增加点在weight和nextPos中的记录
    if (color == UNPLACE) {
        weight[BLACK][x * 15 + y] = myBoard.MarkOfPoint(x, y, BLACK);
        weight[WHITE][x * 15 + y] = myBoard.MarkOfPoint(x, y, WHITE);

        nextPos[MAX].insert(Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])});
        nextPos[WHITE].insert(Position{x, y, weight[WHITE][x * 15 + y]});
        nextPos[BLACK].insert(Position{x, y, weight[BLACK][x * 15 + y]});
    }
    // 修改完成后, 在9*9范围内修改空闲点的权值
    for (int i = max(0, x - 4); i < min(15, x + 5); i++) {
        for (int j = max(0, y - 4); j < min(15, y + 5); j++) {
            if ((i != x || j != y) && myBoard.boardState[i][j] == UNPLACE) {
// 删除现存权值记录
#ifndef ONLINE_JUDGE
                assert(nextPos[MAX].count(Position{
                           i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])}) == 1);
                assert(nextPos[BLACK].count(Position{i, j, weight[BLACK][i * 15 + j]}) == 1);
                assert(nextPos[WHITE].count(Position{i, j, weight[WHITE][i * 15 + j]}) == 1);
#endif
                nextPos[MAX].erase(Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].erase(Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].erase(Position{i, j, weight[BLACK][i * 15 + j]});
                // 求出新权值记录并保存
                weight[BLACK][i * 15 + j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i * 15 + j] = myBoard.MarkOfPoint(i, j, WHITE);
                // 新增记录
                nextPos[MAX].insert(Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].insert(Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].insert(Position{i, j, weight[BLACK][i * 15 + j]});
            }
        }
    }
}

LL Agent::Evaluate(int color) {
    LL tot = 0;
    for (auto& pos : nextPos[color]) tot += pos.w;
    for (auto& pos : nextPos[color ^ 1]) tot -= pos.w * 8 / 10;
    //return nextPos[color].begin()->w;
    return tot;
}

int Agent::Evaluate(int curColor) {
    // 所有可能位置
    auto &v = nextPos[curColor];
    int total = 0;
    for (const auto &i : v) {
        if (i != -1 && myBoard.boardState[i / SIZE][i % SIZE] == UNPLACE) {
            total += myBoard.MarkOfPoint(i / SIZE, i % SIZE, curColor);
        }
    }
    return total;
}

void Agent::PreDrop() {
    #ifdef DEBUG
    cout << "Before PreDrop():" << markcallcnt << '\n';
    #endif
    nextPos[WHITE].clear();
    nextPos[BLACK].clear();
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (myBoard.boardState[r][c] == UNPLACE) {
                nextPos[WHITE].push_back(r * 15 + c);
                myBoard.posValue[WHITE][r * 15 + c] = 14 - abs(r - 7) - abs(c - 7) + myBoard.MarkOfPoint(r, c, WHITE);
                nextPos[BLACK].push_back(r * 15 + c);
                myBoard.posValue[BLACK][r * 15 + c] = 14 - abs(r - 7) - abs(c - 7) + myBoard.MarkOfPoint(r, c, BLACK);
            } else {
                myBoard.posValue[WHITE][r * 15 + c] = -1;
                myBoard.posValue[BLACK][r * 15 + c] = -1;
            }
    #ifdef DEBUG
    cout << "After PreDrop():" << markcallcnt << '\n';
    #endif
    // 按优先级排序
    auto cmpWhite = [&](int &a, int &b) -> bool {
        return myBoard.posValue[WHITE][a] > myBoard.posValue[WHITE][b];
    };
    auto cmpBlack = [&](int &a, int &b) -> bool {
        return myBoard.posValue[BLACK][a] > myBoard.posValue[BLACK][b];
    };
    sort(nextPos[WHITE].begin(), nextPos[WHITE].end(), cmpWhite);
    sort(nextPos[BLACK].begin(), nextPos[BLACK].end(), cmpBlack);

}

#endif
