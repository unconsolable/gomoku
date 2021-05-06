#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "Board.hpp"
#include "Position.hpp"
#include "Timer.hpp"
#include "jsoncpp/json.h"
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
    LL bestScore = -INF;
    // 落子位置和对应的权重, 用于在set中查找.
    // -1表示有棋子的点
    LL weight[2][225];
    // 按照权排序的落子位置集合
    set<Position> nextPos[3];
    // 白子和黑子的权值之和
    LL sumWeight[2];

    // 运行
    void Run();
    // 初始化局面
    void Init();
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
    // set<Position> tmp;
    auto pos = nextPos[MAX].begin();
    // 部分拷贝
    // for(int i = 0; i < SEARCHCNT[depth] && pos != nextPos[MAX].end(); i++) {
    //     tmp.insert(*pos);
    //     pos++;
    // }
    // 全拷贝
    /*for(int i = 0; i < nextPos[MAX].size(); i++) {
        tmp.insert(*pos);
        pos++;
    }*/
    for(int i = 0; i < SEARCHCNT[depth] && pos != nextPos[MAX].end(); i++, pos++) {
        auto x = pos->x, y = pos->y;
        auto w = pos->w;
        Update(x, y, curColor);
        // 更改上一次落子位置
        int tmpId = lastDropId;
        lastDropId = x * 15 + y;
        // 继续搜索
        LL val = -MinMaxSearch(depth - 1, -beta, -alpha, !curColor);
        // 取消落子 更新得分
        Update(x, y, UNPLACE);
        pos = nextPos[MAX].find(Position{x, y, w});
        assert(pos != nextPos[MAX].end());
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
#else
    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    DetermineBlack(input);
    for (int i = 0; i < turnID; i++) {
        myBoard.PlaceAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), !color);
        myBoard.PlaceAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), color);
    }
    myBoard.PlaceAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), !color);
    Preplay();
    MinMaxSearch(SEARCH_DEPTH, -INF, INF, color);
    Json::Value ret;
    ret["response"]["x"] = bestDropId / 15;
    ret["response"]["y"] = bestDropId % 15;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
#endif
}

void Agent::Init() {
    // 根据json输入恢复棋盘状态
    // 最后一对坐标为 (-1,-1) 则 color 设为 1，我方先手
    // 否则，更新棋盘状态，我方后手
    // 本地测试不需要恢复，直接跳过
}

void Agent::DetermineBlack(const Json::Value &input) {
    if (input["requests"][0u]["x"].asInt() == -1 && input["requests"][0u]["y"].asInt() == -1) {
        color = BLACK;
    } else {
        color = WHITE;
    }
}

void Agent::Preplay() {
    // 初始化weight和nextPos
    sumWeight[0] = sumWeight[1] = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // 如果位置上已有棋子, 标记权重为-1
            // 否则计算权重, 并分别存放在weight和nextPos中
            if (myBoard.boardState[i][j] != UNPLACE) {
                weight[BLACK][i * 15 + j] = weight[WHITE][i * 15 + j] = -1;           
            } else {
                weight[BLACK][i * 15 + j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i * 15 + j] = myBoard.MarkOfPoint(i, j, WHITE);
                sumWeight[BLACK] += weight[BLACK][i * 15 + j];
                sumWeight[WHITE] += weight[WHITE][i * 15 + j];
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
                   Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])}) == 1);
        assert(nextPos[WHITE].count(Position{x, y, weight[WHITE][x * 15 + y]}) == 1);
        assert(nextPos[BLACK].count(Position{x, y, weight[BLACK][x * 15 + y]}) == 1);
#endif
        nextPos[MAX].erase(Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])});
        nextPos[WHITE].erase(Position{x, y, weight[WHITE][x * 15 + y]});
        nextPos[BLACK].erase(Position{x, y, weight[BLACK][x * 15 + y]});
        sumWeight[WHITE] -= weight[WHITE][x * 15 + y];
        sumWeight[BLACK] -= weight[BLACK][x * 15 + y];
        weight[BLACK][x * 15 + y] = weight[WHITE][x * 15 + y] = -1;
    }
    myBoard.boardState[x][y] = color;
    // 更改为未放置=>增加点在weight和nextPos中的记录
    if (color == UNPLACE) {
        weight[BLACK][x * 15 + y] = myBoard.MarkOfPoint(x, y, BLACK);
        weight[WHITE][x * 15 + y] = myBoard.MarkOfPoint(x, y, WHITE);
        sumWeight[WHITE] += weight[WHITE][x * 15 + y];
        sumWeight[BLACK] += weight[BLACK][x * 15 + y];
        nextPos[MAX].insert(Position{x, y, max(weight[WHITE][x * 15 + y], weight[BLACK][x * 15 + y])});
        nextPos[WHITE].insert(Position{x, y, weight[WHITE][x * 15 + y]});
        nextPos[BLACK].insert(Position{x, y, weight[BLACK][x * 15 + y]});
    }
    // 修改完成后, 在8*4范围内修改空闲点的权值
    for (int dir = 0; dir < 8; dir++) {
        for (int off = 1; off < 5; off++) {
            if (myBoard.RelativePosVal(x, y, dir, off) == UNPLACE) {
                int i = x + dr[dir] * off, j = y + dc[dir] * off;
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
                sumWeight[WHITE] -= weight[WHITE][i * 15 + j];
                sumWeight[BLACK] -= weight[BLACK][i * 15 + j];
                // 求出新权值记录并保存
                weight[BLACK][i * 15 + j] = myBoard.MarkOfPoint(i, j, BLACK);
                weight[WHITE][i * 15 + j] = myBoard.MarkOfPoint(i, j, WHITE);
                sumWeight[WHITE] += weight[WHITE][i * 15 + j];
                sumWeight[BLACK] += weight[BLACK][i * 15 + j];
                // 新增记录
                nextPos[MAX].insert(Position{i, j, max(weight[WHITE][i * 15 + j], weight[BLACK][i * 15 + j])});
                nextPos[WHITE].insert(Position{i, j, weight[WHITE][i * 15 + j]});
                nextPos[BLACK].insert(Position{i, j, weight[BLACK][i * 15 + j]});
            }
        }
    }
}

LL Agent::Evaluate(int color) {
    // LL tot = 0;
    // for (auto& pos : nextPos[color]) tot += pos.w;
    // for (auto& pos : nextPos[color ^ 1]) tot -= pos.w * 8 / 10;
    return sumWeight[color] - (sumWeight[color ^ 1] * 8 / 10);
    //return nextPos[color].begin()->w;
    // return tot;
}

#endif
