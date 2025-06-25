#pragma once
#include "Maze_Read.h"
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

// 路径信息结构体
/*struct PathInfo {
    std::vector<Coordinate> path; // 路径坐标序列
    int score;                    // 总资源值
    PathInfo() : score(-INT_MAX) {}
};*/

class MazeDP {
private:
    const std::vector<std::vector<int>>& maze;          // 迷宫矩阵
    Coordinate start, dest;                            // 起点和终点
    std::vector<PositionInfo> resources;               // 资源信息
    std::vector<PositionInfo> traps;                   // 陷阱信息
    std::unordered_map<Coordinate, int> resourceMap;   // 资源位置到索引的映射

    // 检查坐标合法性
    bool isValid(const Coordinate& coord) const;

    // 获取相邻可走坐标
    std::vector<Coordinate> getNeighbors(const Coordinate& coord) const;

public:
    // 构造函数
    MazeDP(const Maze_Read& mazeRead);

    // 动态规划寻找最优路径
    PathInfo findOptimalPath();
};