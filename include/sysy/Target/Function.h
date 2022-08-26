#ifndef SYSY_TARGET_FUNCTION_H
#define SYSY_TARGET_FUNCTION_H
#include "sysy/IR/Function.h"
#include "sysy/Target/Block.h"
#include "sysy/Target/Reg.h"
#include <list>
#include <map>

namespace sysy 
{
namespace mc 
{

class Module;
struct RegAllocStat;

class Function {
public:
    enum Type { AllInt, AllFloat, IntFloat };
    Type type;

    std::string name;
    Module* father;
    Block* entry;
    std::list<Block*> blocks;
    int stack_size;
    int arg_num;
    std::map<int,int> offset;
    std::map<int,int> offset_v;
    std::map<int,Reg*> off_reg;
    int sp=1;
    int push_reg_num=0;
    bool is_declared;
    int spill_count=0;
    int reg_num = 0;
    int vreg_num = 0;

    int align8 = 0;

    std::vector<Instruction*> move_arg_list;
    std::vector<Instruction*> move_global_list;
    std::map<std::string, Reg*> float_address_map;

    std::map<Instruction*, int> alloc_offset;
    std::vector<Instruction*> alloc_list;

    std::set<int> use_regs;
    std::set<int> spilling_nodes;
    std::set<int> spilling_nodes_v;

    std::map<ir::Function::Arg*, int> arg_idx;
    std::map<int, ir::Value*> rev_arg_idx;

    std::map<ir::Value*, Reg*> value_map;
    std::map<int, Reg*> const_map;

    // 如果store指令的地址是alloc并且allocType是指针，说明函数参数是指针
    std::map<ir::AllocaInst*, Reg*> store_alloc_map;

    std::map<ir::BasicBlock*, Block*> block_map;
    std::map<ir::Value*, int> alloc_address;
    std::map<Instruction*, int> stack_object_offset;
    std::map<Instruction*, int> arg_offset;
    
    std::map<Instruction*, int> gep_list;
    std::map<ir::Value*, int> gep_map;
    std::map<ir::Value*, int> gep_with_constant;
    std::map<ir::Value*, std::pair<Reg*, int>> gep_with_shift;
    std::map<ir::Value*, Reg*> gep_with_reg;

    std::set<Block*> return_blocks;
    std::vector<Instruction*> sub_sp_list;
    std::vector<Instruction*> add_sp_list;

    void reg_alloc();
    void merge_block();
    std::vector<int> reg_allocate(RegAllocStat *stat);
    void calc_live();

    static bool analyseImm(int x);
    static int getFloatInt(float x);
    int nearestLegalImm(int x);
    void splitSP();

    Function(std::string _name, Module* _father, bool _is_declared, int _arg_num) 
        : name(_name), father(_father), is_declared(_is_declared), stack_size(0), arg_num(_arg_num) {}
    void analyseFunc(ir::Function *func);
    void recompute_offset();
    Reg* get_reg(int value, Block* block);
    Reg* get_reg(float value, Block* block);
    Reg* get_reg(ir::Value* value, Block* block);
    std::string gen_asm();
};

} // namespace mc 
} // namespace sysy


#endif