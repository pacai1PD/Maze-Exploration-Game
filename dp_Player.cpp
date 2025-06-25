#include "Maze_Read.h"
#include "dp_Player.h"
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

// ��������Ƿ�Ϸ�
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

    // ������Դλ�õ�������ӳ��
    for (size_t i = 0; i < resources.size(); ++i) {
        resourceMap[resources[i].position] = i;
    }
}

// ��̬�滮Ѱ������·��
PathInfo MazeDP::findOptimalPath() {
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

    // ��ʼ�����
    dp[start.x][start.y][0] = 0;
    std::queue<std::tuple<int, int, int>> q;
    q.push(std::make_tuple(start.x, start.y, 0));

    // BFS��������״̬
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        int x = std::get<0>(current);
        int y = std::get<1>(current);
        int mask = std::get<2>(current);

        // ��ȡ������������
        std::vector<Coordinate> neighbors = getNeighbors(Coordinate(x, y));
        for (const Coordinate& neighbor : neighbors) {
            int nx = neighbor.x;
            int ny = neighbor.y;
            int newMask = mask;
            int scoreDelta = 0;

            // ������Դ�ռ�
            if (resourceMap.find(neighbor) != resourceMap.end()) {
                int resIndex = resourceMap[neighbor];
                if (!(mask & (1 << resIndex))) {
                    newMask |= (1 << resIndex);
                    scoreDelta += resources[resIndex].value; // ��Դֻ���ռ�һ��
                }
            }

            // �������壨������Զ�δ�����
            if (maze[nx][ny] == TRAP) {
                scoreDelta += -3; // ����ÿ�δ�����3��
            }

            // ����DP����
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

    // Ѱ���յ�������Դֵ
    int maxScore = INT_MIN;
    int bestMask = 0;
    for (int mask = 0; mask < totalMasks; ++mask) {
        if (dp[dest.x][dest.y][mask] > maxScore) {
            maxScore = dp[dest.x][dest.y][mask];
            bestMask = mask;
        }
    }

    // ����·��
    std::vector<Coordinate> path;
    int x = dest.x;
    int y = dest.y;
    int mask = bestMask;
    while (x != -1 && y != -1) {
        path.push_back(Coordinate(x, y));
        std::pair<int, int> prev = parent[x][y][mask];
        int prevX = prev.first;
        int prevY = prev.second;

        // ������Դ״̬������ʱ������ռ�����Դλ��
        if (resourceMap.find(Coordinate(x, y)) != resourceMap.end()) {
            int resIndex = resourceMap[Coordinate(x, y)];
            mask &= ~(1 << resIndex);
        }

        x = prevX;
        y = prevY;
    }

    // ��ת·��������㵽�յ㣩
    std::reverse(path.begin(), path.end());

    PathInfo pathInfo;
    pathInfo.path = path;
    pathInfo.score = maxScore;
    return pathInfo;
}