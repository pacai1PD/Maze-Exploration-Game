#include "Maze_generation.h"




//初始化
MazeGenerator::MazeGenerator(int rows, int cols) 
    : m_rows(rows), m_cols(cols) {
    maze.resize(2 * m_rows + 1, vector<char>(2 * m_cols + 1, WALL));
}

// 确保迷宫连通性
void MazeGenerator::ensureConnectivity() {

    vector<vector<int>> regions(2 * m_rows + 1, vector<int>(2 * m_cols + 1, -1));
    int regionCount = 0;
    
    for (int r = 1; r < 2 * m_rows; r += 2) {
        for (int c = 1; c < 2 * m_cols; c += 2) {
            if (maze[r][c] != WALL && regions[r][c] == -1) {
                floodFill(r, c, regionCount, regions);
                regionCount++;
            }
        }
    }

    if (regionCount <= 1) return;

    connectRegions(regions, regionCount);
}

// 洪水填充算法标记区域
void MazeGenerator::floodFill(int r, int c, int regionId, vector<vector<int>>& regions) {
    static const vector<pair<int, int>> dirs = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    
    queue<pair<int, int>> q;
    q.push({r, c});
    regions[r][c] = regionId;

    while (!q.empty()) {
        auto front = q.front();
        int row = front.first;
        int col = front.second;
        q.pop();

        for (const auto& dir : dirs) {
            int dr = dir.first;
            int dc = dir.second;
            int nr = row + dr;
            int nc = col + dc;

            // 检查是否在迷宫范围内
            if (nr < 0 || nr >= 2 * m_rows + 1 || nc < 0 || nc >= 2 * m_cols + 1)
                continue;

            // 只允许从房间走到房间，中间不能有墙
            if (maze[nr][nc] == WALL || regions[nr][nc] != -1)
                continue;

            // 检查是否有墙阻挡
            int wall_r = (row + nr) / 2;
            int wall_c = (col + nc) / 2;
            if (maze[wall_r][wall_c] == WALL)
                continue;

            regions[nr][nc] = regionId;
            q.push({nr, nc});
        }
    }
}

// 连接不同区域
void MazeGenerator::connectRegions(vector<vector<int>>& regions, int regionCount) {
    // 收集不同区域的边界点
    vector<vector<pair<int, int>>> regionBoundaries(regionCount);
    
    // 遍历所有可能的墙壁位置
    for (int r = 1; r < 2*m_rows; r++) {
        for (int c = 1; c < 2*m_cols; c++) {

            if (r % 2 == 1 && c % 2 == 1) continue;
            if (maze[r][c] != WALL) continue;

            unordered_set<int> adjacentRegions;
            
            if (r > 0 && regions[r-1][c] != -1) adjacentRegions.insert(regions[r-1][c]);
            if (r < 2*m_rows && regions[r+1][c] != -1) adjacentRegions.insert(regions[r+1][c]);
            if (c > 0 && regions[r][c-1] != -1) adjacentRegions.insert(regions[r][c-1]);
            if (c < 2*m_cols && regions[r][c+1] != -1) adjacentRegions.insert(regions[r][c+1]);
            
            if (adjacentRegions.size() >= 2) {
                for (int region : adjacentRegions) {
                    regionBoundaries[region].push_back({r, c});
                }
            }
        }
    }
    
    // 随机选择连接点
    for (int i = 1; i < regionCount; i++) {
        if (!regionBoundaries[i].empty()) {
            uniform_int_distribution<int> dist(0, regionBoundaries[i].size()-1);
            auto rc = regionBoundaries[i][dist(m_rng)];
            maze[rc.first][rc.second] = PATH; // 打通墙壁
        }
    }
}

// // 将迷宫转换为JSON格式
// json MazeGenerator::toJson() const {
//     json j;
//     j["rows"] = 2 * m_rows + 1;
//     j["cols"] = 2 * m_cols + 1;
    
//     json mazeArray = json::array();
//     for (const auto& row : maze) {
//         string rowStr;
//         for (char c : row) {
//             rowStr += c;
//         }
//         mazeArray.push_back(rowStr);
//     }
//     j["maze"] = mazeArray;
    
//     return j;
// }

// // 保存迷宫到文件
// void MazeGenerator::saveToFile(const string& filename) const {
//     json j = toJson();
//     ofstream file(filename);
//     file.close();
// }

// 打印迷宫
void MazeGenerator::print() const {
    for (const auto& row : maze) {
        for (char c : row) {
            cout << c;
        }
        cout << endl;
    }
}

void MazeGenerator::initMaze() {
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            maze[2 * i + 1][2 * j + 1] = PATH;
        }
    }

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols - 1; j++) {
            createPassage(i, j, i, j + 1);  // 水平连接
        }
    }
    
    for (int i = 0; i < m_rows - 1; i++) {
        for (int j = 0; j < m_cols; j++) {
            createPassage(i, j, i + 1, j);  // 垂直连接
        }
    }
}

// 创建通道
void MazeGenerator::createPassage(int r1, int c1, int r2, int c2) {
    int wall_r = 2 * r1 + 1 + (r2 - r1);
    int wall_c = 2 * c1 + 1 + (c2 - c1);
    maze[wall_r][wall_c] = PATH;
}
//建墙
void MazeGenerator::removePassage(int r1, int c1, int r2, int c2) {
    int wall_r = 2 * r1 + 1 + (r2 - r1);
    int wall_c = 2 * c1 + 1 + (c2 - c1);
    maze[wall_r][wall_c] = WALL;
}

// 递归分割法
void MazeGenerator::divide(int left, int top, int right, int bottom) {

    if ((right - left < 1) || (bottom - top < 1)) {
        return;
    }
    
    uniform_int_distribution<int> dist_x(left, right - 1);
    uniform_int_distribution<int> dist_y(top, bottom - 1);
    int x = dist_x(m_rng);
    int y = dist_y(m_rng);
    
    for (int i = top; i <= bottom; i++) {  // 垂直墙
        if (i != y) removePassage(i, x, i, x + 1);
    }
    
    for (int i = left; i <= right; i++) {  // 水平墙
        if (i != x) removePassage(y, i, y + 1, i);
    }

    // 留一面墙
    int wallToKeep = m_rng() % 4;

    if (wallToKeep != 0 && y > top) { 
        uniform_int_distribution<int> gap_dist(top, y - 1);
        int gap = gap_dist(m_rng);
        createPassage(gap, x, gap, x + 1);
    }
    if (wallToKeep != 1 && y < bottom - 1) { 
        uniform_int_distribution<int> gap_dist(y + 1, bottom);
        int gap = gap_dist(m_rng);
        createPassage(gap, x, gap, x + 1);
    }
    if (wallToKeep != 2 && x > left) { 
        uniform_int_distribution<int> gap_dist(left, x - 1);
        int gap = gap_dist(m_rng);
        createPassage(y, gap, y + 1, gap);
    }
    if (wallToKeep != 3 && x < right - 1) {
        uniform_int_distribution<int> gap_dist(x + 1, right);
        int gap = gap_dist(m_rng);
        createPassage(y, gap, y + 1, gap);
    }

    divide(left, top, x, y);
    divide(x + 1, top, right, y);
    divide(left, y + 1, x, bottom); 
    divide(x + 1, y + 1, right, bottom);
}

void MazeGenerator::placeGameElements() {
    vector<pair<int, int>> availableRooms;
    for (int r = 0; r < m_rows; r++) {
        for (int c = 0; c < m_cols; c++) {
            if (maze[2 * r + 1][2 * c + 1] == PATH) {
                availableRooms.push_back(make_pair(r, c));
            }
        }
    }
    shuffle(availableRooms.begin(), availableRooms.end(), m_rng);

    // 防止房间数量不足
    if (availableRooms.size() < 3) return;

    int start_r = availableRooms[0].first, start_c = availableRooms[0].second;
    maze[2 * start_r + 1][2 * start_c + 1] = START;

    int exit_r = availableRooms[1].first, exit_c = availableRooms[1].second;
    maze[2 * exit_r + 1][2 * exit_c + 1] = EXIT;

    int boss_r = availableRooms[2].first, boss_c = availableRooms[2].second;
    maze[2 * boss_r + 1][2 * boss_c + 1] = BOSS;

    int index = 3;
    int resources = min(5, (int)availableRooms.size() - index);
    for (int i = 0; i < resources; i++) {
        if (index >= (int)availableRooms.size()) break;
        int r = availableRooms[index].first, c = availableRooms[index].second;
        maze[2 * r + 1][2 * c + 1] = RESOURCE;
        index++;
    }

    int traps = min(3, (int)availableRooms.size() - index);
    for (int i = 0; i < traps; i++) {
        if (index >= (int)availableRooms.size()) break;
        int r = availableRooms[index].first, c = availableRooms[index].second;
        maze[2 * r + 1][2 * c + 1] = TRAP;
        index++;
    }

    int mechanisms = min(3, (int)availableRooms.size() - index);
    for (int i = 0; i < mechanisms; i++) {
        if (index >= (int)availableRooms.size()) break;
        int r = availableRooms[index].first, c = availableRooms[index].second;
        maze[2 * r + 1][2 * c + 1] = MECHANISM;
        index++;
    }
}
void MazeGenerator::generate() {
    initMaze();
    divide(0, 0, m_cols - 1, m_rows - 1);
    ensureConnectivity();
    placeGameElements();
}

void generateMaze() {
    cout << "请输入迷宫尺寸" << endl;
    
    int rows, cols;
    string input;
    
    do {
        cout << "行数(必须≥7且为整数):";
        getline(cin, input);
        while(!(stringstream(input) >> rows) || rows < 7) {
            cout << "输入无效，请重新输入行数(必须≥7且为整数):";
            getline(cin, input);
        }
    } while (rows < 7);
    
    do {
        cout << "列数(必须≥7且为整数):";
        getline(cin, input);
        while(!(stringstream(input) >> cols) || cols < 7) {
            cout << "输入无效，请重新输入列数(必须≥7且为整数):";
            getline(cin, input);
        }
    } while (cols < 7);
    // 目前仅支持奇数
    MazeGenerator generator((rows - 1)/2, (cols - 1)/2);
    generator.generate();
    generator.print();
    // generator.saveToFile("maze.json");
    
}
int main() {
    generateMaze();
    return 0;
}