#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include "jsoncpp/json.h" // C++编译时默认包含此库

using namespace std;

const int SIZE = 15;
enum GridType
{
    NONE = 0,
    OTHER,
    ME
};

struct Board
{
    Board()
    {
        memset(grid, 0, SIZE * SIZE * sizeof(GridType));
    }
    void placeAt(int x, int y, GridType type)
    {
        if ((x >= 0 && x < SIZE) && (y >= 0 && y < SIZE))
        {
            grid[x][y] = type;
        }
    }
    void determineBlack(const Json::Value &input, int turnID)
    {
        if ((turnID != 0 && input["data"].asString() == "black") ||
            (input["requests"][turnID]["x"].asInt() == -1 &&
             input["requests"][turnID]["y"].asInt() == -1))
        {
            isBlack = true;
        }
    }
    GridType grid[SIZE][SIZE];
    // 黑棋为先手
    bool isBlack = false;
};
Board board;

int main()
{
    srand(time(NULL));
    // 读入JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    int turnID = input["responses"].size();
    for (int i = 0; i < turnID; i++)
    {
        board.placeAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), OTHER);
        board.placeAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), ME);
    }
    board.placeAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), OTHER);
    // 确定谁为先手
    board.determineBlack(input, turnID);
    // 做出决策存为myAction
    // 输出决策JSON
    Json::Value ret;
    if (!turnID)
    {
        ret["response"]["x"] = rand() % SIZE;
        ret["response"]["y"] = rand() % SIZE;
    } else {
        int posx = rand() % SIZE, posy = rand() % SIZE;
        while (board.grid[posx][posy] != NONE) {
            posx = rand() % SIZE;
            posy = rand() % SIZE;
        }
        ret["response"]["x"] = posx;
        ret["response"]["y"] = posy;
    }
    // 注明是否程序为先手
    if (board.isBlack)
        ret["data"] = "black";
    else
        ret["data"] = "white";
    // 输出结果
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    return 0;
}