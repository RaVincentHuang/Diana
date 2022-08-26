#ifndef SYSY_TARGET_REGALLOCATOR_H
#define SYSY_TARGET_REGALLOCATOR_H

#include <queue>
#include <set>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "sysy/Target/Function.h"
#include "sysy/Target/Reg.h"
namespace sysy {
namespace mc {

// no coalescing
class Regallocator {
    public:
    Function *func;
    std::vector<Reg*> occur;  // in fact it's boolean array
    std::vector<int> phy_occur;  // in fact it's boolean array
    std::vector<std::set<Reg*>> interfere_edge;
    std::map<int, std::set<int>> move_edge;
    std::vector<std::pair<Reg*, std::vector<Reg*>>> simplify_history;
    std::set<Reg*> remain_pesudo_nodes;
    std::vector<Reg*> occur_v;  // in fact it's boolean array
    std::vector<int> phy_occur_v;  // in fact it's boolean array
    std::vector<std::set<Reg*>> interfere_edge_v;
    std::map<int, std::set<int>> move_edge_v;
    std::map<int, double> spill_cost;
    std::map<int, double> spill_cost_v;
    std::vector<std::pair<Reg*, std::vector<Reg*>>> simplify_history_v;
    std::set<Reg*> remain_pesudo_nodes_v;
    int num=0;
    void build();
    void simplify();
    void simplify_v();
    void coalesce();
    void freeze();
    Reg* spill();
    Reg* spill_v();
    int get_root(int x);
    void add_spill_code(const std::vector<Reg*> &spill_nodes);
    void add_spill_code_v(const std::vector<Reg*> &spill_nodes);
    void add_spill_code_onlys(const std::vector<Reg*> &spill_nodes);
    void add_spill_code_v_onlys(const std::vector<Reg*> &spill_nodes);
    bool conservative_check(int u, int v);
    void merge(int u, int v);
    void for_each_node(std::function<void(int)> f);
    Regallocator(Function *_func);
    std::vector<int> run(RegAllocStat *stat);
    
};



} // namespace mc 
} // namespace sysy


#endif