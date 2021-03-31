#ifndef CONST
#define CONST
// 放置一些公用常量，不要格式化
#include <bits/stdc++.h>
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
const int LIVEFOURMARK = 10000;
const int SLEEPFOURMARK = 5000;
const int LIVETHREEMARK = 1000;
const int SLEEPTHREEMARK = 500;
const int LIVETWOMARK = 100;
const int SLEEPTWOMARK = 50;
const int ONEMARK = 1;

// bestDropId 表示未设置
const int BEST_UNDEFINED = -1;

#endif