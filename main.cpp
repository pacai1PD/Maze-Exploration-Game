#include "Maze_Read.h"
#include "dp_Player.h"
#include "GreedyPlayer.h"

int main() {
	try {
		std::cout << "Maze Reader Initialized." << std::endl;
		Maze_Read mazeReader("maze.json");
		cout << "请选择程序方式：1. dp, 2. 贪心" << endl;
		char choice;
		std::cin >> choice;
		if (choice == '1') {
			MazeDP dpPlayer(mazeReader);
			PathInfo optimalPath = dpPlayer.findOptimalPath();
			std::cout << "DP Path found with score: " << optimalPath.score << std::endl;
			for (const Coordinate& coord : optimalPath.path) {
				std::cout << "(" << coord.x << ", " << coord.y << ") ";
			}
			std::cout << std::endl;
		}
		else if (choice == '2') {
			GreedyPlayer greedyPlayer(mazeReader);
			PathInfo greedyPath = greedyPlayer.findGreedyPath();
			std::cout << "Greedy Path found with collected resources: " << greedyPath.score << std::endl;
			for (const Coordinate& coord : greedyPath.path) {
				std::cout << "(" << coord.x << ", " << coord.y << ") ";
			}
			std::cout << std::endl;
		}
		else {
			std::cerr << "Invalid choice!" << std::endl;
		}
	}
	catch(const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}