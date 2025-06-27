#pragma once
#include "Maze_Read.h"
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

class MazeDP {
private:
    const std::vector<std::vector<int>>& maze;          // �Թ�����
    Coordinate start, dest;                            // �����յ�
    std::vector<PositionInfo> resources;               // ��Դ��Ϣ
    std::vector<PositionInfo> traps;                   // ������Ϣ
    std::unordered_map<Coordinate, int> resourceMap;   // ��Դλ�õ�������ӳ��

    // �ж������Ƿ�Ϸ�
    bool isValid(const Coordinate& coord) const;

    // ��ȡ��������
    std::vector<Coordinate> getNeighbors(const Coordinate& coord) const;

    // ��������֮�������·��
    PathInfo findPathBetweenPoints(const Coordinate& startPoint, const Coordinate& endPoint);

public:
    // ���캯��
    MazeDP(const Maze_Read& mazeRead);

    // ��̬�滮Ѱ������·��
    PathInfo findOptimalPath();
};