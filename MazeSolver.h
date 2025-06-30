#ifndef MAZESOLVER_H
#define MAZESOLVER_H

#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using Position = std::pair<int, int>;

// 自定义 Position 的哈希函数
struct PositionHash {
    std::size_t operator()(const Position& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

template<typename T>
class Optional {
public:
    Optional() : has_value_(false) {}
    Optional(const T& value) : has_value_(true), value_(value) {}

    bool has_value() const { return has_value_; }
    T& value() { return value_; }
    const T& value() const { return value_; }

private:
    bool has_value_;
    T value_;
};

class MazeSolver {
public:
    MazeSolver(const std::vector<std::vector<std::string>>& maze);
    std::tuple<int, std::vector<std::vector<int>>, std::vector<std::vector<int>>> solve();

private:
    std::vector<std::vector<std::string>> maze;
    int rows;
    int cols;
    std::unordered_map<Position, int, PositionHash> resource_map;
    Optional<Position> start_pos;
    Optional<Position> end_pos;

    std::unordered_map<Position, std::vector<Position>, PositionHash> adj;
    std::unordered_map<Position, Optional<Position>, PositionHash> parent;
    std::unordered_map<Position, std::vector<Position>, PositionHash> children;

    std::unordered_map<Position, int, PositionHash> max_excursion_gain;

    void _build_graph();
    void _build_tree();
    void _dp_pass1_post_order(const Position& u);
    int _calculate_max_resource();
    std::vector<Position> _find_critical_path();
    std::vector<Position> _reconstruct_optimal_path(const std::vector<Position>& critical_path);
    std::vector<Position> _get_excursions_from(const Position& u, const Optional<Position>& came_from, const Optional<Position>& going_to);
    std::vector<Position> _build_round_trip(const Position& u, const Position& came_from);
};

#endif