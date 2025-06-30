// PathPrinter.cpp
#include "PathPrinter.h"
#include "Maze_Generator.h"
#include <iostream>
#include <limits>
using namespace std;

// 动态打印路径的函数
void printPathDynamically(std::vector<std::vector<std::string>>& maze, const std::vector<std::vector<int>>& optimal_path, int max_res) {
    std::vector<std::vector<std::string>> printMaze = maze; // 复制一份迷宫用于修改
    std::vector<std::vector<std::string>> tempMaze = maze; // 复制一份迷宫用于修改资源状态
    std::unordered_map<std::string, int> resource_values = { {"G", 5}, {"T", -3}, {"S", 0}, {"E", 0}, {" ", 0}, {"L", 0}, {"B", 0} };
    int current_res = 0; // 当前资源值，初始化为 0

    // 找到起点的资源值
    int start_row = optimal_path[0][0];
    int start_col = optimal_path[0][1];
    current_res = resource_values[tempMaze[start_row][start_col]];

    // 动态显示路径
    for (size_t i = 0; i < optimal_path.size(); ++i) {
        int row = optimal_path[i][0];
        int col = optimal_path[i][1];

        printMaze[row][col] = "*";

        // 打印迷宫
        for (const auto& rowVec : printMaze) {
            for (const auto& cell : rowVec) {
                std::cout << cell;
            }
            std::cout << std::endl;
        }

        // 打印当前步数和当前的资源值
        std::cout << "当前步数: " << i + 1 << ", 当前资源值: " << current_res << std::endl;

        // 等待用户输入回车
        std::cout << "按回车键继续..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // 移除已经走过的资源点、BOSS 点、机关点和陷阱点
        if (tempMaze[row][col] == "G" || tempMaze[row][col] == "B" ||
            tempMaze[row][col] == "L" || tempMaze[row][col] == "T") {
            current_res += resource_values[tempMaze[row][col]]; // 更新当前资源值
            tempMaze[row][col] = " ";
            printMaze[row][col] = " ";
        }

        // 恢复之前的路径显示
        if (i > 0) {
            int prevRow = optimal_path[i - 1][0];
            int prevCol = optimal_path[i - 1][1];
            if (tempMaze[prevRow][prevCol] == "S") {
                printMaze[prevRow][prevCol] = "S";
            }
            else if (tempMaze[prevRow][prevCol] == "E") {
                printMaze[prevRow][prevCol] = "E";
            }
            else {
                printMaze[prevRow][prevCol] = " ";
            }
        }
    }

    // 打印最优路径
    std::cout << "optimal_path: [" << std::endl;
    for (size_t i = 0; i < optimal_path.size(); ++i) {
        std::cout << "    [" << optimal_path[i][0] << "," << optimal_path[i][1] << "]";
        if (i < optimal_path.size() - 1) {
            std::cout << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;
}

// 打印迷宫的函数
void printMaze(const std::vector<std::vector<std::string>>& maze) {
    for (const auto& row : maze) {
        for (const auto& cell : row) {
            std::cout << cell;
        }
        std::cout << std::endl;
    }
}

int fun_DP(bool useGeneratedMaze, const std::string& input_filename, const std::string& output_filename) {
    std::vector<std::vector<std::string>> maze_grid;

    if (useGeneratedMaze) {
        std::cout << "请输入迷宫的尺寸" << endl;

        int rows, cols;
        string input;

        do {
            cout << "行数(以7为最小):";
            getline(cin, input);
            while (!(stringstream(input) >> rows) || rows < 7) {
                cout << "输入无效，请重新输入行数(以7为最小):";
                getline(cin, input);
            }
        } while (rows < 7);

        do {
            cout << "列数(以7为最小):";
            getline(cin, input);
            while (!(stringstream(input) >> cols) || cols < 7) {
                cout << "输入无效，请重新输入列数(以7为最小):";
                getline(cin, input);
            }
        } while (cols < 7);

        MazeGenerator generator((rows - 1) / 2, (cols - 1) / 2);
        generator.generate();
        generator.saveToFile("generated_maze.json"); // 保存到文件
        maze_grid = generator.getMaze();
    }
    else {
        try {
            std::ifstream input_file(input_filename);
            if (!input_file.is_open()) {
                std::cerr << "错误: 输入文件 '" << input_filename << "' 未找到。" << std::endl;
                return 1;
            }

            json data;
            input_file >> data;
            if (data.contains("maze")) {
                maze_grid = data["maze"].get<std::vector<std::vector<std::string>>>();
            }
            else if (data.is_array()) {
                maze_grid = data.get<std::vector<std::vector<std::string>>>();
            }
            else {
                throw std::invalid_argument("输入 JSON 文件中未找到 'maze' 字段");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "错误: 读取文件时出错: " << e.what() << std::endl;
            return 1;
        }
    }

    // 在游戏开始前打印迷宫布局
    std::cout << "初始迷宫布局:" << std::endl;
    printMaze(maze_grid);

    try {
        MazeSolver solver(maze_grid);
        std::tuple<int, std::vector<std::vector<int>>, std::vector<std::vector<int>>> result = solver.solve();
        int max_res = std::get<0>(result);
        std::vector<std::vector<int>> opt_path = std::get<1>(result);
        std::vector<std::vector<int>> crit_path = std::get<2>(result);

        std::cout << "\n--- 结果 ---" << std::endl;
        std::cout << "最大资源值: " << max_res << std::endl;
        std::cout << "关键路径 (S->E 之间之最短路径) 长度: " << crit_path.size() << " 步" << std::endl;
        std::cout << "最优路径长度: " << opt_path.size() << " 步" << std::endl;

        json output_json;
        output_json["maze"] = maze_grid;
        output_json["max_resource"] = max_res;
        output_json["optimal_path"] = opt_path;

        std::ofstream output_file(output_filename);
        output_file << output_json.dump(2);

        std::cout << "\n结果已成功保存到 '" << output_filename << "'" << std::endl;
        std::cout << "找到的关键路径为: ";
        for (std::vector<std::vector<int>>::iterator it = crit_path.begin(); it != crit_path.end(); ++it) {
            std::vector<int> coord = *it;
            std::cout << "[" << coord[0] << ", " << coord[1] << "] ";
        }
        std::cout << std::endl;

        // 动态打印路径
        printPathDynamically(maze_grid, opt_path, max_res);
    }
    catch (const std::exception& e) {
        std::cerr << "错误: 解决迷宫时出错: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}