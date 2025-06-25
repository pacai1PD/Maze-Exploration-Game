#include "Maze_Read.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include <stdexcept>
#include <unordered_map>

using json = nlohmann::json;

Maze_Read::Maze_Read(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    json j;
    try {
        file >> j;
    }
    catch (const json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }

    // 读取迷宫矩阵
    if (j.find("maze") == j.end() || !j["maze"].is_array()) {
        throw std::runtime_error("Invalid maze format in JSON");
    }

    const json& mazeJson = j["maze"];
    maze.resize(mazeJson.size());

    for (size_t i = 0; i < mazeJson.size(); ++i) {
        if (!mazeJson[i].is_array()) {
            throw std::runtime_error("Invalid maze row format in JSON");
        }

        const json& rowJson = mazeJson[i];
        maze[i].resize(rowJson.size());

        for (size_t j = 0; j < rowJson.size(); ++j) {
            if (!rowJson[j].is_string() || rowJson[j].get<std::string>().length() != 1) {
                throw std::runtime_error("Invalid maze cell format in JSON");
            }

            char ch = rowJson[j].get<std::string>().front();
            switch (ch) {
            case '#': maze[i][j] = WALL; break;
            case ' ': maze[i][j] = PATH; break;
            case 'S':
                maze[i][j] = START;
                start = Coordinate(i, j);
                break;
            case 'E':
                maze[i][j] = EXIT;
                dest = Coordinate(i, j);
                break;
            case 'L': maze[i][j] = LOCKER; break;
            case 'B': maze[i][j] = BOSS; break;
            case 'G': {
                maze[i][j] = RESOURCE;
                resource.emplace_back(Coordinate(i, j), 5);
                break;
            }
            case 'T': {
                maze[i][j] = TRAP;
                trap.emplace_back(Coordinate(i, j), -3);
                break;
            }
            default:
                throw std::runtime_error("Unknown character in maze: " + std::string(1, ch));
            }
        }
    }

    // 读取密码线索
    if (j.find("C") != j.end() && j["C"].is_array()) {
        const json& cluesJson = j["C"];
        for (const auto& clue : cluesJson) {
            if (!clue.is_array() || clue.size() < 1 || clue.size() > 3) {
                throw std::runtime_error("Invalid clue format in JSON");
            }

            std::vector<int> clueVec;
            for (const auto& num : clue) {
                clueVec.push_back(num.get<int>());
            }
            passwordClues.push_back(clueVec);
        }
    }

    // 读取BOSS信息
    if (j.find("B") != j.end() && j["B"].is_array()) {
        const json& bossesJson = j["B"];
        for (const auto& boss : bossesJson) {
            bossHP.push_back(boss.get<int>());
        }
    }

    // 读取玩家技能
    if (j.find("PlayerSkills") != j.end() && j["PlayerSkills"].is_array()) {
        const json& skillsJson = j["PlayerSkills"];
        for (const auto& skill : skillsJson) {
            if (!skill.is_array() || skill.size() != 2) {
                throw std::runtime_error("Invalid skill format in JSON");
            }

            PlayerSkill playerSkill;
            playerSkill.damage = skill[0].get<int>();
            playerSkill.coolDown = skill[1].get<int>();
            playerSkills.push_back(playerSkill);
        }
    }

    // 读取密码哈希值
    if (j.find("L") != j.end() && j["L"].is_string()) {
        passwordHash = j["L"].get<std::string>();
    }

    file.close();
}

// 新增的获取方法
std::vector<std::vector<int>> Maze_Read::getMaze() const {
    return maze;
}

Coordinate Maze_Read::getStart() const {
    return start;
}

Coordinate Maze_Read::getDest() const {
    return dest;
}

std::vector<std::vector<int>> Maze_Read::getPasswordClues() const {
    return passwordClues;
}

std::vector<int> Maze_Read::getBossHP() const {
    return bossHP;
}

std::vector<PlayerSkill> Maze_Read::getPlayerSkills() const {
    return playerSkills;
}

std::string Maze_Read::getPasswordHash() const {
    return passwordHash;
}