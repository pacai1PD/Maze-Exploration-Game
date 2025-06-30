#include "Maze_Generator.h"

MazeGenerator::MazeGenerator(int rows, int cols)
    : m_rows(rows), m_cols(cols) {
    maze.resize(2 * m_rows + 1, vector<char>(2 * m_cols + 1, WALL));
    m_rng.seed(std::time(nullptr));
}

// ȷ���Թ���ͨ
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

// ��ˮ����㷨�����ڱ����ͨ����
void MazeGenerator::floodFill(int r, int c, int regionId, vector<vector<int>>& regions) {
    static const vector<pair<int, int>> dirs = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    queue<pair<int, int>> q;
    q.push({ r, c });
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

            // ����Ƿ����Թ���Χ��
            if (nr < 0 || nr >= 2 * m_rows + 1 || nc < 0 || nc >= 2 * m_cols + 1)
                continue;

            // ֻ�����ǽ��δ��ǵĵ�Ԫ��
            if (maze[nr][nc] == WALL || regions[nr][nc] != -1)
                continue;

            // ����м��Ƿ���ǽ
            int wall_r = (row + nr) / 2;
            int wall_c = (col + nc) / 2;
            if (maze[wall_r][wall_c] == WALL)
                continue;

            regions[nr][nc] = regionId;
            q.push({ nr, nc });
        }
    }
}

// ���Ӳ�ͬ����
void MazeGenerator::connectRegions(vector<vector<int>>& regions, int regionCount) {
    // �洢��ͬ����ı߽�ǽ
    vector<vector<pair<int, int>>> regionBoundaries(regionCount);

    // �ҳ����п��ܵ�ǽλ��
    for (int r = 1; r < 2 * m_rows; r++) {
        for (int c = 1; c < 2 * m_cols; c++) {

            if (r % 2 == 1 && c % 2 == 1) continue;
            if (maze[r][c] != WALL) continue;

            unordered_set<int> adjacentRegions;

            if (r > 0 && regions[r - 1][c] != -1) adjacentRegions.insert(regions[r - 1][c]);
            if (r < 2 * m_rows && regions[r + 1][c] != -1) adjacentRegions.insert(regions[r + 1][c]);
            if (c > 0 && regions[r][c - 1] != -1) adjacentRegions.insert(regions[r][c - 1]);
            if (c < 2 * m_cols && regions[r][c + 1] != -1) adjacentRegions.insert(regions[r][c + 1]);

            if (adjacentRegions.size() >= 2) {
                for (int region : adjacentRegions) {
                    regionBoundaries[region].push_back({ r, c });
                }
            }
        }
    }

    // ���ѡ��ǽ��ͨ
    for (int i = 1; i < regionCount; i++) {
        if (!regionBoundaries[i].empty()) {
            uniform_int_distribution<int> dist(0, regionBoundaries[i].size() - 1);
            auto rc = regionBoundaries[i][dist(m_rng)];
            maze[rc.first][rc.second] = PATH; // ��ͨǽ
        }
    }
}

// ��ӡ�Թ�
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
            createPassage(i, j, i, j + 1);  // ˮƽͨ��
        }
    }

    for (int i = 0; i < m_rows - 1; i++) {
        for (int j = 0; j < m_cols; j++) {
            createPassage(i, j, i + 1, j);  // ��ֱͨ��
        }
    }
}

// ����ͨ��
void MazeGenerator::createPassage(int r1, int c1, int r2, int c2) {
    int wall_r = 2 * r1 + 1 + (r2 - r1);
    int wall_c = 2 * c1 + 1 + (c2 - c1);
    maze[wall_r][wall_c] = PATH;
}

// �Ƴ�ͨ���е�ǽ
void MazeGenerator::removePassage(int r1, int c1, int r2, int c2) {
    int wall_r = 2 * r1 + 1 + (r2 - r1);
    int wall_c = 2 * c1 + 1 + (c2 - c1);
    maze[wall_r][wall_c] = WALL;
}

// �ݹ�ָ��㷨
void MazeGenerator::divide(int left, int top, int right, int bottom) {

    if ((right - left < 1) || (bottom - top < 1)) {
        return;
    }

    uniform_int_distribution<int> dist_x(left, right - 1);
    uniform_int_distribution<int> dist_y(top, bottom - 1);
    int x = dist_x(m_rng);
    int y = dist_y(m_rng);

    for (int i = top; i <= bottom; i++) {  // ��ֱǽ
        if (i != y) removePassage(i, x, i, x + 1);
    }

    for (int i = left; i <= right; i++) {  // ˮƽǽ
        if (i != x) removePassage(y, i, y + 1, i);
    }

    // ����һ��ȱ��
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

    // ȷ����㡢�յ�͹ؼ�Ԫ�ص�λ��
    if (availableRooms.size() < 3) return;

    int start_r = availableRooms[0].first, start_c = availableRooms[0].second;
    maze[2 * start_r + 1][2 * start_c + 1] = START;

    int exit_r = availableRooms[1].first, exit_c = availableRooms[1].second;
    maze[2 * exit_r + 1][2 * exit_c + 1] = EXIT;

    int index = 2;
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

// �����޻�����ͨ���Թ�
void MazeGenerator::generateAcyclicConnectedMaze() {
    std::vector<std::vector<bool>> visited(2 * m_rows + 1, std::vector<bool>(2 * m_cols + 1, false));
    int startR = 2 * (m_rng() % m_rows) + 1;
    int startC = 2 * (m_rng() % m_cols) + 1;
    dfs(startR, startC, visited);
}

// �����������
void MazeGenerator::dfs(int r, int c, std::vector<std::vector<bool>>& visited) {
    visited[r][c] = true;
    std::vector<std::pair<int, int>> directions = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} };
    std::shuffle(directions.begin(), directions.end(), m_rng);

    for (const auto& dir : directions) {
        int nr = r + dir.first;
        int nc = c + dir.second;
        if (nr > 0 && nr < 2 * m_rows + 1 && nc > 0 && nc < 2 * m_cols + 1 && !visited[nr][nc]) {
            int wallR = r + dir.first / 2;
            int wallC = c + dir.second / 2;
            maze[wallR][wallC] = PATH;
            maze[nr][nc] = PATH;
            dfs(nr, nc, visited);
        }
    }
}

void MazeGenerator::generate() {
    generateAcyclicConnectedMaze(); // ʹ���µ����ɷ���
    placeGameElements();
}

// ��ȡ���ɵ��Թ�
std::vector<std::vector<std::string>> MazeGenerator::getMaze() const {
    std::vector<std::vector<std::string>> stringMaze;
    for (const auto& row : maze) {
        std::vector<std::string> stringRow;
        for (char c : row) {
            stringRow.push_back(std::string(1, c));
        }
        stringMaze.push_back(stringRow);
    }
    return stringMaze;
}

// �����Թ���Ϣ��JSON�ļ�
void MazeGenerator::saveToFile(const std::string& filename) const {
    json data;
    std::vector<std::vector<std::string>> stringMaze = getMaze();
    data["maze"] = stringMaze;

    std::ofstream output_file(filename);
    if (output_file.is_open()) {
        output_file << data.dump(2);
        output_file.close();
    }
    else {
        std::cerr << "Error: Unable to open file '" << filename << "' for writing." << std::endl;
    }
}

// �ڹؼ�·���Ϸ���Boss�ͻ���L
void MazeGenerator::placeBossAndMechanismOnPath(const std::vector<std::vector<int>>& optimal_path) {
    if (optimal_path.size() < 2) return;

    // ѡ��ؼ�·���ϵ�λ�÷���Boss�ͻ���L
    int boss_index = optimal_path.size() / 3;
    int mechanism_index = 2 * optimal_path.size() / 3;

    int boss_row = optimal_path[boss_index][0];
    int boss_col = optimal_path[boss_index][1];
    maze[boss_row][boss_col] = BOSS;

    int mechanism_row = optimal_path[mechanism_index][0];
    int mechanism_col = optimal_path[mechanism_index][1];
    maze[mechanism_row][mechanism_col] = MECHANISM;
}

// �����Թ��������ݸ���maze
void MazeGenerator::setMaze(const std::vector<std::vector<std::string>>& maze_grid) {
    maze.clear();
    for (const auto& row : maze_grid) {
        std::vector<char> charRow;
        for (const auto& cell : row) {
            if (!cell.empty()) {
                charRow.push_back(cell[0]);
            }
        }
        maze.push_back(charRow);
    }
}

void generateMaze() {
    cout << "�������Թ��ĳߴ�" << endl;

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
    // Ŀǰֻ֧������
    MazeGenerator generator((rows - 1) / 2, (cols - 1) / 2);
    generator.generate();
    generator.print();
    generator.saveToFile("generated_maze.json"); // ���浽�ļ�
}