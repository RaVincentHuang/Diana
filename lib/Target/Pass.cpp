#include "sysy/Target/Pass.h"
#include "sysy/Target/Instruction.h"

namespace sysy 
{
namespace mc 
{

bool is_necessary(Instruction* instr) {
    if (auto br = dynamic_cast<Branch*>(instr)) 
        return true;
    else if (auto store = dynamic_cast<Push*>(instr)) 
        return true;
    else if (auto store = dynamic_cast<Pop*>(instr)) 
        return true;
    else if (auto add_sp = dynamic_cast<BinaryImm*>(instr)) {
        if (add_sp->type == BinaryImm::ADD && add_sp->dst->id == 13 && add_sp->lhs->id == 13)
            return true;
    }
    else if (auto store = dynamic_cast<StoreImm*>(instr)) 
        return true;
    else if (auto store = dynamic_cast<StoreReg*>(instr)) 
        return true;
    else if (auto call = dynamic_cast<Call*>(instr)) 
        return true;
    else if (auto ret = dynamic_cast<Return*>(instr)) 
        return true;

    return false;
}

bool is_compare(Instruction* instr) {
    if (auto comp = dynamic_cast<CompareReg*>(instr))
        return true;
    if (auto comp = dynamic_cast<CompareImm*>(instr))
        return true;
    return false;
}

bool may_effect_load(Instruction* instr) {
    // if (auto call = dynamic_cast<Call*>(instr)) {
    //     std::string name = call->callee->name;
    //     if (name == "putint" || name == "putch" || 
    //         name == "putfloat" || name == "putarray" || name == "putfarray")
    //         return false;
    //     else
    //         return true;
    // }
    // return false;

    if (auto call = dynamic_cast<Call*>(instr))
        return true;
    else
        return false;
}

bool may_effect_store(Instruction* instr) {
    // if (auto call = dynamic_cast<Call*>(instr)) {
    //     std::string name = call->callee->name;
    //     if (name == "getint" || name == "getch" || 
    //         name == "getfloat" || name == "getarray" || name == "getfarray")
    //         return false;
    //     else
    //         return true;
    // }
    // return false;
    if (auto call = dynamic_cast<Call*>(instr))    
        return true;    
    else
        return false;
}

void analyseType(Function* func) {
    bool all_int = true;
    bool all_float = true;
    for (auto block : func->blocks)
        for (auto it : block->instructions) {
            if (it->is_float)
                all_int = false;
            else
                all_float = false;
        }

    if (all_int)
        func->type = Function::AllInt;
    else if (all_float)
        func->type = Function::AllFloat;
    else
        func->type = Function::IntFloat;

    return;
}

void remove_top_to_bottom(Function* func) {
    struct RegObject {
        enum { ConstantInt, ConstantFloat, Address, StackObject } type;
        int int_value;
        int float_value;
        std::string address_name;
    };
    
}

void remove_bottom_to_top(Function* func) {
    func->calc_live();


    for (auto block : func->blocks) {
        std::set<std::pair<int, Reg::Type>> live_out; 
        std::set<std::pair<int, Reg::Type>> live_out_v; 
        for (auto r :  block->live_out)
            live_out.insert({r->id, r->type});
        for (auto r :  block->live_out_v)
            live_out_v.insert({r->id, r->type});

        bool cond = false;
        for (auto it = block->instructions.rbegin(); it != block->instructions.rend();) {
            bool used = is_necessary(*it) || (is_compare(*it) && cond);
            for (auto r : (*it)->def_reg) {
                if (r->type == Reg::PHY) {                    
                    if ( r->kind == Reg::SREG && (!allocable(r->id) || live_out.count({r->id, Reg::PHY})) )
                        used = true;
                    if ( r->kind == Reg::VREG && live_out_v.count({r->id, Reg::PHY}) )
                        used = true;
                }
                else {
                    if ( r->kind == Reg::SREG && live_out.count({r->id, Reg::VIRTUAL}) )    
                        used = true;                    
                    if ( r->kind == Reg::VREG && live_out_v.count({r->id, Reg::VIRTUAL}) )
                        used = true;
                }
            }

            if (!used) {
                // if (block->name == "float_eq.copy.float_abs.copy.ifnext__19") {
                //     B("no use");
                //     B((*it)->gen_asm());
                //     for (auto i : live_out)
                //         std::cout << i.first << " " << i.second << "   ";
                //     std::cout << "\n";
                //     for (auto i : live_out_v)
                //         std::cout << i.first << " " << i.second << "   ";
                //     std::cout << "\n";                    
                // }

                auto base = it.base();
                block->instructions.erase(std::prev(base));
                it = std::make_reverse_iterator(base);
            } else {
                if (is_compare(*it)) cond = false;
                if ((*it)->cond != Instruction::AL) cond = true;

                for (auto r : (*it)->def_reg) {
                    if (r->type == Reg::PHY) {
                        if (r->kind == Reg::SREG && allocable(r->id))
                            live_out.erase({r->id, Reg::PHY});
                        else if (r->kind == Reg::VREG)
                            live_out_v.erase({r->id, Reg::PHY});  
                    }
                    if (r->type == Reg::VIRTUAL) {
                        if (r->kind == Reg::SREG)
                            live_out.erase({r->id, Reg::VIRTUAL});
                        else if (r->kind == Reg::VREG)
                            live_out_v.erase({r->id, Reg::VIRTUAL});
                    }
                }

                for (auto r : (*it)->use_reg) {
                    if (r->type == Reg::PHY) {
                        if (r->kind == Reg::SREG && allocable(r->id))
                            live_out.insert({r->id, Reg::PHY});
                        else if (r->kind == Reg::VREG)
                            live_out_v.insert({r->id, Reg::PHY});  
                    }
                    if (r->type == Reg::VIRTUAL) {
                        if (r->kind == Reg::SREG)
                            live_out.insert({r->id, Reg::VIRTUAL});
                        else if (r->kind == Reg::VREG)
                            live_out_v.insert({r->id, Reg::VIRTUAL});
                    }
                }

                ++it;
            }

        }
    }
}

void remove_same_move(Function* func) {
    for (auto &block : func->blocks)
        for (auto it = block->instructions.begin(); it != block->instructions.end();) {
            auto nxt = std::next(it);
            if (auto move = dynamic_cast<MoveReg*>(*it))
                if (move->dst->id == move->lhs->id) block->instructions.erase(it);
            it = nxt;
        }
}

void remove_multi_store(Function* func) {

    std::set<std::pair<int, int>> store_with_imm;
    for (auto block : func->blocks) {
        store_with_imm.clear();
        for (auto it = block->instructions.rbegin(); it != block->instructions.rend();) {
            bool used = true;
            if (auto store = dynamic_cast<StoreImm*>(*it)) {
                if (store_with_imm.count({store->dst->id, store->offset}))
                    used = false;
                else
                    store_with_imm.insert({store->dst->id, store->offset});
            }
            else if (auto load = dynamic_cast<LoadImm*>(*it)) {
                if (store_with_imm.count({load->lhs->id, load->offset}))
                    store_with_imm.erase({load->lhs->id, load->offset});
            }
            else if (may_effect_store(*it))
                store_with_imm.clear();


            if (!used) {
                auto base = it.base();
                block->instructions.erase(std::prev(base));
                it = std::make_reverse_iterator(base);
            }
            else
                ++it;
        }
    }
}

void remove_multi_load(Function* func) {

    std::map<std::pair<int, int>, int> load_with_imm;
    // std::map<int, std::set<int>> dst_regs;

    for (auto block : func->blocks) {
        load_with_imm.clear();
        for (auto it = block->instructions.begin(); it != block->instructions.end();) {
            bool used = true;
            if (auto load = dynamic_cast<LoadImm*>(*it)) {
                if (load_with_imm.count({load->lhs->id, load->offset}))
                    used = false;
                else {
                    load_with_imm.insert({{load->lhs->id, load->offset}, load->dst->id});
                    // dst_regs[load->lhs->id].insert(load->offset);
                }
            }
            else if (auto store = dynamic_cast<StoreImm*>(*it)) {
                if (load_with_imm.count({store->dst->id, store->offset})) 
                    load_with_imm.erase({store->dst->id, store->offset});
            }
            else if (may_effect_load(*it))
                load_with_imm.clear();



            if (!used) {
                auto load = dynamic_cast<LoadImm*>(*it);
                load->dst->id = load_with_imm[{load->lhs->id, load->offset}];
                block->instructions.erase(it++);
            }
            else
                ++it;
        }
    }
}


void remove_redundant_move (Function* func) {
    
}

bool is_unconditional_jump(Instruction* inst) {
    if (auto branch = dynamic_cast<Branch*>(inst)) {
        if (branch->cond == Instruction::AL)
            return true;
    }
    return false;
}

void calculate_pre_succ(Function* func) {
    for (auto block : func->blocks) {
        block->predecessor.clear();
        block->successor.clear();
    }
    bool flag = true;
    while (flag) {
        for (auto block : func->blocks) {
            for (auto inst : block->instructions) {
                if (auto br = dynamic_cast<Branch*>(inst)) {
                    auto target = br->target;
                    if (block->successor.count(target))
                        block->successor[target]++;
                    else
                        block->successor[target] = 1;

                    if (target->predecessor.count(block))
                        target->predecessor[block]++;
                    else
                        target->predecessor[block] = 1;
                }
            }
        }
        flag = false;
        for (auto it = func->blocks.begin(); it != func->blocks.end();) {
            if ((*it) != func->entry && (*it)->predecessor.size() == 0) {
                flag = true;
                func->blocks.erase(it++);
            }
            else
                ++it;
        }
    }


}

void merge_block(Function* func) {
    bool flag = true;
    std::set<Block*> del;
    while (flag) {
        flag = false;
        for(auto block : func->blocks) {
            auto inst = block->instructions.rbegin();
            if(is_unconditional_jump(*inst)) {            
                auto branch = dynamic_cast<Branch*>(*inst);
                Block* target = branch->target;  
                if(target->predecessor.size() == 1){
                    flag = true;
                    block->instructions.pop_back();
                    for(auto ins : target->instructions)
                        block->instructions.emplace_back(ins);            
                }
                else if (target->predecessor.size() && target->instructions.size() <= 20) {
                    if ( target->name == block->name || target->predecessor.count(target) ) {
                        continue;
                    }
                    flag = true;
                    block->instructions.pop_back();
                    for(auto ins : target->instructions)
                        block->instructions.emplace_back(ins);                    
                }    
            }

        }


        calculate_pre_succ(func);
    }

    calculate_pre_succ(func);

}

void remove_branch(Function* func) {
    for (auto it = func->blocks.begin(); it != func->blocks.end(); ++it) {
        auto next = std::next(it);
        if (next == func->blocks.end())
            break;

        auto inst = (*it)->instructions.rbegin();
        if (is_unconditional_jump(*inst)) {
            auto br = dynamic_cast<Branch*>(*inst);
            if (br->target->name == (*next)->name)
                (*it)->instructions.pop_back();
        }
    }
}


}
}