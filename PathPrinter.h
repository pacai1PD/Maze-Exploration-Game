// PathPrinter.h
#ifndef PATHPRINTER_H
#define PATHPRINTER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "MazeSolver.h"
#include "json.hpp"

// 动态打印路径的函数
void printPathDynamically(const std::vector<std::vector<std::string>>& maze,
    const std::vector<std::vector<int>>& path,
    int max_res);

// 打印迷宫的函数
void printMaze(const std::vector<std::vector<std::string>>& maze);

// 修改后的函数声明，与定义保持一致
int fun_DP(bool useGeneratedMaze = false, const std::string& input_filename = "result_maze_15_15_0_formatted.json", const std::string& output_filename = "result_maze_15_15_1_solution.json");

#endif