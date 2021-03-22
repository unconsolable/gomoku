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
    ME,
    INVAILD
};

struct Board
{
    // 坐标x为水平方向
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
    // curx, cury为当前坐标, dir为方向, offset为偏移量(可正可负)
    GridType relativeGridType(int curx, int cury, int dir, int offset) 
    {
        int x = curx, y = cury;
        // dir: 0右, 1右下, 2下, 3左下, 4左, 5左上, 6上, 7右上
        switch (dir) {
            case 0:
                y += offset;
                break;
            case 1:
                x += offset;
                y += offset;
                break;
            case 2:
                x += offset;
                break;
            case 3:
                x -= offset;
                y += offset;
                break;
            case 4:
                x -= offset;
                break;
            case 5:
                x -= offset;
                y -= offset;
                break;
            case 6:
                x -= offset;
                break;
            case 7:
                x += offset;
                y -= offset;
                break;
        }
        if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
            return INVAILD;
        }
        return grid[x][y];
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