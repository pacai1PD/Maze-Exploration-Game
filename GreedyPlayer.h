#pragma once
#include "Maze_Read.h"
#include <vector>
#include <cmath>

// 路径信息结构体
/*struct PathInfo {
    std::vector<Coordinate> path;  // 路径坐标序列
    int score;        // 已收集资源总值
    PathInfo() : score(0) {}
};*/

// 贪心算法玩家类（封装版）
class GreedyPlayer {
private:
    const std::vector<std::vector<int>>& maze;       // 迷宫矩阵引用
    Coordinate currentPos;                          // 当前位置
    Coordinate start, dest;                         // 起点和终点
    std::vector<PositionInfo> resources;            // 资源信息
    std::vector<Coordinate> collectedResourcePos;   // 已收集资源的位置

    // 检查坐标是否在3×3视野内
    bool isInVision(const Coordinate& pos, const Coordinate& visionCenter) const;

    // 计算曼哈顿距离
    int manhattanDistance(const Coordinate& a, const Coordinate& b) const;

    // 检查资源是否已收集
    bool isResourceCollected(const Coordinate& pos) const;

    // 向目标位置移动一步
    Coordinate moveTowards(const Coordinate& target) const;

    // 检查坐标合法性
    bool isValid(const Coordinate& coord) const;

public:
    // 构造函数：接收迷宫读取器对象
    explicit GreedyPlayer(const Maze_Read& mazeRead);

    // 贪心算法主函数：获取最优资源拾取路径
    PathInfo findGreedyPath();
};