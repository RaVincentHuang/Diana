#include "sysy/Target/Regallocator.h"
#include "sysy/Target/Block.h"
#include "sysy/Target/Instruction.h"
#include "sysy/Target/Function.h"
#include "sysy/Target/Reg.h"
#include "sysy/Support/debug.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <map>
 
using std::pair;
using std::set;
using std::vector;
using std::queue;
namespace sysy {
namespace mc {

extern int REGISTER_USAGE[16]; 

Regallocator::Regallocator(Function *_func) : func(_func) {}
    
    
    
    // void Regallocator::for_each_node(std::function<void(int)> f) {
    //     for (int i = 0; i < 16; ++i)
    //         if (occur.at(i)) f(i);
    //     for (int i : remain_pesudo_nodes) f(i);
    // }

void Regallocator::build() {
    // std::cout<<"build\n";
    for(int i=0;i<func->reg_num;i++){
        spill_cost[i]=0;
    }
    for(int i=0;i<func->vreg_num;i++){
        spill_cost_v[i]=0;
    }
    occur.resize(func->reg_num,nullptr);
    phy_occur.resize(16, 0);
    interfere_edge.resize(func->reg_num);
    occur_v.resize(func->vreg_num,nullptr);
    phy_occur_v.resize(32, 0);
    interfere_edge_v.resize(func->vreg_num);
    func->calc_live();//判断活跃变量
    // std::cout <<"regnum:" <<func->reg_num <<"\n";
    // std::cout << "vregnum:" <<func->vreg_num <<"\n";
    vector<Reg*> temp, new_nodes;
    vector<Reg*> temp_v, new_nodes_v;
    for (auto block : func->blocks) {
        double cur_loop=block->loop_depth;
        set<Reg*> live = block->live_out;
        set<Reg*> live_v = block->live_out_v;
        temp.clear();
        temp_v.clear();
        for (auto r : live)
            if (r->type==Reg::VIRTUAL || allocable(r->id)) temp.push_back(r);
        if (block->instructions.size() > 0)
            for (auto r : (*block->instructions.rbegin())->def_reg)
                if(r->kind==Reg::SREG)
                    if (r->type==Reg::VIRTUAL || allocable(r->id)) temp.push_back(r);
        for (size_t idx1 = 0; idx1 < temp.size(); ++idx1)
            for (size_t idx0 = 0; idx0 < idx1; ++idx0)
                if (temp[idx0]->id != temp[idx1]->id) {
                    if(temp[idx0]->type!=Reg::PHY)
                        interfere_edge[temp[idx0]->id].insert(temp[idx1]);
                    if(temp[idx1]->type!=Reg::PHY)
                        interfere_edge[temp[idx1]->id].insert(temp[idx0]);
                }

        for (auto r : live_v)
             temp_v.push_back(r);
        if (block->instructions.size() > 0)
            for (auto r : (*block->instructions.rbegin())->def_reg)
                if(r->kind==Reg::VREG)
                    temp_v.push_back(r);
        for (size_t idx1 = 0; idx1 < temp_v.size(); ++idx1)
            for (size_t idx0 = 0; idx0 < idx1; ++idx0)
                if (temp_v[idx0]->id != temp_v[idx1]->id) {
                    if(temp_v[idx0]->type!=Reg::PHY)
                        interfere_edge_v[temp_v[idx0]->id].insert(temp_v[idx1]);
                    if(temp_v[idx1]->type!=Reg::PHY)
                        interfere_edge_v[temp_v[idx1]->id].insert(temp_v[idx0]);
                }


        for (auto i = block->instructions.rbegin(); i != block->instructions.rend(); ++i) {
            //std::cout << Instruction::type_map[(*i)->instr_type] << "\n";
            new_nodes.clear();
            new_nodes_v.clear();
            for (auto r : (*i)->def_reg){
                if(r->kind==Reg::SREG){
                    if(r->type==Reg::VIRTUAL)
                        {
                            spill_cost[r->id]+=pow(10, cur_loop);
                            occur[r->id] = r;
                            std::set<Reg*> del;
                            for(auto rr:live){
                                if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                            }
                            for(auto delt:del){
                                live.erase(delt);
                            }
                            
                        }
                    if(r->type==Reg::PHY)
                    {
                            phy_occur[r->id] = 1;
                            std::set<Reg*> del;
                            for(auto rr:live){
                                if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                            }
                            for(auto delt:del){
                                live.erase(delt);
                            }
                    }
                }
                if(r->kind==Reg::VREG){
                    if(r->type==Reg::VIRTUAL)
                        {
                            spill_cost_v[r->id]+=pow(10, cur_loop);
                            occur_v[r->id] = r;
                            std::set<Reg*> del;
                            for(auto rr:live_v){
                                if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                            }
                            for(auto delt:del){
                                live_v.erase(delt);
                            }
                        }
                    if(r->type==Reg::PHY)
                    {
                            phy_occur_v[r->id] = 1;
                            std::set<Reg*> del;
                            for(auto rr:live_v){
                                if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                            }
                            for(auto delt:del){
                                live_v.erase(delt);
                            }
                    }
                }
                
            }    
            
            for (auto r : (*i)->use_reg) {
                if(r->kind==Reg::SREG) {
                    if( (r->type==Reg::VIRTUAL || allocable(r->id))) 
                    {
                        if(r->type==Reg::VIRTUAL){
                            spill_cost[r->id]+=pow(10, cur_loop);
                            occur[r->id] = r;
                        } 
                        else if(r->type==Reg::PHY) {
                            phy_occur[r->id] = 1;
                        }
                        if (live.find(r) == live.end()) {
                            int flag=0;
                            for(auto rr:live){
                                if(r->id==rr->id&&r->type==rr->type) flag=1;
                            }
                            if(flag==0){
                                for (auto o : live) {
                                    if(r->type!=Reg::PHY)
                                        interfere_edge[r->id].insert(o);
                                    if(o->type!=Reg::PHY)
                                        interfere_edge[o->id].insert(r);
                                }
                                live.insert(r);
                            }
                            
                        }   
                        new_nodes.push_back(r);
                    }
                }
                if(r->kind==Reg::VREG){
                    if(r->type==Reg::VIRTUAL){
                        spill_cost_v[r->id]+=pow(10, cur_loop);
                        occur_v[r->id] = r;
                    } 
                    else if(r->type==Reg::PHY) {
                        phy_occur_v[r->id] = 1;
                    }
                    if (live_v.find(r) == live_v.end()) {
                        int flag=0;
                        for(auto rr:live_v){
                            if(r->id==rr->id&&r->type==rr->type) flag=1;
                        }
                        if(flag==0){
                            for (auto o : live_v) {
                                if(r->type!=Reg::PHY)
                                    interfere_edge_v[r->id].insert(o);
                                if(o->type!=Reg::PHY)
                                    interfere_edge_v[o->id].insert(r);
                            }
                            live_v.insert(r);
                        }
                    }   
                    new_nodes_v.push_back(r);
                }

            }
                
            
            if (std::next(i) != block->instructions.rend()){
                for (auto r : (*std::next(i))->def_reg){
                    if(r->kind==Reg::SREG)
                        if ( (r->type==Reg::VIRTUAL || allocable(r->id))) {
                            new_nodes.push_back(r);
                            if (live.find(r) == live.end()){
                                int flag=0;
                                for(auto rr:live){
                                    if(r->id==rr->id&&r->type==rr->type) flag=1;
                                }
                                if(flag==0){
                                        for (auto o : live) {
                                        if(r->type!=Reg::PHY)
                                            interfere_edge[r->id].insert(o);
                                        if(o->type!=Reg::PHY)
                                            interfere_edge[o->id].insert(r);
                                    }
                                }
                            }
                                
                        }
                    if(r->kind==Reg::VREG)
                        {
                            new_nodes_v.push_back(r);
                            if (live_v.find(r) == live_v.end()){
                                int flag=0;
                                for(auto rr:live_v){
                                    if(r->id==rr->id&&r->type==rr->type) flag=1;
                                }
                                if(flag==0){
                                        for (auto o : live_v) {
                                        if(r->type!=Reg::PHY)
                                            interfere_edge_v[r->id].insert(o);
                                        if(o->type!=Reg::PHY)
                                            interfere_edge_v[o->id].insert(r);
                                    }
                                }
                            }
                        
                        }
                }
            }
                
            for (size_t idx1 = 0; idx1 < new_nodes.size(); ++idx1)
                for (size_t idx0 = 0; idx0 < idx1; ++idx0)
                if (new_nodes[idx0]->id != new_nodes[idx1]->id) {
                    if(new_nodes[idx0]->type!=Reg::PHY)
                        interfere_edge[new_nodes[idx0]->id].insert(new_nodes[idx1]);
                    if(new_nodes[idx1]->type!=Reg::PHY)
                        interfere_edge[new_nodes[idx1]->id].insert(new_nodes[idx0]);
       
            }

            for (size_t idx1 = 0; idx1 < new_nodes_v.size(); ++idx1)
                for (size_t idx0 = 0; idx0 < idx1; ++idx0)
                if (new_nodes_v[idx0]->id != new_nodes_v[idx1]->id) {
                    if(new_nodes_v[idx0]->type!=Reg::PHY)
                        interfere_edge_v[new_nodes_v[idx0]->id].insert(new_nodes_v[idx1]);
                    if(new_nodes_v[idx1]->type!=Reg::PHY)
                        interfere_edge_v[new_nodes_v[idx1]->id].insert(new_nodes_v[idx0]);
       
            }

        }
    }

    for (int i = 0; i < func->reg_num; ++i)
        if (occur.at(i)!=nullptr) {
            remain_pesudo_nodes.insert(occur.at(i));
        }
    for (int i = 0; i < func->vreg_num; ++i)
        if (occur_v.at(i)!=nullptr) {
            remain_pesudo_nodes_v.insert(occur_v.at(i));
        }
}

void Regallocator::simplify() {
    queue<Reg*> simplify_q;
    for (auto i : remain_pesudo_nodes) {
        if (interfere_edge[i->id].size() <14)
            simplify_q.push(i);
        
    }
    while(simplify_q.size()){
        //PrintMsg("while1");
        while (simplify_q.size()) {
            auto cur = simplify_q.front();
            simplify_q.pop();
            vector<Reg*> neighbors;
            //PrintMsg("while2");
            neighbors.reserve(interfere_edge[cur->id].size());
            for (auto i : interfere_edge[cur->id]) {
                //std::cout<<i->id<<" ";
                neighbors.push_back(i);
                if(i->type==Reg::VIRTUAL)
                    interfere_edge[i->id].erase(cur);
            }
            //std::cout<<"\n";
            simplify_history.emplace_back(cur, neighbors);
            interfere_edge[cur->id].clear();
            remain_pesudo_nodes.erase(cur);
            //std::cout<<"simplify:"<<cur->id<<"\n";    
        }
        for (auto i : remain_pesudo_nodes) {
            if (interfere_edge[i->id].size() <14)
                simplify_q.push(i);
        }
    }
}
void Regallocator::simplify_v() {
    queue<Reg*> simplify_q;
    for (auto i : remain_pesudo_nodes_v) {
        if (interfere_edge_v[i->id].size() <24)
            simplify_q.push(i);
        
    }
    while(simplify_q.size()){
        //PrintMsg("while1");
        while (simplify_q.size()) {
            auto cur = simplify_q.front();
            simplify_q.pop();
            vector<Reg*> neighbors;
            //PrintMsg("while2");
            neighbors.reserve(interfere_edge_v[cur->id].size());
            for (auto i : interfere_edge_v[cur->id]) {
                //std::cout<<i->id<<" ";
                neighbors.push_back(i);
                if(i->type==Reg::VIRTUAL)
                    interfere_edge_v[i->id].erase(cur);
                
            }
            //std::cout<<"\n";
            simplify_history_v.emplace_back(cur, neighbors);
            interfere_edge_v[cur->id].clear();
            remain_pesudo_nodes_v.erase(cur);
            //std::cout<<"simplify:"<<cur->id<<"\n";    
        }
        for (auto i : remain_pesudo_nodes_v) {
            if (interfere_edge_v[i->id].size() <24)
                simplify_q.push(i);
        }
    }
}
Reg* Regallocator::spill() { 
    int spill_node = -1;
    Reg* choose_spill;
    double spill_heuristic = 0;
    for (auto i : remain_pesudo_nodes){
        if(func->spilling_nodes.find(i->id)==func->spilling_nodes.end())
        {
            int countt=0;
            double cur_spill_heuristic=static_cast<double>(interfere_edge[i->id].size())/(pow(1.4,spill_cost[i->id]));
            for(auto r:interfere_edge[i->id]){

                if(interfere_edge[r->id].size()<=14){
                    countt++;
                } 
            }
            
            if (spill_node == -1 || cur_spill_heuristic > spill_heuristic) {
                spill_node = i->id;
                spill_heuristic = cur_spill_heuristic;
                choose_spill = i;
            }
        }
            
    }  
    assert(spill_node != -1);
    remain_pesudo_nodes.erase(choose_spill);
    for (auto i : interfere_edge[spill_node]) if(i->type==Reg::VIRTUAL) interfere_edge[i->id].erase(choose_spill);
    interfere_edge[spill_node].clear();
    func->spilling_nodes.insert(choose_spill->id);
    return choose_spill;
    }
Reg* Regallocator::spill_v() { 
    int spill_node = -1;
    Reg* choose_spill;
    double spill_heuristic = 0;
    for (auto i : remain_pesudo_nodes_v){
        //std::cout<<i->id<<":"<<interfere_edge[i->id].size()<<"\n";
        
        if(func->spilling_nodes_v.find(i->id)==func->spilling_nodes_v.end()){
            int countt=0;
            double cur_spill_heuristic=static_cast<double>(interfere_edge_v[i->id].size())/(pow(1.4,spill_cost_v[i->id]));
            for(auto r:interfere_edge_v[i->id]){

                if(interfere_edge_v[r->id].size()<=14){
                    countt++;
                } 
            }
            if (spill_node == -1 || cur_spill_heuristic > spill_heuristic) {
                spill_node = i->id;
                spill_heuristic = cur_spill_heuristic;
                choose_spill = i;
            }
        }
            
    }  


    assert(spill_node != -1);
    remain_pesudo_nodes_v.erase(choose_spill);
    for (auto i : interfere_edge_v[spill_node]) if(i->type==Reg::VIRTUAL) interfere_edge_v[i->id].erase(choose_spill);
    interfere_edge_v[spill_node].clear();
    func->spilling_nodes_v.insert(choose_spill->id);
    return choose_spill;
    }
void Regallocator::add_spill_code(const vector<Reg*> &spill_nodes) {
    set<int> constant_spilled;
    func->spill_count=0;
    for (auto &block : func->blocks){
        for (size_t j = 0; j < spill_nodes.size(); ++j){
            int flag=0,symbol=0;
            auto id = spill_nodes[j];
            for (auto i = block->instructions.begin(); i != block->instructions.end();) { 
                bool cur_def = (*i)->def(id->id);
                bool cur_use = (*i)->use(id->id);
                if(flag==1){
                    if(!func->offset[id->id]){
                        func->offset[id->id]=func->sp;
                        int cur_offset=func->spill_count+func->sp-1;
                        func->sp+=4;
                        block->instructions.insert(i, new StoreImm(StoreImm::SSTORE, new Reg(13,Reg::Type::PHY),id, cur_offset));
                    }
                    else{
                        int cur_offset=func->spill_count+func->offset[id->id]-1;
                        block->instructions.insert(i, new StoreImm(StoreImm::SSTORE, new Reg(13,Reg::Type::PHY),id, cur_offset));
                    }
                    flag=0;
                }
                if(cur_def) flag=1;
                if (cur_use) {
                    if(!func->offset[id->id]){
                        func->offset[id->id]=func->sp;
                        int cur_offset=func->spill_count+func->sp-1;
                        func->sp+=4;
                    
                        block->instructions.insert(i, new LoadImm(LoadImm::SLOAD, id, new Reg(13,Reg::Type::PHY),cur_offset));
                    }
                    else{
                        //int tmp1=func->reg_num++;
                        int cur_offset=func->spill_count+func->offset[id->id]-1;
                        block->instructions.insert(i, new LoadImm(LoadImm::SLOAD, id, new Reg(13,Reg::Type::PHY),cur_offset));
                        //(*i)->replace_reg(id->id, tmp1);
                    }
                }
                if(Push * ifff = dynamic_cast<Push *>(*i)) func->spill_count+=4;
                if(symbol==1){
                    if(BinaryImm * iff = dynamic_cast<BinaryImm *>(*i)){
                        if(iff->type==  BinaryImm::ADD  && iff->dst->id==13 && iff->dst->type==Reg::PHY)
                            func->spill_count-=iff->rhs;
                    }
                    symbol=0;
                }
                if(Call * ifff = dynamic_cast<Call *>(*i)){
                    symbol=1;
                    
                }
                if(BinaryImm * iff = dynamic_cast<BinaryImm *>(*i)){
                    if(iff->type==  BinaryImm::SUB  && iff->dst->id==13 && iff->dst->type==Reg::PHY && iff->rhs ==4)
                                func->spill_count+=iff->rhs;
                }
                ++i;
            } 
        }
    }       
    }
void Regallocator::add_spill_code_v(const vector<Reg*> &spill_nodes) {
    set<int> constant_spilled;
    func->spill_count=0;
    for (auto &block : func->blocks){
        for (size_t j = 0; j < spill_nodes.size(); ++j){
            int flag=0,symbol=0;
            auto id = spill_nodes[j];
            for (auto i = block->instructions.begin(); i != block->instructions.end();) {
                bool cur_def = (*i)->def_v(id->id);
                bool cur_use = (*i)->use_v(id->id);
                if(flag==1){
                    if(!func->offset_v[id->id]){
                        func->offset_v[id->id]=func->sp;
                        int cur_offset=func->spill_count+func->sp-1;
                        func->sp+=4;
                        block->instructions.insert(i, new StoreImm(StoreImm::VSTORE, new Reg(13,Reg::Type::PHY),id, cur_offset));
                    }
                    else{
                        int cur_offset=func->spill_count+func->offset_v[id->id]-1;
                        block->instructions.insert(i, new StoreImm(StoreImm::VSTORE, new Reg(13,Reg::Type::PHY),id, cur_offset));
                    }
                    flag=0;
                }
                if(cur_def) flag=1;
                if (cur_use) {
                    if(!func->offset_v[id->id]){
                        func->offset_v[id->id]=func->sp;
                        int cur_offset=func->spill_count+func->sp-1;
                        func->sp+=4;
                        block->instructions.insert(i, new LoadImm(LoadImm::VLOAD, id, new Reg(13,Reg::Type::PHY),cur_offset));
                    }
                    else{
                        //int tmp1=func->reg_num++;
                        int cur_offset=func->spill_count+func->offset_v[id->id]-1;
                        block->instructions.insert(i, new LoadImm(LoadImm::VLOAD, id, new Reg(13,Reg::Type::PHY),cur_offset));
                        //(*i)->replace_reg(id->id, tmp1);
                    }
                }
                if(Push * ifff = dynamic_cast<Push *>(*i)) func->spill_count+=4;
                if(symbol==1){
                    if(BinaryImm * iff = dynamic_cast<BinaryImm *>(*i)){
                        if(iff->type==  BinaryImm::ADD  && iff->dst->id==13 && iff->dst->type==Reg::PHY)
                            func->spill_count-=iff->rhs;
                    }
                    symbol=0;
                }
                if(Call * ifff = dynamic_cast<Call *>(*i)){
                    symbol=1;
                    
                }
                if(BinaryImm * iff = dynamic_cast<BinaryImm *>(*i)){
                    if(iff->type==  BinaryImm::SUB  && iff->dst->id==13 && iff->dst->type==Reg::PHY && iff->rhs ==4)
                                func->spill_count+=iff->rhs;
                }
                ++i;
            } 
        }
    }       
}
void Regallocator::add_spill_code_v_onlys(const vector<Reg*> &spill_nodes) {
    set<int> constant_spilled;
    set<Instruction*> del;
    func->spill_count=0;
    for (auto &block : func->blocks){
        for (size_t j = 0; j < spill_nodes.size(); ++j){
            int flag=0,symbol=0;
            Reg * here;
            auto id = spill_nodes[j];
            for (auto i = block->instructions.begin(); i != block->instructions.end();) {
                bool cur_def = (*i)->def_v(id->id);
                bool cur_use = (*i)->use_v(id->id);
                if(!cur_def)
                    for(auto merge : move_edge [id->id]){
                        cur_def = (*i)->def_v(merge);
                        if(cur_def) break;
                    }
                if(!cur_use)
                    for(auto merge : move_edge[id->id]){
                        cur_use = (*i)->use_v(merge);
                        if(cur_use) break;
                    }
                if(cur_def){
                        del.insert(*i);
                        if(MoveReg * ifff = dynamic_cast<MoveReg *>(*i)) here = ifff->lhs; 
                        if(!func->offset_v[id->id]){
                            func->offset_v[id->id]=func->sp;
                            int cur_offset=func->spill_count+func->sp-1;
                            func->sp+=4;
                            block->instructions.insert(i, new StoreImm(StoreImm::SSTORE, new Reg(13,Reg::Type::PHY,Reg::SREG),here, cur_offset));
                        }
                        else{
                            int cur_offset=func->spill_count+func->offset_v[id->id]-1;
                            block->instructions.insert(i, new StoreImm(StoreImm::SSTORE, new Reg(13,Reg::Type::PHY,Reg::SREG),here, cur_offset));
                        }
                }
                if (cur_use) {
                        del.insert(*i);
                        if(MoveReg * ifff = dynamic_cast<MoveReg *>(*i)) here = ifff->dst;
                        if(!func->offset_v[id->id]){
                            func->offset_v[id->id]=func->sp;
                            int cur_offset=func->spill_count+func->sp-1;
                            func->sp+=4;
                            block->instructions.insert(i, new LoadImm(LoadImm::SLOAD, here,new Reg(13,Reg::Type::PHY,Reg::SREG), cur_offset));
                        }
                        else{
                            int cur_offset=func->spill_count+func->offset_v[id->id]-1;
                            block->instructions.insert(i, new LoadImm(LoadImm::SLOAD,here, new Reg(13,Reg::Type::PHY,Reg::SREG), cur_offset));
                        }  
                }
                if(Push * ifff = dynamic_cast<Push *>(*i)) func->spill_count+=4;
                if(symbol==1){
                    if(BinaryImm * iff = dynamic_cast<BinaryImm *>(*i)){
                        if(iff->type==  BinaryImm::ADD  && iff->dst->id==13 && iff->dst->type==Reg::PHY)
                            func->spill_count-=iff->rhs;
                    }
                    symbol=0;
                }
                if(Call * ifff = dynamic_cast<Call *>(*i)){
                    symbol=1;
                    
                }
                if(BinaryImm * iff = dynamic_cast<BinaryImm *>(*i)){
                    if(iff->type==  BinaryImm::SUB  && iff->dst->id==13 && iff->dst->type==Reg::PHY && iff->rhs ==4)
                                func->spill_count+=iff->rhs;
                }
                ++i;
            } 
        }
    }  
    for (auto &block : func->blocks){
        for(auto i:del){
            block->instructions.remove(i);
        }  
    }
       
}
void Regallocator::add_spill_code_onlys(const vector<Reg*> &spill_nodes) {
    set<int> constant_spilled;
    func->spill_count=0;
    for (auto &block : func->blocks){
        for (size_t j = 0; j < spill_nodes.size(); ++j){
            int flag=0,symbol=0;
            Reg * here;
            auto id = spill_nodes[j];
            for (auto i = block->instructions.begin(); i != block->instructions.end();) {
                bool cur_def = (*i)->def(id->id);
                bool cur_use = (*i)->use(id->id);
                if(!cur_def)
                    for(auto merge : move_edge [id->id]){
                        cur_def = (*i)->def(merge);
                        if(cur_def) break;
                    }
                if(!cur_use)
                    for(auto merge : move_edge[id->id]){
                        cur_use = (*i)->use(merge);
                        if(cur_use) break;
                    }
                if(flag==1){
                        if(func->off_reg[id->id]==nullptr){
                            int tmp1=func->vreg_num++;
                            func->off_reg[id->id]=new Reg(tmp1,Reg::VIRTUAL,Reg::VREG);
                            block->instructions.insert(i, new MoveReg(MoveReg::SVMOV, func->off_reg[id->id], id));
                        }
                        else{
                            block->instructions.insert(i, new MoveReg(MoveReg::SVMOV, func->off_reg[id->id], id));
                        }
                        flag=0; 
                }
                if(cur_def) {
                    flag=1;
                }
                if (cur_use) {
                    
                        if(func->off_reg[id->id]==nullptr){
                            int tmp1=func->vreg_num++;
                            func->off_reg[id->id]=new Reg(tmp1,Reg::VIRTUAL,Reg::VREG);
                            block->instructions.insert(i, new MoveReg(MoveReg::SVMOV, id, func->off_reg[id->id]));
                        }else{
                            //int tmp1=func->reg_num++;
                            block->instructions.insert(i, new MoveReg(MoveReg::SVMOV, id, func->off_reg[id->id]));
                            //(*i)->replace_reg(id->id, tmp1);
                        }
                   
                }
                ++i;
            } 
        }
    }  

}
void Regallocator::coalesce() {
    for(auto block:func->blocks){
        for(auto inst:block->instructions){
            int k=0;
            set<int> tell;
            if(MoveReg * ifff = dynamic_cast<MoveReg *>(inst)){
                if(ifff->dst->id!=ifff->lhs->id&& ifff->dst->type==Reg::VIRTUAL && ifff->lhs->type==Reg::VIRTUAL&&ifff->dst->kind==Reg::SREG&&ifff->lhs->kind==Reg::SREG){
                    if(interfere_edge[ifff->dst->id].find(ifff->lhs)==interfere_edge[ifff->lhs->id].end()){
                        for(auto r : interfere_edge[ifff->lhs->id]){
                            if(interfere_edge[r->id].size()>14){
                                k++;
                                tell.insert(r->id);
                            }
                        }
                        for(auto r : interfere_edge[ifff->dst->id]){
                            if(interfere_edge[r->id].size()> 14 &&tell.find(r->id)==tell.end()){
                                k++;
                                tell.insert(r->id);
                            }
                        }
                        if(k<14) {
                            move_edge[ifff->dst->id].insert(ifff->lhs->id);
                            for(auto merge:move_edge[ifff->lhs->id]){
                                move_edge[ifff->dst->id].insert(merge);
                            } 
                            remain_pesudo_nodes.erase(ifff->lhs);
                            for(auto r : interfere_edge[ifff->lhs->id]){
                                if(interfere_edge[ifff->dst->id].find(r)==interfere_edge[ifff->dst->id].end()){
                                    interfere_edge[ifff->dst->id].insert(r);
                                    interfere_edge[r->id].erase(ifff->lhs);
                                    interfere_edge[r->id].insert(ifff->dst);
                                }
                            }
                            interfere_edge[ifff->lhs->id].clear();
                        }
                    }     
                }
                if(ifff->dst->id!=ifff->lhs->id&&ifff->dst->type==Reg::VIRTUAL && ifff->lhs->type==Reg::VIRTUAL&&ifff->dst->kind==Reg::VREG&&ifff->lhs->kind==Reg::VREG){
                    if(interfere_edge_v[ifff->dst->id].find(ifff->lhs)==interfere_edge_v[ifff->lhs->id].end()){
                        for(auto r : interfere_edge_v[ifff->lhs->id]){
                            if(interfere_edge_v[r->id].size()>24){
                                k++;
                                tell.insert(r->id);
                            }
                        }
                        for(auto r : interfere_edge_v[ifff->dst->id]){
                            if(interfere_edge_v[r->id].size()>24&&tell.find(r->id)==tell.end()){
                                k++;
                                tell.insert(r->id);
                            }
                        }
                        if(k<24) {
                            move_edge_v[ifff->dst->id].insert(ifff->lhs->id);
                            for(auto merge:move_edge_v[ifff->lhs->id]){
                                move_edge_v[ifff->dst->id].insert(merge);
                            } 
                            remain_pesudo_nodes_v.erase(ifff->lhs);
                            for(auto r : interfere_edge_v[ifff->lhs->id]){
                                if(interfere_edge_v[ifff->dst->id].find(r)==interfere_edge_v[ifff->dst->id].end()){
                                    interfere_edge_v[ifff->dst->id].insert(r);
                                    interfere_edge_v[r->id].erase(ifff->lhs);
                                    interfere_edge_v[r->id].insert(ifff->dst);
                                }
                            }
                            interfere_edge_v[ifff->lhs->id].clear();
                        }
                    }     
                }
            }
        }
    }
}


vector<int> Regallocator::run(RegAllocStat *stat) {
    
    build();
    // std::cout<<"here\n";
    stat->succeed = true;
    vector<Reg*> spill_list;
    vector<Reg*> spill_list_v;
    for(auto i=0;i<interfere_edge.size();i++){
        set<int> num;
        set<int> num1;
        set<Reg*> del;
        del.clear();
        for(auto r:interfere_edge[i]){
            
            if(r->type==Reg::VIRTUAL){
                if(num.find(r->id)==num.end()){
                    num.insert(r->id);
                }
                else{
                    del.insert(r);
                }
            }else{
                if(num1.find(r->id)==num1.end()){
                    num1.insert(r->id);
                }
                else{
                    del.insert(r);
                }
            }
        }
        for(auto r:del){
            interfere_edge[i].erase(r);
        }
    }

    for(auto i=0;i<interfere_edge_v.size();i++){
        set<int> num;
        set<int> num1;
        set<Reg*> del;
        del.clear();
        for(auto r:interfere_edge_v[i]){
            
            if(r->type==Reg::VIRTUAL){
                if(num.find(r->id)==num.end()){
                    num.insert(r->id);
                }
                else{
                    del.insert(r);
                }
                
            }else{
                if(num1.find(r->id)==num1.end()){
                    num1.insert(r->id);
                }
                else{
                    del.insert(r);
                    
                }
            }
        }
        for(auto r:del){
            interfere_edge_v[i].erase(r);
        }
    }
    
    coalesce();
    //std::cout << "coalesce" <<"\n";
    while (remain_pesudo_nodes.size()) {

        simplify();
    //std::cout << "simplify" <<"\n";
        if (remain_pesudo_nodes.size() <= 0) break;
        stat->succeed = false;
        if(!stat->ifonly)
            ++stat->spill_cnt;
        spill_list.push_back(spill());
    }

    while (remain_pesudo_nodes_v.size()) {

        simplify_v();
    //std::cout << "simplify_v" <<"\n";
        if (remain_pesudo_nodes_v.size() <= 0) break;
        stat->succeed = false;
        ++stat->spill_cnt;
        spill_list_v.push_back(spill_v());
    }
    //std::cout << "finish simplify" <<"\n";

    if (!stat->succeed) {
        if(stat->ifonly){
            add_spill_code_onlys(spill_list);
            add_spill_code_v_onlys(spill_list_v);
        }else {
            add_spill_code(spill_list);
            add_spill_code_v(spill_list_v);
        }
        
        return {};
    }
    //std::cout << "choose spill" <<"\n";
    vector<int> ans;
    vector<int> ans_v;
    bool used[16] = {};
    bool used_v[32] = {};
    ans.resize(func->reg_num);
    ans_v.resize(func->vreg_num);
    std::fill(ans.begin(), ans.end(), -1);
    std::fill(ans_v.begin(), ans_v.end(), -1);
    for (int i = 0; i < 16; ++i)
        if (phy_occur.at(i)) {
            used[i] = true;
        }
    for (int i = 0; i < 32; ++i)
        if (phy_occur_v.at(i)) {
            used_v[i] = true;
        }
    
    for (size_t i = simplify_history.size() - 1; i < simplify_history.size();--i) {
        if(simplify_history.at(i).first->type==Reg::VIRTUAL){
            bool flag[16] = {};
            for (auto neighbor : simplify_history.at(i).second) {
                if(neighbor->type==Reg::VIRTUAL) flag[ans[neighbor->id]] = true;
                if(neighbor->type==Reg::PHY) flag[neighbor->id] = true;
            }    
            for (int j = 0; j < 16; ++j)
                if ((allocable(j) && used[j]) && !flag[j]) {
                    ans[simplify_history.at(i).first->id] = j;
                    break;
                }
            if (ans[simplify_history.at(i).first->id] == -1)
                for (int j = 0; j < 16; ++j)
                    if (allocable(j) && !flag[j]) {
                    ans[simplify_history.at(i).first->id] = j;
                    break;
                }
            for(auto merge:move_edge[simplify_history.at(i).first->id]){
                ans[merge]=ans[simplify_history.at(i).first->id];
            }
            used[ans[simplify_history.at(i).first->id]] = true;
        }
    }
    for (size_t i = simplify_history_v.size() - 1; i < simplify_history_v.size();--i) {
        if(simplify_history_v.at(i).first->type==Reg::VIRTUAL){
            bool flag[32] = {};
            for (auto neighbor : simplify_history_v.at(i).second) {
                if(neighbor->type==Reg::VIRTUAL) flag[ans_v[neighbor->id]] = true;
                if(neighbor->type==Reg::PHY) flag[neighbor->id] = true;
            }    
            for (int j = 8; j < 32; ++j)
                if (( used_v[j]) && !flag[j]) {
                    ans_v[simplify_history_v.at(i).first->id] = j;
                    break;
                }
            if (ans_v[simplify_history_v.at(i).first->id] == -1)
                for (int j = 8; j < 32; ++j)
                    if (  !flag[j]) {
                    ans_v[simplify_history_v.at(i).first->id] = j;
                    break;
                }
            for(auto merge:move_edge_v[simplify_history_v.at(i).first->id]){
                ans_v[merge]=ans_v[simplify_history_v.at(i).first->id];
            }
            used_v[ans_v[simplify_history_v.at(i).first->id]] = true;
        }
    }
    // std::cout<<"here3\n";
    vector<int> res;
    res.resize(func->reg_num + func->vreg_num);
    
    std::fill(res.begin(), res.end(), -1);
    for(int i=0;i<ans.size();i++){
        res[i]=ans[i];
    }
    
    for(int i=0;i< ans_v.size();i++){
        res[func->reg_num+i]=ans_v[i];
    }
    for (int i = 0; i < 16; ++i)
        if (used[i] && REGISTER_USAGE[i] == 2) ++stat->callee_save_used;
    
    return res;
}
}
}
