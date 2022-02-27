# Gomoku
A gomoku player via Min-Max Search and Alpha-Beta Pruning, running on https://botzone.org.cn/

### 使用方法
在线评测：在 botzone 上创建 bot，将 main_single.cpp 上传。

人机对局：在首页创建游戏桌，一个玩家设为 Bot，另一个设为房主。

本地调试：注释掉首行 ```#define ONLINE_JUDGE ``` 再运行。

### bot 链接
8 层搜索：https://www.botzone.org.cn/account/6092697466ecf10ef5cc9676

10 层搜索：https://www.botzone.org.cn/user/605ecd5eb5a5d724a1fb9230


### 部分对局记录
https://www.botzone.org.cn/match/62197db4bbc22c318dc04a83

https://www.botzone.org.cn/match/62197dfcbbc22c318dc04ac1

https://www.botzone.org.cn/match/62197ceebbc22c318dc04a1d

https://www.botzone.org.cn/match/62197f8dbbc22c318dc04be9

### 现有问题
1. 棋力不够。
2. 棋形得分设置不合理（具体体现在 MarkOfPoint 函数以及 MARKS。这部分是队友设计的，改起来也挺麻烦，暂时保持这样），导致一些关键位置被赋予分数过低，间接造成第一个问题。下面是 1 个例子。

<center>
    <img style="border-radius: 0.3125em;" 
    src=".\img\sample.png">
    <br>
    <div style="color:orange; border-bottom: 1px solid #d9d9d9;
    display: inline-block;
    color: #999;
    padding: 2px;"></div>
</center>



