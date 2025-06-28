#pragma once
#include "Maze_Read.h"
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

class MazeDP {
private:
    const std::vector<std::vector<int>>& maze;          // 迷宫矩阵
    Coordinate start, dest;                            // 起点和终点
    std::vector<PositionInfo> resources;               // 资源信息
    std::vector<PositionInfo> traps;                   // 陷阱信息
    std::unordered_map<Coordinate, int> resourceMap;   // 资源位置到索引的映射

    // 判断坐标是否合法
    bool isValid(const Coordinate& coord) const;

    // 获取相邻坐标
    std::vector<Coordinate> getNeighbors(const Coordinate& coord) const;

    // 计算两点之间的最优路径
    PathInfo findPathBetweenPoints(const Coordinate& startPoint, const Coordinate& endPoint);

public:
    // 构造函数
    MazeDP(const Maze_Read& mazeRead);

    // 动态规划寻找最优路径
    PathInfo findOptimalPath();
};