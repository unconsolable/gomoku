#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "../jsoncpp/json.h"
#include "Board.hpp"
#include "Position.hpp"
#include "Timer.hpp"
#define UNUSED(x, y) (x) += (y)

using namespace std;
struct Agent {
    // 默认后手，持白子
    int color;
    //计时器
    Timer *myTimer;
    // 棋盘
    Board myBoard;
    // 最优落子
    pair<int, int> bestDropPos;
    // 地方上一次落子位置
    pair<int, int> lastDropPos;
    // 最高得分
    LL bestScore;
    // 落子位置和对应的权重, 用于在set中查找.
    // -1表示有棋子的点
    LL weight[2][15][15];
    // 按照权排序的落子位置集合
    set<Position> nextPos[3];
    // 白子和黑子的权值之和
    LL sumWeight[2];

    // 构造函数
    Agent() {
        color = WHITE;
        myTimer = nullptr;
        bestDropPos = lastDropPos = {-1, -1};
        bestScore = -INF;
        sumWeight[0] = sumWeight[1] = 0;
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++) {
                weight[BLACK][i][j] = weight[WHITE][i][j] = 0;
                nextPos[MAX].insert(Position{i, j, 0});
                nextPos[WHITE].insert(Position{i, j, 0});
                nextPos[BLACK].insert(Position{i, j, 0});
            }
    }
    ~Agent() { delete myTimer; }
    // 运行
    void Run();
    // 判断AI是否为先手
    void DetermineColor(const Json::Value &);
    // 局面估值
    LL Evaluate(int);
    // 更改一个点的颜色, 同时更新附近的 9 * 4 格子内的空闲点权值信息
    void Update(int x, int y, int color);
    // 局面预处理
    void Init();
    // 搜素
    LL MinMaxSearch(int, LL, LL, bool);
};

LL Agent::MinMaxSearch(int depth, LL alpha, LL beta, bool curColor) {
    // 对手五连应该直接返回 -INF
    if (lastDropPos != POS_UNDEFINED &&
        myBoard.CheckFive(lastDropPos.first, lastDropPos.second, curColor ^ 1))
        return -INF;
    // 层数用完，估值返回
    if (depth <= 0) return Evaluate(curColor);
    // 无子可走
    if (!nextPos[MAX].size()) return -INF;

    auto pos = nextPos[MAX].begin();
    for (int i = 0; i < SEARCHCNT[depth] && pos != nextPos[MAX].end();
         i++, pos++) {
        int x = pos->x, y = pos->y;
        int w = pos->w;
        Update(x, y, curColor);
        // 更改上一次落子位置
        pair<int, int> tmpId = lastDropPos;
        lastDropPos = {x, y};
        // 继续搜索
        LL val = -MinMaxSearch(depth - 1, -beta, -alpha, !curColor);
        // 取消落子 更新得分
        Update(x, y, UNPLACE);
        pos = nextPos[MAX].find(Position{x, y, w});
        assert(pos != nextPos[MAX].end());
        // 恢复上一次落子位置
        lastDropPos = tmpId;
        if (val >= beta) {
            if (depth == SEARCH_DEPTH) bestScore = val, bestDropPos = {x, y};
            return val;
        }
        if (val > alpha) alpha = val;  // bestDropPos
        if (depth == SEARCH_DEPTH &&
            (val > bestScore || bestDropPos == POS_UNDEFINED))
            bestScore = val, bestDropPos = {x, y};
    }
    return alpha;
}

void Agent::Run() {
#ifndef ONLINE_JUDGE
    myBoard.Show();
    while (true) {
        Init();
        int x, y;
        cout << "Your drop position: ";
        cin >> x >> y;
        if (myBoard.RelativePosState(x, y, 0, 0) != UNPLACE) {
            cout << "This position is already occupied!" << endl;
            continue;
        }
        Update(x, y, BLACK);
        if (myBoard.CheckFive(BLACK)) {
            myBoard.Show();
            cout << "you win" << endl;
            break;
        }
        myTimer = new Timer;
        myTimer->prepare(__LINE__);

        MinMaxSearch(SEARCH_DEPTH, -INF, INF, WHITE);

        myTimer->getTimePass(__LINE__);
        cout << "Opponent: " << bestDropPos.first << " " << bestDropPos.second
             << endl;
        // 落子
        Update(bestDropPos.first, bestDropPos.second, WHITE);
        debug(nextPos[WHITE].size());
        debug(nextPos[BLACK].size());
        debug(nextPos[MAX].size());
        debug(bestScore);
        myBoard.Show();
        if (myBoard.CheckFive(WHITE)) {
            cout << "you lose" << endl;
            break;
        }
    }
#else
    Init();
    MinMaxSearch(SEARCH_DEPTH, -INF, INF, color);
    Json::Value ret;
    ret["response"]["x"] = bestDropPos.first;
    ret["response"]["y"] = bestDropPos.second;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
#endif
}

void Agent::DetermineColor(const Json::Value &input) {
    if (input["requests"][0u]["x"].asInt() == -1 &&
        input["requests"][0u]["y"].asInt() == -1) {
        color = BLACK;
    } else {
        color = WHITE;
    }
}

void Agent::Init() {
#ifndef ONLINE_JUDGE
    lastDropPos = bestDropPos = POS_UNDEFINED;
    bestScore = -INF;
#else
    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    DetermineColor(input);
    for (int i = 0; i < turnID; i++) {
        Update(input["requests"][i]["x"].asInt(),
               input["requests"][i]["y"].asInt(), !color);
        Update(input["responses"][i]["x"].asInt(),
               input["responses"][i]["y"].asInt(), color);
    }
    Update(input["requests"][turnID]["x"].asInt(),
           input["requests"][turnID]["y"].asInt(), !color);
#endif
}

void Agent::Update(int x, int y, int color) {
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
        return;
    }
    // 检查是否均为或均不为未放置
    assert(!(weight[BLACK][x][y] == -1) ^ (weight[WHITE][x][y] == -1));
    // 原始为未放置=>清除点在weight和nextPos中的记录
    if (weight[BLACK][x][y] != -1 && weight[WHITE][x][y] != -1) {
#ifndef ONLINE_JUDGE
        assert(nextPos[MAX].count(Position{
                   x, y, max(weight[WHITE][x][y], weight[BLACK][x][y])}) == 1);
        assert(nextPos[WHITE].count(Position{x, y, weight[WHITE][x][y]}) == 1);
        assert(nextPos[BLACK].count(Position{x, y, weight[BLACK][x][y]}) == 1);
#endif
        nextPos[MAX].erase(
            Position{x, y, max(weight[WHITE][x][y], weight[BLACK][x][y])});
        nextPos[WHITE].erase(Position{x, y, weight[WHITE][x][y]});
        nextPos[BLACK].erase(Position{x, y, weight[BLACK][x][y]});
        sumWeight[WHITE] -= weight[WHITE][x][y];
        sumWeight[BLACK] -= weight[BLACK][x][y];
        weight[BLACK][x][y] = weight[WHITE][x][y] = -1;
    }
    myBoard.boardState[x][y] = color;
    // 更改为未放置=>增加点在weight和nextPos中的记录
    if (color == UNPLACE) {
        weight[BLACK][x][y] = myBoard.MarkOfPoint(x, y, BLACK);
        weight[WHITE][x][y] = myBoard.MarkOfPoint(x, y, WHITE);
        sumWeight[WHITE] += weight[WHITE][x][y];
        sumWeight[BLACK] += weight[BLACK][x][y];
        nextPos[MAX].insert(
            Position{x, y, max(weight[WHITE][x][y], weight[BLACK][x][y])});
        nextPos[WHITE].insert(Position{x, y, weight[WHITE][x][y]});
        nextPos[BLACK].insert(Position{x, y, weight[BLACK][x][y]});
    }
    // 修改完成后, 在8*4范围内修改空闲点的权值
    for (int dir = 0; dir < 8; dir++) {
        int i = x, j = y;
        for (int off = 1; off < 5; off++) {
            i += dr[dir], j += dc[dir];
            if (myBoard.RelativePosState(i, j, 0, 0) == UNPLACE) {
// 删除现存权值记录
#ifndef ONLINE_JUDGE
                assert(
                    nextPos[MAX].count(Position{
                        i, j, max(weight[WHITE][i][j], weight[BLACK][i][j])}) ==
                    1);
                assert(nextPos[BLACK].count(
                           Position{i, j, weight[BLACK][i][j]}) == 1);
                assert(nextPos[WHITE].count(
                           Position{i, j, weight[WHITE][i][j]}) == 1);
#endif
                nextPos[MAX].erase(Position{
                    i, j, max(weight[WHITE][i][j], weight[BLACK][i][j])});
                nextPos[WHITE].erase(Position{i, j, weight[WHITE][i][j]});
                nextPos[BLACK].erase(Position{i, j, weight[BLACK][i][j]});
                sumWeight[WHITE] -= weight[WHITE][i][j];
                sumWeight[BLACK] -= weight[BLACK][i][j];
                // 求出新权值记录并保存
                weight[BLACK][i][j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i][j] = myBoard.MarkOfPoint(i, j, WHITE);
                sumWeight[WHITE] += weight[WHITE][i][j];
                sumWeight[BLACK] += weight[BLACK][i][j];
                // 新增记录
                nextPos[MAX].insert(Position{
                    i, j, max(weight[WHITE][i][j], weight[BLACK][i][j])});
                nextPos[WHITE].insert(Position{i, j, weight[WHITE][i][j]});
                nextPos[BLACK].insert(Position{i, j, weight[BLACK][i][j]});
            }
        }
    }
}

LL Agent::Evaluate(int color) {
    return sumWeight[color] - (sumWeight[color ^ 1] * 8 / 10);
}

#endif
