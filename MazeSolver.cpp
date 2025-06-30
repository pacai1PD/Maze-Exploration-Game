#include "MazeSolver.h"
#include <queue>
#include <algorithm>

MazeSolver::MazeSolver(const std::vector<std::vector<std::string>>& maze) : maze(maze) {
    rows = maze.size();
    cols = maze[0].size();
}

std::tuple<int, std::vector<std::vector<int>>, std::vector<std::vector<int>>> MazeSolver::solve() {
    _build_graph();
    _build_tree();

    std::vector<Position> nodes_in_bfs_order;
    std::queue<Position> q;
    q.push(start_pos.value());
    std::unordered_set<Position, PositionHash> visited;
    visited.insert(start_pos.value());

    while (!q.empty()) {
        Position u = q.front();
        q.pop();
        nodes_in_bfs_order.push_back(u);

        std::vector<Position> childList = children[u];
        for (std::vector<Position>::iterator it = childList.begin(); it != childList.end(); ++it) {
            Position v = *it;
            if (visited.find(v) == visited.end()) {
                visited.insert(v);
                q.push(v);
            }
        }
    }

    std::reverse(nodes_in_bfs_order.begin(), nodes_in_bfs_order.end());
    for (std::vector<Position>::iterator it = nodes_in_bfs_order.begin(); it != nodes_in_bfs_order.end(); ++it) {
        Position node = *it;
        _dp_pass1_post_order(node);
    }

    int max_resource = _calculate_max_resource();
    std::vector<Position> critical_path_coords = _find_critical_path();
    std::vector<Position> optimal_path_coords = _reconstruct_optimal_path(critical_path_coords);

    std::vector<std::vector<int>> optimal_path_list;
    for (std::vector<Position>::iterator it = optimal_path_coords.begin(); it != optimal_path_coords.end(); ++it) {
        Position coord = *it;
        std::vector<int> coordVec;
        coordVec.push_back(coord.first);
        coordVec.push_back(coord.second);
        optimal_path_list.push_back(coordVec);
    }

    std::vector<std::vector<int>> critical_path_list;
    for (std::vector<Position>::iterator it = critical_path_coords.begin(); it != critical_path_coords.end(); ++it) {
        Position coord = *it;
        std::vector<int> coordVec;
        coordVec.push_back(coord.first);
        coordVec.push_back(coord.second);
        critical_path_list.push_back(coordVec);
    }

    return std::make_tuple(max_resource, optimal_path_list, critical_path_list);
}

void MazeSolver::_build_graph() {
    std::unordered_map<std::string, int> resource_values = { {"G", 5}, {"T", -3}, {"S", 0}, {"E", 0}, {" ", 0}, {"L", 0}, {"B", 0} };
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            std::string char_ = maze[r][c];
            if (char_ == "#") continue;

            Position pos = std::make_pair(r, c);
            resource_map[pos] = resource_values[char_];

            if (char_ == "S") start_pos = Optional<Position>(pos);
            else if (char_ == "E") end_pos = Optional<Position>(pos);

            std::vector<Position> directions = { std::make_pair(0, 1), std::make_pair(0, -1), std::make_pair(1, 0), std::make_pair(-1, 0) };
            for (std::vector<Position>::iterator it = directions.begin(); it != directions.end(); ++it) {
                Position dir = *it;
                int nr = r + dir.first;
                int nc = c + dir.second;
                if (0 <= nr && nr < rows && 0 <= nc && nc < cols && maze[nr][nc] != "#") {
                    adj[pos].push_back(std::make_pair(nr, nc));
                }
            }
        }
    }

    if (!start_pos.has_value() || !end_pos.has_value()) {
        throw std::invalid_argument("Î´ÕÒµ½ 'S' »ò 'E'");
    }
}

void MazeSolver::_build_tree() {
    std::queue<Position> q;
    q.push(start_pos.value());
    parent[start_pos.value()] = Optional<Position>();
    std::unordered_set<Position, PositionHash> visited;
    visited.insert(start_pos.value());

    while (!q.empty()) {
        Position u = q.front();
        q.pop();

        std::vector<Position> adjList = adj[u];
        for (std::vector<Position>::iterator it = adjList.begin(); it != adjList.end(); ++it) {
            Position v = *it;
            if (visited.find(v) == visited.end()) {
                visited.insert(v);
                parent[v] = Optional<Position>(u);
                children[u].push_back(v);
                q.push(v);
            }
        }
    }
}

void MazeSolver::_dp_pass1_post_order(const Position& u) {
    int gain = 0;
    std::vector<Position> childList = children[u];
    for (std::vector<Position>::iterator it = childList.begin(); it != childList.end(); ++it) {
        Position v_child = *it;
        int excursion_value = resource_map[v_child] + max_excursion_gain[v_child];
        if (excursion_value > 0) {
            gain += excursion_value;
        }
    }
    max_excursion_gain[u] = gain;
}

int MazeSolver::_calculate_max_resource() {
    std::unordered_map<Position, int, PositionHash> max_path_to;
    max_path_to[start_pos.value()] = resource_map[start_pos.value()] + max_excursion_gain[start_pos.value()];

    std::queue<Position> q;
    q.push(start_pos.value());
    std::unordered_set<Position, PositionHash> visited;
    visited.insert(start_pos.value());

    while (!q.empty()) {
        Position u = q.front();
        q.pop();

        std::vector<Position> childList = children[u];
        for (std::vector<Position>::iterator it = childList.begin(); it != childList.end(); ++it) {
            Position v_child = *it;
            if (visited.find(v_child) == visited.end()) {
                visited.insert(v_child);
                int v_excursion_value = resource_map[v_child] + max_excursion_gain[v_child];
                int value_at_u_without_v = max_path_to[u] - std::max(0, v_excursion_value);
                max_path_to[v_child] = value_at_u_without_v + resource_map[v_child] + max_excursion_gain[v_child];
                q.push(v_child);
            }
        }
    }

    return max_path_to[end_pos.value()];
}

std::vector<Position> MazeSolver::_find_critical_path() {
    std::vector<Position> path;
    Position curr = end_pos.value();
    while (true) {
        path.push_back(curr);
        Optional<Position> parentOpt = parent[curr];
        if (!parentOpt.has_value()) break;
        curr = parentOpt.value();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Position> MazeSolver::_reconstruct_optimal_path(const std::vector<Position>& critical_path) {
    std::vector<Position> optimal_path;
    for (std::vector<Position>::size_type i = 0; i < critical_path.size(); ++i) {
        Position u = critical_path[i];
        optimal_path.push_back(u);

        Optional<Position> p;
        if (i > 0) p = Optional<Position>(critical_path[i - 1]);
        Optional<Position> next_on_critical;
        if (i < critical_path.size() - 1) next_on_critical = Optional<Position>(critical_path[i + 1]);

        std::vector<Position> excursions = _get_excursions_from(u, p, next_on_critical);
        optimal_path.insert(optimal_path.end(), excursions.begin(), excursions.end());
    }
    return optimal_path;
}

std::vector<Position> MazeSolver::_get_excursions_from(const Position& u, const Optional<Position>& came_from, const Optional<Position>& going_to) {
    std::vector<Position> full_excursion_path;
    std::vector<Position> childList = children[u];
    for (std::vector<Position>::iterator it = childList.begin(); it != childList.end(); ++it) {
        Position v_child = *it;
        if ((!came_from.has_value() || v_child != came_from.value()) && (!going_to.has_value() || v_child != going_to.value())) {
            int excursion_value = resource_map[v_child] + max_excursion_gain[v_child];
            if (excursion_value > 0) {
                std::vector<Position> trip_path = _build_round_trip(v_child, u);
                full_excursion_path.insert(full_excursion_path.end(), trip_path.begin(), trip_path.end());
                full_excursion_path.push_back(u);
            }
        }
    }
    return full_excursion_path;
}

std::vector<Position> MazeSolver::_build_round_trip(const Position& u, const Position& came_from) {
    std::vector<Position> path_segment;
    path_segment.push_back(u);

    std::vector<Position> sub_excursions = _get_excursions_from(u, Optional<Position>(came_from), Optional<Position>());
    path_segment.insert(path_segment.end(), sub_excursions.begin(), sub_excursions.end());
    return path_segment;
}