#ifndef CONST
#define CONST
#include <bits/stdc++.h>
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