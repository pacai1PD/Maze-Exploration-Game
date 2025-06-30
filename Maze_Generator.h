// Maze_Generator.h
#pragma once
#include <vector>
#include <random>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <string>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class MazeGenerator {
private:
    int m_rows;
    int m_cols;
    std::vector<std::vector<char>> maze;
    std::mt19937 m_rng;

    enum Element {
        WALL = '#',
        PATH = ' ',
        START = 'S',
        EXIT = 'E',
        RESOURCE = 'G',
        TRAP = 'T',
        MECHANISM = 'L',
        BOSS = 'B'
    };

public:
    MazeGenerator(int rows, int cols);
    void generate();
    void ensureConnectivity();
    void print() const;
    std::vector<std::vector<std::string>> getMaze() const; // 获取生成的迷宫
    void saveToFile(const std::string& filename) const; // 保存迷宫

    // 在关键路径上放置Boss和机关L
    void placeBossAndMechanismOnPath(const std::vector<std::vector<int>>& optimal_path);

    // 根据迷宫网格数据更新maze
    void setMaze(const std::vector<std::vector<std::string>>& maze_grid);

private:
    void initMaze();
    void createPassage(int r1, int c1, int r2, int c2);
    void removePassage(int r1, int c1, int r2, int c2);
    void divide(int left, int top, int right, int bottom);
    void placeGameElements();
    void floodFill(int r, int c, int regionId, std::vector<std::vector<int>>& regions);
    void connectRegions(std::vector<std::vector<int>>& regions, int regionCount);
    void generateAcyclicConnectedMaze(); // 新增方法
    void dfs(int r, int c, std::vector<std::vector<bool>>& visited); // 深度优先搜索
};

void generateMaze();