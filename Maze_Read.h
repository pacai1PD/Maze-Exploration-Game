#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include "json.hpp"
#include <functional>

#define minus_limit -10000
using namespace std;

enum ElementType {
    PATH = 0,                 // ͨ·(�ո�)
    START,                    // ���S
    EXIT,                     // �յ�E
    RESOURCE = 5,             // ��ԴG
    LOCKER,                   // ����L
    BOSS,                     // BOSS B
    TRAP = -3,                // ����T
    WALL = minus_limit,       // ǽ��#
};

struct Coordinate {
    size_t x, y;
    Coordinate(size_t x = 0, size_t y = 0) : x(x), y(y) {}
    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Coordinate& other) const {
        return !(*this == other);
    }
};

    template <>
    struct hash<Coordinate> {
        std::size_t operator()(const Coordinate& coord) const {
            // ʹ�� std::hash ��� x �� y �Ĺ�ϣֵ
            auto h1 = std::hash<size_t>{}(coord.x);
            auto h2 = std::hash<size_t>{}(coord.y);
            return h1 ^ (h2 << 1);
        }
    };

struct PositionInfo {
    Coordinate position;
    int value;
    PositionInfo(Coordinate pos, int _value)
        : position(pos), value(_value) {
    }
    double distance2dest(Coordinate dest) {
        return sqrt(pow(position.x - dest.x, 2) + pow(position.y - dest.y, 2));
    }
};

struct PathInfo {
    std::vector<Coordinate> path; // ·����������
    int score;                    // ����Դֵ
    PathInfo() : score(-INT_MAX) {}
};

struct PlayerSkill {
    int damage;
    int coolDown;
};

class Maze_Read {
protected:
    vector<vector<int>> maze;
    Coordinate start, dest;
    vector<PositionInfo> resource;
    vector<PositionInfo> trap;
    vector<vector<int>> passwordClues;  // ��������
    vector<int> bossHP;                 // BOSSѪ��
    vector<PlayerSkill> playerSkills;   // ��Ҽ���
    string passwordHash;                // �����ϣֵ

public:
    Maze_Read(const std::string& filename);
    vector<vector<int>> getMaze() const;
    Coordinate getStart() const;
    Coordinate getDest() const;
    vector<PositionInfo> getResource() const { return resource; }
    vector<PositionInfo> getTrap() const { return trap; }
    vector<vector<int>> getPasswordClues() const;
    vector<int> getBossHP() const;
    vector<PlayerSkill> getPlayerSkills() const;
    string getPasswordHash() const;
};