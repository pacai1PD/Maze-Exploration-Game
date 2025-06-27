#include "Maze_Read.h"
#include "dp_Player.h"
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
#include <iostream>

// �ж������Ƿ�Ϸ�
bool MazeDP::isValid(const Coordinate& coord) const {
    return coord.x >= 0 && coord.x < maze.size() &&
        coord.y >= 0 && coord.y < maze[0].size() &&
        maze[coord.x][coord.y] != WALL;
}

// ��ȡ��������
std::vector<Coordinate> MazeDP::getNeighbors(const Coordinate& coord) const {
    std::vector<Coordinate> neighbors;
    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0} // �ҡ��¡�����
    };

    for (const auto& dir : directions) {
        Coordinate neighbor(coord.x + dir.first, coord.y + dir.second);
        if (isValid(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

// ���캯��
MazeDP::MazeDP(const Maze_Read& mazeRead) :
    maze(mazeRead.getMaze()),
    start(mazeRead.getStart()),
    dest(mazeRead.getDest()),
    resources(mazeRead.getResource()),
    traps(mazeRead.getTrap()) {

    // ��ʼ����Դλ�õ�������ӳ��
    for (size_t i = 0; i < resources.size(); ++i) {
        resourceMap[resources[i].position] = i;
    }
}

// ��������֮�������·��
PathInfo MazeDP::findPathBetweenPoints(const Coordinate& startPoint, const Coordinate& endPoint) {
    const int rows = maze.size();
    const int cols = maze[0].size();
    const int numResources = resources.size();
    const int totalMasks = 1 << numResources; // ��Դ״̬����

    // dp[x][y][mask]��ʾ����(x,y)����Դ״̬Ϊmaskʱ�������Դֵ
    std::vector<std::vector<std::vector<int>>> dp(
        rows, std::vector<std::vector<int>>(
            cols, std::vector<int>(totalMasks, INT_MIN)
        )
    );

    // ��¼·��
    std::vector<std::vector<std::vector<std::pair<int, int>>>> parent(
        rows, std::vector<std::vector<std::pair<int, int>>>(
            cols, std::vector<std::pair<int, int>>(
                totalMasks, { -1, -1 }
            )
        )
    );

    // ��ʼ�����״̬
    dp[startPoint.x][startPoint.y][0] = 0;
    std::queue<std::tuple<int, int, int>> q;
    q.push(std::make_tuple(startPoint.x, startPoint.y, 0));

    // BFS��������״̬
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        int x = std::get<0>(current);
        int y = std::get<1>(current);
        int mask = std::get<2>(current);

        // ��ȡ���ڽڵ�
        std::vector<Coordinate> neighbors = getNeighbors(Coordinate(x, y));
        for (const Coordinate& neighbor : neighbors) {
            int nx = neighbor.x;
            int ny = neighbor.y;
            int newMask = mask;
            int scoreDelta = 0;

            // ������Դ��
            if (resourceMap.find(neighbor) != resourceMap.end()) {
                int resIndex = resourceMap[neighbor];
                if (!(mask & (1 << resIndex))) {
                    newMask |= (1 << resIndex);
                    scoreDelta += resources[resIndex].value; // ��Դֻ�ܻ�ȡһ��
                }
            }

            // �������壨����ֻ����һ�Σ�
            if (maze[nx][ny] == TRAP) {
                scoreDelta += -3; // ÿ�δ�����3��
            }

            // ����DP״̬
            if (dp[x][y][mask] != INT_MIN) {
                int newScore = dp[x][y][mask] + scoreDelta;

                // ����Boss����ʱ��������
                if (maze[nx][ny] == BOSS) {
                    // ��ʱ��������
                }

                // �ؼ��޸ģ�ֻҪ��·����Դֵ���ߣ���ѡ�����
                if (newScore > dp[nx][ny][newMask]) {
                    dp[nx][ny][newMask] = newScore;
                    parent[nx][ny][newMask] = std::make_pair(x, y);
                    q.push(std::make_tuple(nx, ny, newMask));
                }
            }
        }
    }

    // Ѱ���յ������Դ״̬
    int maxResourcesCollected = 0;
    int bestMask = 0;

    for (size_t m = 0; m < totalMasks; ++m) {
        if (dp[endPoint.x][endPoint.y][m] != INT_MIN) {
            if (dp[endPoint.x][endPoint.y][m] > dp[endPoint.x][endPoint.y][bestMask]) {
                bestMask = m;
            }
        }
    }

    // ������Դ·��
    std::vector<Coordinate> resourcePath;
    int x = endPoint.x;
    int y = endPoint.y;
    int mask = bestMask;

    while (x != -1 && y != -1) {
        resourcePath.push_back(Coordinate(x, y));
        std::pair<int, int> prev = parent[x][y][mask];
        int prevX = prev.first;
        int prevY = prev.second;

        // ������Դ��ʱ��������Դ״̬
        if (resourceMap.find(Coordinate(x, y)) != resourceMap.end()) {
            int resIndex = resourceMap[Coordinate(x, y)];
            mask &= ~(1 << resIndex);
        }

        x = prevX;
        y = prevY;
    }

    // ��ת·��������㵽�յ㣩
    std::reverse(resourcePath.begin(), resourcePath.end());

    PathInfo pathInfo;
    pathInfo.path = resourcePath;
    pathInfo.score = dp[endPoint.x][endPoint.y][bestMask];
    return pathInfo;
}

// ��̬�滮Ѱ������·��
PathInfo MazeDP::findOptimalPath() {
    std::vector<Coordinate> allPoints;
    allPoints.push_back(start);
    for (const auto& res : resources) {
        allPoints.push_back(res.position);
    }
    allPoints.push_back(dest);

    std::vector<Coordinate> finalPath;
    int finalScore = 0;
    Coordinate currentPos = start;

    while (true) {
        int bestScore = INT_MIN;
        Coordinate nextPoint;
        std::vector<Coordinate> bestPath;

        for (const auto& point : allPoints) {
            if (point == currentPos) continue;

            PathInfo pathInfo = findPathBetweenPoints(currentPos, point);
            if (pathInfo.score > bestScore && pathInfo.score >= 0) {
                bestScore = pathInfo.score;
                nextPoint = point;
                bestPath = pathInfo.path;
            }
        }

        if (bestScore == INT_MIN) {
            // û���ҵ����ʵ�·����ֱ��ǰ���յ�
            PathInfo pathInfo = findPathBetweenPoints(currentPos, dest);
            if (pathInfo.score >= 0) {
                finalPath.insert(finalPath.end(), pathInfo.path.begin(), pathInfo.path.end());
                finalScore += pathInfo.score;
            }
            break;
        }

        // �Ƴ��Ѿ�����ĵ�
        auto it = std::find(allPoints.begin(), allPoints.end(), nextPoint);
        if (it != allPoints.end()) {
            allPoints.erase(it);
        }

        // ����·�������
        if (!finalPath.empty()) {
            bestPath.erase(bestPath.begin());
        }

        finalPath.insert(finalPath.end(), bestPath.begin(), bestPath.end());
        finalScore += bestScore;
        currentPos = nextPoint;

        if (currentPos == dest) {
            break;
        }
    }

    PathInfo pathInfo;
    pathInfo.path = finalPath;
    pathInfo.score = finalScore;
    return pathInfo;
}