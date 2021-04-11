#define ONLINE_JUDGE
#define RELEASE
#ifdef RELEASE
// #pragma GCC optimize(2)
// #pragma GCC optimize(3, "Ofast", "inline")
#endif
#include <bits/stdc++.h>
using namespace std;
// #include <Const.h>
#ifndef CONST
#define CONST
// 放置一些公用常量，不要格式化

//#define ONLINE_JUDGE

// 为加快运行速度，用以下类型代替int(如果不超出范围)
typedef int_fast8_t Byte;
typedef uint_fast8_t UByte;

// 各局面价值表，待完善
const int valueTable = {0};
// 棋盘大小
const int SIZE = 15;
// 方向增量，只设置右，右下，下，左下
const int dr[] = {0, 1, 1, 1};
const int dc[] = {1, 1, 0, -1};

// 每个位置落子类型：
// -1 无子，0 白，1 黑，2 下标非法
const int UNPLACE = -1;
const int WHITE = 0;
const int BLACK = 1;
const int INVALID = -2;

// 棋子类型得分，参数可以调整
// 拆分远近活三，远中近活二
const int LIVEFOURMARK = 100000;
const int SLEEPFOURMARK = 50000;
const int NEARLIVETHREEMARK = 10200;
const int FARLIVETHREEMARK = 10000;
const int SLEEPTHREEMARK = 5000;
const int NEARLIVETWOMARK = 1200;
const int MIDLIVETWOMARK = 1050;
const int FARLIVETWOMARK = 1000;
const int SLEEPTWOMARK = 500;
const int ONEMARK = 1;

#endif
// #include <Board.hpp>
#ifndef BOARD_H
#define BOARD_H

#include <bits/stdc++.h>

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
    int MarkOfPoint(int, int, int);
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
    int maxMark = MarkOfPoint(v[0] / 15, v[0] % 15, color) + CentralMark(v[0] / 15, v[0] % 15),
        maxPoint = v[0];
    for (size_t id = 1; id < v.size(); id++) {
        auto curMark = MarkOfPoint(v[id] / 15, v[id] % 15, color) +
                       CentralMark(v[id] / 15, v[id] % 15);
        // cout << "curMark: " << curMark << "x: " << v[id] / 15 << "y: " << v[id] % 15 << "\n";
        if (curMark > maxMark) {
            maxMark = curMark;
            maxPoint = v[id];
        }
    }
    // 在敌方形成活三后尝试堵子
    int avoidOppoent = -1, oppoentMark = -1;
    for (size_t id = 0; id < v.size(); id++) {
        auto curMark = MarkOfPoint(v[id] / 15, v[id] % 15, !color) +
                       CentralMark(v[id] / 15, v[id] % 15);
        if (curMark > FARLIVETHREEMARK && curMark > oppoentMark) {
            avoidOppoent = v[id];
            oppoentMark = curMark;
        }
    }
    if (avoidOppoent != -1) {
        maxPoint = avoidOppoent;
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
    if (curX < 0 || curX >= SIZE || curY < 0 || curY >= SIZE) {
        return INVALID;
    }
    return boardState[curX][curY];
}

int Board::MarkOfPoint(int curX, int curY, int playerColor) {
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
            total += LIVEFOURMARK;
        }
        // 眠四SLEEPFOUR, #11112(3), 1#111, 11#11
        else if (RelativePosVal(curX, curY, i, 1) == playerColor &&
                 RelativePosVal(curX, curY, i, 2) == playerColor &&
                 RelativePosVal(curX, curY, i, 3) == playerColor &&
                 RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += SLEEPFOURMARK;
        } else if (RelativePosVal(curX, curY, i, -1) == playerColor &&
                   RelativePosVal(curX, curY, i, 1) == playerColor &&
                   RelativePosVal(curX, curY, i, 2) == playerColor &&
                   RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += SLEEPFOURMARK;
        } else if (RelativePosVal(curX, curY, i, -2) == playerColor &&
                   RelativePosVal(curX, curY, i, -1) == playerColor &&
                   RelativePosVal(curX, curY, i, 1) == playerColor &&
                   RelativePosVal(curX, curY, i, 2) == playerColor) {
            // cout << "SLEEPFOURMARK ";
            total += SLEEPFOURMARK;
        }
        // 远活三FARLIVETHREE, #0111
        else if (RelativePosVal(curX, curY, i, 1) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 2) == playerColor &&
                 RelativePosVal(curX, curY, i, 3) == playerColor &&
                 RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "LIVETHREEMARK ";
            total += FARLIVETHREEMARK;
        }
        // 近活三NEARLIVETHREE, 0#111, #1110
        else if (RelativePosVal(curX, curY, i, -1) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 1) == playerColor &&
                 RelativePosVal(curX, curY, i, 2) == playerColor &&
                 RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "LIVETHREEMARK ";
            total += NEARLIVETHREEMARK;
        } else if (RelativePosVal(curX, curY, i, 4) == UNPLACE &&
                   RelativePosVal(curX, curY, i, 1) == playerColor &&
                   RelativePosVal(curX, curY, i, 2) == playerColor &&
                   RelativePosVal(curX, curY, i, 3) == playerColor) {
            total += NEARLIVETHREEMARK;
        }
        // 眠三SLEEPTHREE, 1#11
        else if (RelativePosVal(curX, curY, i, -1) == playerColor &&
                 RelativePosVal(curX, curY, i, 1) == playerColor &&
                 RelativePosVal(curX, curY, i, 2) == playerColor) {
            // cout << "SLEEPTHREEMARK ";
            total += SLEEPTHREEMARK;
        }
        // 远活二 #0011
        else if (RelativePosVal(curX, curY, i, 1) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 2) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 3) == playerColor &&
                 RelativePosVal(curX, curY, i, 4) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += FARLIVETWOMARK;
        }
        // 中活二 0#011, #0110
        else if (RelativePosVal(curX, curY, i, -1) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 1) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 2) == playerColor &&
                 RelativePosVal(curX, curY, i, 3) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += MIDLIVETWOMARK;
        } else if (RelativePosVal(curX, curY, i, 4) == UNPLACE &&
                   RelativePosVal(curX, curY, i, 1) == UNPLACE &&
                   RelativePosVal(curX, curY, i, 2) == playerColor &&
                   RelativePosVal(curX, curY, i, 3) == playerColor) {
            total += MIDLIVETWOMARK;
        }
        // 近活二 00#11, 0#110, #1100
        else if (RelativePosVal(curX, curY, i, -2) == UNPLACE &&
                 RelativePosVal(curX, curY, i, -1) == UNPLACE &&
                 RelativePosVal(curX, curY, i, 1) == playerColor &&
                 RelativePosVal(curX, curY, i, 2) == playerColor) {
            // cout << "LIVETWOMARK ";
            total += NEARLIVETWOMARK;
        } else if (RelativePosVal(curX, curY, i, 3) == UNPLACE &&
                   RelativePosVal(curX, curY, i, -1) == UNPLACE &&
                   RelativePosVal(curX, curY, i, 1) == playerColor &&
                   RelativePosVal(curX, curY, i, 2) == playerColor) {
            total += NEARLIVETWOMARK;
        } else if (RelativePosVal(curX, curY, i, 3) == UNPLACE &&
                   RelativePosVal(curX, curY, i, 4) == UNPLACE &&
                   RelativePosVal(curX, curY, i, 1) == playerColor &&
                   RelativePosVal(curX, curY, i, 2) == playerColor) {
            total += NEARLIVETWOMARK;
        }
        // 眠二 1#1
        else if (RelativePosVal(curX, curY, i, -1) == playerColor &&
                 RelativePosVal(curX, curY, i, 1) == playerColor) {
            // cout << "SLEEPTWOMARK ";
            total += SLEEPTWOMARK;
        }
        // 只认为0000#1为活一
        else if ((RelativePosVal(curX, curY, i, -3) == UNPLACE) +
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
    DetermineBlack(input, turnID);
    for (int i = 0; i < turnID; i++) {
        myBoard.PlaceAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(),
                        !color);
        myBoard.PlaceAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(),
                        color);
    }
    myBoard.PlaceAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(),
                    !color);
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

int main() {
    ios::sync_with_stdio(false);
    auto ai = new Agent;
    ai->Run();
    delete ai;
    return 0;
}