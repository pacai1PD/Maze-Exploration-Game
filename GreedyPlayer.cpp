#include "GreedyPlayer.h"
#include <algorithm>
#include <climits>

// 构造函数实现
GreedyPlayer::GreedyPlayer(const Maze_Read& mazeRead) :
    maze(mazeRead.getMaze()),
    currentPos(mazeRead.getStart()),
    start(mazeRead.getStart()),
    dest(mazeRead.getDest()),
    resources(mazeRead.getResource()),
    collectedResourcePos() {
    // 初始化时将起点加入路径
}

// 检查坐标是否在3×3视野内
bool GreedyPlayer::isInVision(const Coordinate& pos, const Coordinate& visionCenter) const {
    return pos.x >= visionCenter.x - 1 && pos.x <= visionCenter.x + 1 &&
        pos.y >= visionCenter.y - 1 && pos.y <= visionCenter.y + 1;
}

// 计算曼哈顿距离
int GreedyPlayer::manhattanDistance(const Coordinate& a, const Coordinate& b) const {
    return std::abs(static_cast<int>(a.x - b.x)) + std::abs(static_cast<int>(a.y - b.y));
}

// 检查资源是否已收集
bool GreedyPlayer::isResourceCollected(const Coordinate& pos) const {
    for (const Coordinate& collectedPos : collectedResourcePos) {
        if (pos.x == collectedPos.x && pos.y == collectedPos.y) {
            return true;
        }
    }
    return false;
}

// 向目标位置移动一步（优先横向移动）
Coordinate GreedyPlayer::moveTowards(const Coordinate& target) const {
    Coordinate nextPos = currentPos;

    // 水平移动优先（左/右）
    if (currentPos.x < target.x && isValid(Coordinate(currentPos.x + 1, currentPos.y))) {
        nextPos.x += 1;
    }
    else if (currentPos.x > target.x && isValid(Coordinate(currentPos.x - 1, currentPos.y))) {
        nextPos.x -= 1;
    }
    // 垂直移动（上/下）
    else if (currentPos.y < target.y && isValid(Coordinate(currentPos.x, currentPos.y + 1))) {
        nextPos.y += 1;
    }
    else if (currentPos.y > target.y && isValid(Coordinate(currentPos.x, currentPos.y - 1))) {
        nextPos.y -= 1;
    }

    return nextPos;
}

// 检查坐标合法性（非墙壁且在迷宫范围内）
bool GreedyPlayer::isValid(const Coordinate& coord) const {
    return coord.x >= 0 && coord.x < maze.size() &&
        coord.y >= 0 && coord.y < maze[0].size() &&
        maze[coord.x][coord.y] != WALL;
}

// 贪心算法主函数实现
PathInfo GreedyPlayer::findGreedyPath() {
    std::vector<Coordinate> path;
    path.push_back(currentPos);  // 起点加入路径

    while (currentPos.x != dest.x || currentPos.y != dest.y) {
        Coordinate bestResourcePos;
        double bestRatio = -1.0;  // 性价比初始化为负数

        // 1. 寻找视野内性价比最高的资源
        for (const PositionInfo& res : resources) {
            if (isResourceCollected(res.position)) continue;  // 跳过已收集资源

            if (isInVision(res.position, currentPos)) {  // 检查是否在3×3视野内
                int distance = manhattanDistance(currentPos, res.position);
                if (distance == 0) continue;  // 避免除零错误

                double ratio = static_cast<double>(res.value) / distance;
                if (ratio > bestRatio) {
                    bestRatio = ratio;
                    bestResourcePos = res.position;
                }
            }
        }

        // 2. 决策移动方向
        if (bestRatio > 0) {  // 存在可拾取资源
            Coordinate nextPos = moveTowards(bestResourcePos);
            currentPos = nextPos;

            // 检查是否到达资源位置
            if (currentPos == bestResourcePos) {
                collectedResourcePos.push_back(bestResourcePos);
                for (const PositionInfo& res : resources) {
                    if (res.position == bestResourcePos) {
                        collectedResourcePos.push_back(bestResourcePos);
                        break;
                    }
                }
            }
        }
        else {  // 无资源可拾取，直接向终点移动
            Coordinate nextPos = moveTowards(dest);
            if (currentPos == nextPos) break;  // 无法移动，终止循环
            currentPos = nextPos;
        }

        path.push_back(currentPos);  // 记录当前位置

        // 检查是否到达终点
        if (currentPos == dest) break;
    }

    // 构建返回结果
    PathInfo pathInfo;
    pathInfo.path = path;
    pathInfo.score = collectedResourcePos.size() * 5;  // 假设每个资源值为5
    return pathInfo;
}