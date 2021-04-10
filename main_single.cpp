#define ONLINE_JUDGE
#define RELEASE
#ifdef RELEASE
#pragma GCC optimize(2)
#pragma GCC optimize(3, "Ofast", "inline")
#endif
#include <bits/stdc++.h>
// #include "Const.h"
#ifndef CONST
#define CONST
// 放置一些公用常量，不要格式化

//#define ONLINE_JUDGE

// 为加快运行速度，用以下类型代替int(如果不超出范围)
typedef int_fast8_t Byte;
typedef uint_fast8_t UByte;


const int INF = 1E8;

// 各局面价值表，待完善
// const int valueTable = {0};
// 棋盘大小
const int SIZE = 15;
// 方向增量，只设置右，右下，下，左下，左，左上，上，右上, clockwise
const int dr[] = {0, 1, 1, 1, 0, -1, -1, -1};
const int dc[] = {1, 1, 0, -1, -1, -1, 0, 1};

// 每个位置落子类型：
// -1 无子，0 白，1 黑，2 下标非法
const int UNPLACE = -1;
const int WHITE = 0;
const int BLACK = 1;
const int INVALID = -2;

// 棋子类型得分，参数可以调整
const int LIVEFOURMARK = 100000;
const int SLEEPFOURMARK = 5000;
const int LIVETHREEMARK = 10000;
const int SLEEPTHREEMARK = 500;
const int LIVETWOMARK = 100;
const int SLEEPTWOMARK = 50;
const int ONEMARK = 1;

// bestDropId 表示未设置
const int BEST_UNDEFINED = -1;

#endif
// #include <Board.hpp>
#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>

using namespace std;
#ifdef DEBUG
int markcallcnt = 0;
#endif
struct Board {
    /*
     * 两个选手未落子位置得分，用于MinMaxSearch对选择位置进行排序，得分高的位置先选
     * 可能不需要两个，之后根据需求调整
     */
    int posValue[2][SIZE * SIZE];
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
    // 每个位置得分
    void CalcValue();
    // 和当前点相对位置的格子值
    int RelativePosVal(int, int, int, int);
    // 判断当前点的得分情况, 详见函数处注释
    int MarkOfPoint(int, int, int);
};

Board::Board() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            posValue[0][i * 15 + j] = posValue[1][i * 15 + j] = 0;
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

#ifdef DEBUG
    cout << "Chess Placed at: "
         << "(" << (int)x << ", " << (int)y << ")" << endl;
#endif
}

void Board::UnPlaceAt(int x, int y) { boardState[x][y] = UNPLACE; }

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
    int maxMark =
            MarkOfPoint(v[0] / 15, v[0] % 15, color) + CentralMark(v[0] / 15, v[0] % 15),
        maxPoint = v[0];
    for (size_t id = 1; id < v.size(); id++) {
        auto curMark =
            MarkOfPoint(v[id] / 15, v[id] % 15, color) + CentralMark(v[id] / 15, v[id] % 15);
        // cout << "curMark: " << curMark << "x: " << v[id] / 15 << "y: " <<
        // v[id] % 15 << "\n";
        if (curMark > maxMark) {
            maxMark = curMark;
            maxPoint = v[id];
        }
    }
    PlaceAt(maxPoint / 15, maxPoint % 15, color);
    return {maxPoint / 15, maxPoint % 15, true};
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

int Board::MarkOfPoint(int curX, int curY, int playerColor) {
    /*
     * 8个方向枚举
     * 以下注释以#表示(curX, curY), 1表示playerColor
     * 0表示无子, 3表示非法下标, 未注明的表示任意
     * 活的情况必须**中间同色棋子连续**, 中间有间断一定为眠
     * 默认返回正值分数
     */
#ifdef DEBUG
    markcallcnt++;
#endif
    int total = 0;
    for (int i = 0; i < 8; i++) {
        // 活四LIVEFOUR, #11110
        if (RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor &&
            RelativePosVal(curX, curY, i, 5) == UNPLACE) {
            // cout << "LIVEFOURMARK ";
            total += LIVEFOURMARK;
        }
        // 眠四SLEEPFOUR, #11112(3), 1#111, 11#11
        if (RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor &&
            (RelativePosVal(curX, curY, i, 5) == !playerColor ||
             RelativePosVal(curX, curY, i, 5) == INVALID)) {
            // cout << "SLEEPFOURMARK ";
            total += SLEEPFOURMARK;
        }
        if (RelativePosVal(curX, curY, i, -1) == playerColor &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += SLEEPFOURMARK;
        }
        if (RelativePosVal(curX, curY, i, -2) == playerColor &&
            RelativePosVal(curX, curY, i, -1) == playerColor &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += SLEEPFOURMARK;
        }
        // 活三LIVETHREE, #0111, 0#111
        if (RelativePosVal(curX, curY, i, 1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "LIVETHREEMARK ";
            total += LIVETHREEMARK;
        }
        if (RelativePosVal(curX, curY, i, -1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "LIVETHREEMARK ";
            total += LIVETHREEMARK;
        }
        // 眠三SLEEPTHREE, 1#11
        if (RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == UNPLACE &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "SLEEPTHREEMARK ";
            total += SLEEPTHREEMARK;
        }
        // 活二 #0011, 0#011, 00#11
        if (RelativePosVal(curX, curY, i, 1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 2) == UNPLACE &&
            RelativePosVal(curX, curY, i, 3) == playerColor &&
            RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += LIVETWOMARK;
        }
        // 眠二 1#1
        if (RelativePosVal(curX, curY, i, -1) == playerColor &&
            RelativePosVal(curX, curY, i, 1) == playerColor) {
            // cout << "SLEEPTWOMARK ";
            total += SLEEPTWOMARK;
        }
        if (RelativePosVal(curX, curY, i, -1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 2) == playerColor &&
            RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += LIVETWOMARK;
        }
        if (RelativePosVal(curX, curY, i, -2) == UNPLACE &&
            RelativePosVal(curX, curY, i, -1) == UNPLACE &&
            RelativePosVal(curX, curY, i, 1) == playerColor &&
            RelativePosVal(curX, curY, i, 2) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += LIVETWOMARK;
        }
        // 只认为0000#1为活一
        if ((RelativePosVal(curX, curY, i, -3) == UNPLACE) +
                (RelativePosVal(curX, curY, i, -2) == UNPLACE) +
                (RelativePosVal(curX, curY, i, -1) == UNPLACE) +
                (RelativePosVal(curX, curY, i, 1) == playerColor) ==
            1) {
            // cout << "ONEMARK ";
            total += ONEMARK;
        }
    }
    return total;
}

#endif
// #include <Agent.hpp>
#ifndef AGENT_HPP
#define AGENT_HPP

#include <bits/stdc++.h>

#include "jsoncpp/json.h"
#define UNUSED(x, y) (x) += (y)
//#include "Timer.hpp"
using namespace std;
#ifdef DEBUG
extern int markcallcnt;
#endif
struct Agent {
    // 搜索深度默认为 6, 优化后再升级
    const int SEARCH_DEPTH = 2;
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
    // 可能的落子位置
    vector<int> nextPos[2];

    // 运行
    void Run();
    // 从jason读入并初始化棋盘
    void Init();
    // 按 json 格式输出
    void Output();
    // 判断AI是否为先手
    void DetermineBlack(const Json::Value &, int);
    // 搜索
    int MinMaxSearch(int, int, int);
    // 局面估值
    int Evaluate(int);
    // 对某个位置八个方向检查连五
    bool CheckFive(int, int);
    // 搜索之前: 预处理得分情况
    void PreDrop();
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
            if (tr < 0 || tr >= SIZE || tc < 0 || tc >= SIZE)
                break;
            if (myBoard.boardState[tr][tc] != color)
                break;
            cnt++;
        }
        for (int s = 1; s <= 4; s++) {
            tr = r - dr[k] * s;
            tc = c - dc[k] * s;
            if (tr < 0 || tr >= SIZE || tc < 0 || tc >= SIZE)
                break;
            if (myBoard.boardState[tr][tc] != color)
                break;
            cnt++;
        }
        if (cnt >= 5)
            return true;
    }
    return false;
}

int Agent::MinMaxSearch(int depth, int alpha, int beta) {
    bool curColor = (depth & 1) ? myColor : myColor ^ 1;
    // if (CheckFive(curColor, lastDropId))
    //     return INF;
    auto &v = nextPos[curColor];
    if (depth <= 0) {
        #ifdef DEBUG
        cout << "Before Evaulate()" << markcallcnt << '\n';
        #endif
        int res = Evaluate(curColor);
        #ifdef DEBUG
        cout << "After Evaulate()" << markcallcnt << '\n';
        #endif
        return res;
    }
    if (!nextPos[curColor].size())
        return -INF; //无子可走
    // 对所有可能局面进行搜索
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == -1 || myBoard.boardState[v[i] / 15][v[i] % 15] != UNPLACE)
            continue;
        // 记录落子位置
        myBoard.PlaceAt(v[i] / 15, v[i] % 15, curColor);
        int tmpId = lastDropId;
        lastDropId = v[i];
        v[i] = -1;
        // 继续搜索
        int val = -MinMaxSearch(depth - 1, -beta, -alpha);
        // 取消落子
        v[i] = lastDropId;
        myBoard.UnPlaceAt(v[i] / 15, v[i] % 15);
        lastDropId = tmpId;
        if (val >= beta) {
            if (depth == SEARCH_DEPTH)
                bestScore = val, bestDropId = v[i];
            return val;
        }
        if (val > alpha)
            alpha = val; // bestDropId
        if (depth == SEARCH_DEPTH && (val > bestScore || bestDropId == BEST_UNDEFINED))
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
        bestDropId = BEST_UNDEFINED;
        PreDrop();
        #ifdef DEBUG
        clock_t clk0, clk1;
        clk0 = clock();
        #endif
        MinMaxSearch(SEARCH_DEPTH, -INF, INF);
        #ifdef DEBUG
        clk1 = clock();
        cout << (double)(clk1 - clk0) / CLOCKS_PER_SEC << ' ' << markcallcnt << '\n';
        #endif
        myBoard.PlaceAt(bestDropId / 15, bestDropId % 15, myColor);
        myBoard.Show();
        if (!bestDropId)
            break;
        if (CheckFive(WHITE, bestDropId)) {
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
    for (int i = 0; i < turnID; i++) {
        myBoard.PlaceAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(),
                        BLACK);
        myBoard.PlaceAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(),
                        WHITE);
    }
    myBoard.PlaceAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(),
                    BLACK);
    DetermineBlack(input, turnID);
    bestDropId = BEST_UNDEFINED;
    PreDrop();
    MinMaxSearch(SEARCH_DEPTH, -INF, INF);
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

void Agent::DetermineBlack(const Json::Value &input, int turnID) {
    if ((turnID != 0 && input["data"].asString() == "black") ||
        (input["requests"][turnID]["x"].asInt() == -1 &&
         input["requests"][turnID]["y"].asInt() == -1)) {
        myColor = BLACK;
    }
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


using namespace std;

int main() {
    ios::sync_with_stdio(false);
    srand(time(0));
    auto ai = new Agent;
    ai->Run();
    delete ai;
    return 0;
}