// PathPrinter.h
#ifndef PATHPRINTER_H
#define PATHPRINTER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "MazeSolver.h"
#include "json.hpp"

// ��̬��ӡ·���ĺ���
void printPathDynamically(const std::vector<std::vector<std::string>>& maze,
    const std::vector<std::vector<int>>& path,
    int max_res);

// ��ӡ�Թ��ĺ���
void printMaze(const std::vector<std::vector<std::string>>& maze);

// �޸ĺ�ĺ����������붨�屣��һ��
int fun_DP(bool useGeneratedMaze = false, const std::string& input_filename = "result_maze_15_15_0_formatted.json", const std::string& output_filename = "result_maze_15_15_1_solution.json");

#endif