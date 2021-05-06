#ifndef CONST
#define CONST
#include <bits/stdc++.h>
// 放置一些公用常量，不要格式化

#define debug(x) cerr << #x << " = " << x << endl

//#define ONLINE_JUDGE

// 为加快运行速度，用以下类型代替int(如果不超出范围)
typedef int_fast8_t Byte;
typedef uint_fast8_t UByte;
typedef long long LL;

const LL INF = 1E16;

// bestDropId 表示未设置
const int BEST_UNDEFINED = -1;

// 最大分支数
const int BRANCH_LIMIT = 32;

// 搜索深度默认为 6, 优化后再升级
const int SEARCH_DEPTH = 6;

// 各局面价值表，待完善
const int valueTable = {0};
// 棋盘大小
const int SIZE = 15;
// 方向增量
const int dr[] = {0, 1, 1, 1, 0, -1, -1, -1};
const int dc[] = {1, 1, 0, -1, -1, 1, 0, -1};

// 每个位置落子类型：
// -1 无子，0 白，1 黑，2 下标非法
const int UNPLACE = -1;
const int WHITE = 0;
const int BLACK = 1;
const int MAX = 2;
const int INVALID = -2;

// 棋子类型得分，参数可以调整
// 拆分远近活三，远中近活二
const LL LIVEFOURMARK = 2000000;
const LL SLEEPFOURMARK = 1000000;
const LL NEARLIVETHREEMARK = 102000;
const LL FARLIVETHREEMARK = 100000;
const LL SLEEPTHREEMARK = 50000;
const LL NEARLIVETWOMARK = 1200;
const LL MIDLIVETWOMARK = 1050;
const LL FARLIVETWOMARK = 1000;
const LL SLEEPTWOMARK = 500;
const LL ONEMARK = 1;

const LL MARKS[][2]={{3, 1}, {1000, 20}, {100000, 20000}, {10000000, 200000}};

int SEARCHCNT[] = {0, 4, 4, 4, 6, 6, 9};
#endif