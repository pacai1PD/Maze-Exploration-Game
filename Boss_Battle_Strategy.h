#include <vector>
#include <unordered_map>
#include <queue>
#include <climits>
#include <algorithm>
#include <string>
#include <iostream> 
#include <iomanip>

using namespace std;

struct Skill {
    string name;
    int damage;
    int cooldown;
    Skill(string n, int d, int cd) : name(n), damage(d), cooldown(cd) {}
};

struct Node {
    vector<int> boss_hp;            // BOSS血量数组
    int used_turns;                 // 已用回合数
    int current_boss;               // 当前目标BOSS索引
    unordered_map<string, int> cooldowns; // 技能冷却状态
    vector<pair<string, int>> sequence; // 技能序列（技能名，目标索引）
    int cost;    
    
    Node(const vector<int>& bh, int ut, int cb, 
         const unordered_map<string, int>& cd, 
         const vector<pair<string, int>>& seq);// 代价（已用回合 + 启发值）
    bool all_bosses_defeated() const;
    bool current_boss_defeated() const;
    int calculate_heuristic(const vector<Skill>& skills) const;
    bool operator>(const Node& other) const;
};

vector<pair<string, int>> branch_and_bound;