#include "GreedyPlayer.h"
#include <algorithm>
#include <climits>

// ���캯��ʵ��
GreedyPlayer::GreedyPlayer(const Maze_Read& mazeRead) :
    maze(mazeRead.getMaze()),
    currentPos(mazeRead.getStart()),
    start(mazeRead.getStart()),
    dest(mazeRead.getDest()),
    resources(mazeRead.getResource()),
    collectedResourcePos() {
    // ��ʼ��ʱ��������·��
}

// ��������Ƿ���3��3��Ұ��
bool GreedyPlayer::isInVision(const Coordinate& pos, const Coordinate& visionCenter) const {
    return pos.x >= visionCenter.x - 1 && pos.x <= visionCenter.x + 1 &&
        pos.y >= visionCenter.y - 1 && pos.y <= visionCenter.y + 1;
}

// ���������پ���
int GreedyPlayer::manhattanDistance(const Coordinate& a, const Coordinate& b) const {
    return std::abs(static_cast<int>(a.x - b.x)) + std::abs(static_cast<int>(a.y - b.y));
}

// �����Դ�Ƿ����ռ�
bool GreedyPlayer::isResourceCollected(const Coordinate& pos) const {
    for (const Coordinate& collectedPos : collectedResourcePos) {
        if (pos.x == collectedPos.x && pos.y == collectedPos.y) {
            return true;
        }
    }
    return false;
}

// ��Ŀ��λ���ƶ�һ�������Ⱥ����ƶ���
Coordinate GreedyPlayer::moveTowards(const Coordinate& target) const {
    Coordinate nextPos = currentPos;

    // ˮƽ�ƶ����ȣ���/�ң�
    if (currentPos.x < target.x && isValid(Coordinate(currentPos.x + 1, currentPos.y))) {
        nextPos.x += 1;
    }
    else if (currentPos.x > target.x && isValid(Coordinate(currentPos.x - 1, currentPos.y))) {
        nextPos.x -= 1;
    }
    // ��ֱ�ƶ�����/�£�
    else if (currentPos.y < target.y && isValid(Coordinate(currentPos.x, currentPos.y + 1))) {
        nextPos.y += 1;
    }
    else if (currentPos.y > target.y && isValid(Coordinate(currentPos.x, currentPos.y - 1))) {
        nextPos.y -= 1;
    }

    return nextPos;
}

// �������Ϸ��ԣ���ǽ�������Թ���Χ�ڣ�
bool GreedyPlayer::isValid(const Coordinate& coord) const {
    return coord.x >= 0 && coord.x < maze.size() &&
        coord.y >= 0 && coord.y < maze[0].size() &&
        maze[coord.x][coord.y] != WALL;
}

// ̰���㷨������ʵ��
PathInfo GreedyPlayer::findGreedyPath() {
    std::vector<Coordinate> path;
    path.push_back(currentPos);  // ������·��

    while (currentPos.x != dest.x || currentPos.y != dest.y) {
        Coordinate bestResourcePos;
        double bestRatio = -1.0;  // �Լ۱ȳ�ʼ��Ϊ����

        // 1. Ѱ����Ұ���Լ۱���ߵ���Դ
        for (const PositionInfo& res : resources) {
            if (isResourceCollected(res.position)) continue;  // �������ռ���Դ

            if (isInVision(res.position, currentPos)) {  // ����Ƿ���3��3��Ұ��
                int distance = manhattanDistance(currentPos, res.position);
                if (distance == 0) continue;  // ����������

                double ratio = static_cast<double>(res.value) / distance;
                if (ratio > bestRatio) {
                    bestRatio = ratio;
                    bestResourcePos = res.position;
                }
            }
        }

        // 2. �����ƶ�����
        if (bestRatio > 0) {  // ���ڿ�ʰȡ��Դ
            Coordinate nextPos = moveTowards(bestResourcePos);
            currentPos = nextPos;

            // ����Ƿ񵽴���Դλ��
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
        else {  // ����Դ��ʰȡ��ֱ�����յ��ƶ�
            Coordinate nextPos = moveTowards(dest);
            if (currentPos == nextPos) break;  // �޷��ƶ�����ֹѭ��
            currentPos = nextPos;
        }

        path.push_back(currentPos);  // ��¼��ǰλ��

        // ����Ƿ񵽴��յ�
        if (currentPos == dest) break;
    }

    // �������ؽ��
    PathInfo pathInfo;
    pathInfo.path = path;
    pathInfo.score = collectedResourcePos.size() * 5;  // ����ÿ����ԴֵΪ5
    return pathInfo;
}