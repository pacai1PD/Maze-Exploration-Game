#pragma once
#include "Maze_Read.h"
#include <vector>
#include <cmath>

// ·����Ϣ�ṹ��
/*struct PathInfo {
    std::vector<Coordinate> path;  // ·����������
    int score;        // ���ռ���Դ��ֵ
    PathInfo() : score(0) {}
};*/

// ̰���㷨����ࣨ��װ�棩
class GreedyPlayer {
private:
    const std::vector<std::vector<int>>& maze;       // �Թ���������
    Coordinate currentPos;                          // ��ǰλ��
    Coordinate start, dest;                         // �����յ�
    std::vector<PositionInfo> resources;            // ��Դ��Ϣ
    std::vector<Coordinate> collectedResourcePos;   // ���ռ���Դ��λ��

    // ��������Ƿ���3��3��Ұ��
    bool isInVision(const Coordinate& pos, const Coordinate& visionCenter) const;

    // ���������پ���
    int manhattanDistance(const Coordinate& a, const Coordinate& b) const;

    // �����Դ�Ƿ����ռ�
    bool isResourceCollected(const Coordinate& pos) const;

    // ��Ŀ��λ���ƶ�һ��
    Coordinate moveTowards(const Coordinate& target) const;

    // �������Ϸ���
    bool isValid(const Coordinate& coord) const;

public:
    // ���캯���������Թ���ȡ������
    explicit GreedyPlayer(const Maze_Read& mazeRead);

    // ̰���㷨����������ȡ������Դʰȡ·��
    PathInfo findGreedyPath();
};