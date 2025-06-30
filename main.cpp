#include <iostream>
#include <fstream>
#include "MazeSolver.h"
#include "json.hpp"
#include "PathPrinter.h"
#include "Maze_Generator.h"

using json = nlohmann::json;

int main() {
    std::cout << "请选择地图来源:" << std::endl;
    std::cout << "1. 使用生成的地图" << std::endl;
    std::cout << "2. 从 JSON 文件中读取地图" << std::endl;

    int choice;
    std::cin >> choice;
    std::cin.ignore(); // 忽略换行符

    if (choice == 1) {
        fun_DP(true);
    }
    else if (choice == 2) {
        std::string input_filename;
        std::cout << "请输入 JSON 文件名称: ";
        std::getline(std::cin, input_filename);
        fun_DP(false, input_filename);
    }
    else {
        std::cout << "无效的选择。" << std::endl;
    }

    return 0;
}