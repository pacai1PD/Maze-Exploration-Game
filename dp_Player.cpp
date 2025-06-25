#include "Maze_Read.h"
#include "dp_Player.h"
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

// 检查坐标是否合法
bool MazeDP::isValid(const Coordinate& coord) const {
    return coord.x >= 0 && coord.x < maze.size() &&
        coord.y >= 0 && coord.y < maze[0].size() &&
        maze[coord.x][coord.y] != WALL;
}

// 获取相邻坐标
std::vector<Coordinate> MazeDP::getNeighbors(const Coordinate& coord) const {
    std::vector<Coordinate> neighbors;
    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0} // 右、下、左、上
    };

    for (const auto& dir : directions) {
        Coordinate neighbor(coord.x + dir.first, coord.y + dir.second);
        if (isValid(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

// 构造函数
MazeDP::MazeDP(const Maze_Read& mazeRead) :
    maze(mazeRead.getMaze()),
    start(mazeRead.getStart()),
    dest(mazeRead.getDest()),
    resources(mazeRead.getResource()),
    traps(mazeRead.getTrap()) {

    // 构建资源位置到索引的映射
    for (size_t i = 0; i < resources.size(); ++i) {
        resourceMap[resources[i].position] = i;
    }
}

// 动态规划寻找最优路径
PathInfo MazeDP::findOptimalPath() {
    const int rows = maze.size();
    const int cols = maze[0].size();
    const int numResources = resources.size();
    const int totalMasks = 1 << numResources; // 资源状态总数

    // dp[x][y][mask]表示到达(x,y)且资源状态为mask时的最大资源值
    std::vector<std::vector<std::vector<int>>> dp(
        rows, std::vector<std::vector<int>>(
            cols, std::vector<int>(totalMasks, INT_MIN)
        )
    );

    // 记录路径
    std::vector<std::vector<std::vector<std::pair<int, int>>>> parent(
        rows, std::vector<std::vector<std::pair<int, int>>>(
            cols, std::vector<std::pair<int, int>>(
                totalMasks, { -1, -1 }
            )
        )
    );

    // 初始化起点
    dp[start.x][start.y][0] = 0;
    std::queue<std::tuple<int, int, int>> q;
    q.push(std::make_tuple(start.x, start.y, 0));

    // BFS遍历所有状态
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        int x = std::get<0>(current);
        int y = std::get<1>(current);
        int mask = std::get<2>(current);

        // 获取所有相邻坐标
        std::vector<Coordinate> neighbors = getNeighbors(Coordinate(x, y));
        for (const Coordinate& neighbor : neighbors) {
            int nx = neighbor.x;
            int ny = neighbor.y;
            int newMask = mask;
            int scoreDelta = 0;

            // 处理资源收集
            if (resourceMap.find(neighbor) != resourceMap.end()) {
                int resIndex = resourceMap[neighbor];
                if (!(mask & (1 << resIndex))) {
                    newMask |= (1 << resIndex);
                    scoreDelta += resources[resIndex].value; // 资源只能收集一次
                }
            }

            // 处理陷阱（陷阱可以多次触发）
            if (maze[nx][ny] == TRAP) {
                scoreDelta += -3; // 陷阱每次触发扣3分
            }

            // 更新DP数组
            if (dp[x][y][mask] != INT_MIN) {
                int newScore = dp[x][y][mask] + scoreDelta;
                if (newScore > dp[nx][ny][newMask]) {
                    dp[nx][ny][newMask] = newScore;
                    parent[nx][ny][newMask] = std::make_pair(x, y);
                    q.push(std::make_tuple(nx, ny, newMask));
                }
            }
        }
    }

    // 寻找终点的最大资源值
    int maxScore = INT_MIN;
    int bestMask = 0;
    for (int mask = 0; mask < totalMasks; ++mask) {
        if (dp[dest.x][dest.y][mask] > maxScore) {
            maxScore = dp[dest.x][dest.y][mask];
            bestMask = mask;
        }
    }

    // 回溯路径
    std::vector<Coordinate> path;
    int x = dest.x;
    int y = dest.y;
    int mask = bestMask;
    while (x != -1 && y != -1) {
        path.push_back(Coordinate(x, y));
        std::pair<int, int> prev = parent[x][y][mask];
        int prevX = prev.first;
        int prevY = prev.second;

        // 更新资源状态（回溯时清除已收集的资源位）
        if (resourceMap.find(Coordinate(x, y)) != resourceMap.end()) {
            int resIndex = resourceMap[Coordinate(x, y)];
            mask &= ~(1 << resIndex);
        }

        x = prevX;
        y = prevY;
    }

    // 反转路径（从起点到终点）
    std::reverse(path.begin(), path.end());

    PathInfo pathInfo;
    pathInfo.path = path;
    pathInfo.score = maxScore;
    return pathInfo;
}