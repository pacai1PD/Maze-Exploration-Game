#include <iostream>
#include <fstream>
#include "MazeSolver.h"
#include "json.hpp"
#include "PathPrinter.h"
#include "Maze_Generator.h"

using json = nlohmann::json;

int main() {
    std::cout << "��ѡ���ͼ��Դ:" << std::endl;
    std::cout << "1. ʹ�����ɵĵ�ͼ" << std::endl;
    std::cout << "2. �� JSON �ļ��ж�ȡ��ͼ" << std::endl;

    int choice;
    std::cin >> choice;
    std::cin.ignore(); // ���Ի��з�

    if (choice == 1) {
        fun_DP(true);
    }
    else if (choice == 2) {
        std::string input_filename;
        std::cout << "������ JSON �ļ�����: ";
        std::getline(std::cin, input_filename);
        fun_DP(false, input_filename);
    }
    else {
        std::cout << "��Ч��ѡ��" << std::endl;
    }

    return 0;
}