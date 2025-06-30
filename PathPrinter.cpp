// PathPrinter.cpp
#include "PathPrinter.h"
#include "Maze_Generator.h"
#include <iostream>
#include <limits>
using namespace std;

// ��̬��ӡ·���ĺ���
void printPathDynamically(std::vector<std::vector<std::string>>& maze, const std::vector<std::vector<int>>& optimal_path, int max_res) {
    std::vector<std::vector<std::string>> printMaze = maze; // ����һ���Թ������޸�
    std::vector<std::vector<std::string>> tempMaze = maze; // ����һ���Թ������޸���Դ״̬
    std::unordered_map<std::string, int> resource_values = { {"G", 5}, {"T", -3}, {"S", 0}, {"E", 0}, {" ", 0}, {"L", 0}, {"B", 0} };
    int current_res = 0; // ��ǰ��Դֵ����ʼ��Ϊ 0

    // �ҵ�������Դֵ
    int start_row = optimal_path[0][0];
    int start_col = optimal_path[0][1];
    current_res = resource_values[tempMaze[start_row][start_col]];

    // ��̬��ʾ·��
    for (size_t i = 0; i < optimal_path.size(); ++i) {
        int row = optimal_path[i][0];
        int col = optimal_path[i][1];

        printMaze[row][col] = "*";

        // ��ӡ�Թ�
        for (const auto& rowVec : printMaze) {
            for (const auto& cell : rowVec) {
                std::cout << cell;
            }
            std::cout << std::endl;
        }

        // ��ӡ��ǰ�����͵�ǰ����Դֵ
        std::cout << "��ǰ����: " << i + 1 << ", ��ǰ��Դֵ: " << current_res << std::endl;

        // �ȴ��û�����س�
        std::cout << "���س�������..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // �Ƴ��Ѿ��߹�����Դ�㡢BOSS �㡢���ص�������
        if (tempMaze[row][col] == "G" || tempMaze[row][col] == "B" ||
            tempMaze[row][col] == "L" || tempMaze[row][col] == "T") {
            current_res += resource_values[tempMaze[row][col]]; // ���µ�ǰ��Դֵ
            tempMaze[row][col] = " ";
            printMaze[row][col] = " ";
        }

        // �ָ�֮ǰ��·����ʾ
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

    // ��ӡ����·��
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

// ��ӡ�Թ��ĺ���
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
        std::cout << "�������Թ��ĳߴ�" << endl;

        int rows, cols;
        string input;

        do {
            cout << "����(��7Ϊ��С):";
            getline(cin, input);
            while (!(stringstream(input) >> rows) || rows < 7) {
                cout << "������Ч����������������(��7Ϊ��С):";
                getline(cin, input);
            }
        } while (rows < 7);

        do {
            cout << "����(��7Ϊ��С):";
            getline(cin, input);
            while (!(stringstream(input) >> cols) || cols < 7) {
                cout << "������Ч����������������(��7Ϊ��С):";
                getline(cin, input);
            }
        } while (cols < 7);

        MazeGenerator generator((rows - 1) / 2, (cols - 1) / 2);
        generator.generate();
        generator.saveToFile("generated_maze.json"); // ���浽�ļ�
        maze_grid = generator.getMaze();
    }
    else {
        try {
            std::ifstream input_file(input_filename);
            if (!input_file.is_open()) {
                std::cerr << "����: �����ļ� '" << input_filename << "' δ�ҵ���" << std::endl;
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
                throw std::invalid_argument("���� JSON �ļ���δ�ҵ� 'maze' �ֶ�");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "����: ��ȡ�ļ�ʱ����: " << e.what() << std::endl;
            return 1;
        }
    }

    // ����Ϸ��ʼǰ��ӡ�Թ�����
    std::cout << "��ʼ�Թ�����:" << std::endl;
    printMaze(maze_grid);

    try {
        MazeSolver solver(maze_grid);
        std::tuple<int, std::vector<std::vector<int>>, std::vector<std::vector<int>>> result = solver.solve();
        int max_res = std::get<0>(result);
        std::vector<std::vector<int>> opt_path = std::get<1>(result);
        std::vector<std::vector<int>> crit_path = std::get<2>(result);

        std::cout << "\n--- ��� ---" << std::endl;
        std::cout << "�����Դֵ: " << max_res << std::endl;
        std::cout << "�ؼ�·�� (S->E ֮��֮���·��) ����: " << crit_path.size() << " ��" << std::endl;
        std::cout << "����·������: " << opt_path.size() << " ��" << std::endl;

        json output_json;
        output_json["maze"] = maze_grid;
        output_json["max_resource"] = max_res;
        output_json["optimal_path"] = opt_path;

        std::ofstream output_file(output_filename);
        output_file << output_json.dump(2);

        std::cout << "\n����ѳɹ����浽 '" << output_filename << "'" << std::endl;
        std::cout << "�ҵ��Ĺؼ�·��Ϊ: ";
        for (std::vector<std::vector<int>>::iterator it = crit_path.begin(); it != crit_path.end(); ++it) {
            std::vector<int> coord = *it;
            std::cout << "[" << coord[0] << ", " << coord[1] << "] ";
        }
        std::cout << std::endl;

        // ��̬��ӡ·��
        printPathDynamically(maze_grid, opt_path, max_res);
    }
    catch (const std::exception& e) {
        std::cerr << "����: ����Թ�ʱ����: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}