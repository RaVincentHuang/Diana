#ifndef SYSY_TARGET_BLOCK_H
#define SYSY_TARGET_BLOCK_H
#include "sysy/IR/Instructions.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/Target/Instruction.h"
#include <list>
#include <set>

namespace sysy 
{
namespace mc 
{

class Function;

class Block {
public:
    std::string name;
    Function* father;
    std::list<Instruction*> instructions;
    std::map<Block*, int> successor;
    std::map<Block*, int> predecessor;
    std::set<Reg*> live_out;
    std::set<Reg*> live_use;
    std::set<Reg*> live_def;
    std::set<Reg*> live_in;
    std::set<Reg*> live_out_v;
    std::set<Reg*> live_use_v;
    std::set<Reg*> live_def_v;
    std::set<Reg*> live_in_v;

    static int num;
    int loop_depth;

    std::map<int, Reg*> const_int_map;
    std::map<int, Reg*> const_float_map;
    std::map<std::pair<ir::Value*, ir::Value*>, Reg*> vv_div_map;
    std::map<std::pair<int, ir::Value*>, Reg*> cv_div_map;
    std::map<std::pair<ir::Value*, int>, Reg*> vc_div_map;

    std::map<std::pair<ir::Value*, int>, Reg*> gep_factor_map; // 只会统计乘法，加法会直接并入

    Block(std::string _name, Function* _father, int _loop_depth) : name(_name), father(_father), loop_depth(_loop_depth) {}

    void instSelection(ir::BasicBlock* block);
    void insertFront(std::vector<Instruction*> inst_list);
    void insertBack(std::vector<Instruction*> inst_list);
    void setSuccessor(std::vector<ir::BasicBlock*>);
    void setPredecessor(std::vector<ir::BasicBlock*>);
    std::string gen_asm();
};


} // namespace mc 
} // namespace sysy


#endif