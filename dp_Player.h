#pragma once
#include "Maze_Read.h"
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

// ·����Ϣ�ṹ��
/*struct PathInfo {
    std::vector<Coordinate> path; // ·����������
    int score;                    // ����Դֵ
    PathInfo() : score(-INT_MAX) {}
};*/

class MazeDP {
private:
    const std::vector<std::vector<int>>& maze;          // �Թ�����
    Coordinate start, dest;                            // �����յ�
    std::vector<PositionInfo> resources;               // ��Դ��Ϣ
    std::vector<PositionInfo> traps;                   // ������Ϣ
    std::unordered_map<Coordinate, int> resourceMap;   // ��Դλ�õ�������ӳ��

    // �������Ϸ���
    bool isValid(const Coordinate& coord) const;

    // ��ȡ���ڿ�������
    std::vector<Coordinate> getNeighbors(const Coordinate& coord) const;

public:
    // ���캯��
    MazeDP(const Maze_Read& mazeRead);

    // ��̬�滮Ѱ������·��
    PathInfo findOptimalPath();
};