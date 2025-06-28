#pragma once
#include <vector>
#include <random>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <queue>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    json toJson() const;
    void saveToFile(const std::string& filename) const;
    void print() const;

private:
    void initMaze();
    void createPassage(int r1, int c1, int r2, int c2);
    void removePassage(int r1, int c1, int r2, int c2);
    void divide(int left, int top, int right, int bottom);
    void placeGameElements();
    void floodFill(int r, int c, int regionId, std::vector<std::vector<int>>& regions);
    void connectRegions(std::vector<std::vector<int>>& regions, int regionCount);
};

void generateMaze();