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

#endif