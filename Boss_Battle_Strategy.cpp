#include"Boss_Battle_Strategy.h"

Node::Node(const vector<int>& bh, int ut, int cb, 
        const unordered_map<string, int>& cd, 
        const vector<pair<string, int>>& seq)
    : boss_hp(bh), used_turns(ut), current_boss(cb), 
        cooldowns(cd), sequence(seq), cost(0) {}

// 检查所有BOSS是否被击败
bool Node::all_bosses_defeated() const {
    for (int hp : boss_hp) {
        if (hp > 0) return false;
    }
    return true;
}

// 检查当前BOSS是否被击败
bool Node::current_boss_defeated() const {
    if (current_boss >= boss_hp.size()) return true;
    return boss_hp[current_boss] <= 0;
}

int Node::calculate_heuristic(const vector<Skill>& skills) const {
    // 计算剩余BOSS总血量（从当前BOSS开始）
    int total_hp = 0;
    for (int i = current_boss; i < boss_hp.size(); ++i) {
        if (boss_hp[i] > 0) total_hp += boss_hp[i];
    }
    if (total_hp <= 0) return 0;

    // 定义伤害函数 D(K)：K回合内最大伤害
    auto D = [&](int K) -> int {
        int total_damage = 0;
        for (const Skill& s : skills) {
            int r_s = 0;
            auto it = cooldowns.find(s.name);
            if (it != cooldowns.end()) r_s = it->second;
            
            // 处理冷却时间为0的情况
            if (s.cooldown == 0) {
                if (r_s < K) { // 技能可用
                    total_damage += K * s.damage;
                }
            }
            // 正常冷却情况
            else if (r_s < K) { 
                int uses = 1 + (K - r_s - 1) / s.cooldown;
                total_damage += uses * s.damage;
            }
        }
        return total_damage;
    };

    // 二分查找最小K满足 D(K) >= total_hp
    int low = 0;
    int high = total_hp; // 初始上界
    
    // 倍增确定上界
    while (D(high) < total_hp) {
        low = high;
        high = (high > 1e6) ? high + 1 : high * 2;
        if (high > 10e6) return 1000000; // 安全终止
    }

    // 二分查找最小K
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (D(mid) >= total_hp) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    return low;
}

// 节点比较运算符
bool Node::operator>(const Node& other) const {
    return cost > other.cost;
}


vector<pair<string, int>> branch_and_bound(
    const vector<int>& initial_boss_hp, 
    const vector<Skill>& skills, 
    int turn_limit = INT_MAX) 
{
    // 初始化冷却状态
    unordered_map<string, int> initial_cooldowns;
    for (const auto& skill : skills) {
        initial_cooldowns[skill.name] = 0;
    }
    
    // 创建初始节点
    Node start_node(initial_boss_hp, 0, 0, initial_cooldowns, {});
    start_node.cost = start_node.used_turns + start_node.calculate_heuristic(skills);
    
    priority_queue<Node, vector<Node>, greater<Node>> queue;
    queue.push(start_node);
    
    int best_turns = INT_MAX;
    vector<pair<string, int>> best_sequence;
    
    while (!queue.empty()) {
        Node node = queue.top();
        queue.pop();
        
        // 剪枝：代价超过当前最优解
        if (node.cost >= best_turns) continue;
        
        // 回合限制检查
        if (node.used_turns >= turn_limit) continue;
        
        // 目标检查
        if (node.all_bosses_defeated()) {
            if (node.used_turns < best_turns) {
                best_turns = node.used_turns;
                best_sequence = node.sequence;
            }
            continue;
        }
        
        // 自动切换到第一个存活的BOSS
        int target_boss = node.current_boss;
        while (target_boss < node.boss_hp.size() && node.boss_hp[target_boss] <= 0) {
            target_boss++;
        }
        
        // 所有BOSS已击败但状态未更新
        if (target_boss >= node.boss_hp.size()) continue;
        
        // 遍历所有技能
        bool has_valid_action = false;
        for (const auto& skill : skills) {
            // 检查技能是否可用
            if (node.cooldowns.at(skill.name) == 0) {
                has_valid_action = true;
                
                // 创建新状态
                vector<int> new_boss_hp = node.boss_hp;
                new_boss_hp[target_boss] = max(0, new_boss_hp[target_boss] - skill.damage);
                
                // 更新冷却（关键修正：先减1再设置使用技能冷却）
                unordered_map<string, int> new_cooldowns;
                for (const auto& cd : node.cooldowns) {
                    new_cooldowns[cd.first] = max(0, cd.second - 1);
                }
                new_cooldowns[skill.name] = skill.cooldown; // 覆盖为完整冷却
                
                // 确定新当前BOSS
                int new_current_boss = target_boss;
                if (new_boss_hp[target_boss] <= 0) {
                    new_current_boss = target_boss + 1; // 自动切换到下一个
                }
                
                // 创建新节点
                vector<pair<string, int>> new_sequence = node.sequence;
                new_sequence.push_back({skill.name, target_boss});
                
                Node new_node(
                    new_boss_hp,
                    node.used_turns + 1,
                    new_current_boss,
                    new_cooldowns,
                    new_sequence
                );
                
                // 计算新代价
                new_node.cost = new_node.used_turns + new_node.calculate_heuristic(skills);
                
                // 剪枝并加入队列
                if (new_node.cost < best_turns) {
                    queue.push(new_node);
                }
            }
        }
    }
    
    return best_sequence;
}

    // // 示例输入：BOSS血量（第0位是BOSS个数，后面是各个BOSS的血量）
    // vector<int> boss_data = {2, 20, 15}; // 3个BOSS，血量分别为20,15,30
    // // 提取BOSS信息
    // int n = boss_data[0];
    // vector<int> boss_hp(boss_data.begin() + 1, boss_data.begin() + 1 + n);
    // // 技能定义实例
    // vector<Skill> skills = {
    //     Skill("Normal", 5, 0),    // 普通攻击：伤害5，无冷却
    //     Skill("Fireball", 8, 1),  // 火球术：伤害8，冷却1回合
    //     Skill("Ultimate", 15, 3)  // 大招：伤害15，冷却3回合
    // };
