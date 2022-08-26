#include "sysy/Target/Module.h"
#include "sysy/Analysis/LoopInfo.h"
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/IRDump.h"
#include <string>
#include <map>
#include <queue>
#include <functional>

namespace sysy 
{
namespace mc 
{

extern int REGISTER_USAGE[16];

bool Function::analyseImm(int x) {
    uint t = (uint)x;
    if (t <= 0xffu) 
        return true;
    for (int i = 1; i < 16; i++) {
        uint x = (t << 2*i) | (t  >> (32-2*i) );
        if (x <= 0xffu) 
            return true;
      }
    return false;
}


Reg* Function::get_reg(int value, Block* block) {
    if (block->const_int_map.count(value))
        return block->const_int_map[value];

    Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
    block->const_int_map[value] = new_reg;
    block->instructions.push_back(new MoveImm(new_reg, value));
    return new_reg;
}

Reg* Function::get_reg(float value, Block* block) {
    // Reg* new_reg = new Reg(vreg_num++, Reg::VIRTUAL, Reg::VREG);

    // B(value);
    int t = Function::getFloatInt(value);
    // B(t);
    if (block->const_float_map.count(t)) 
        return block->const_float_map[t];

    auto it = father->float_map.find(t);
    if (it != father->float_map.end()) {
        // B("find");
        Reg* address;
        if (float_address_map.count(it->second->name))
            address = float_address_map[it->second->name];
        else {
            address = new Reg(reg_num++, Reg::VIRTUAL);
            move_global_list.push_back(new LoadGlobal(address, it->second->name));
            float_address_map[it->second->name] = address;
        }
     
        Reg* dst = new Reg(vreg_num++, Reg::VIRTUAL, Reg::VREG);
        block->instructions.push_back(new LoadImm(LoadImm::VLOAD, dst, address, 0));
        block->const_float_map[t] = dst;

        return dst;
    }
    else {
        // B("not find");
        GlobalObject* new_global = new GlobalObject(GlobalObject::getName(), true);
        father->data_flag = true;
        new_global->is_zero = false;
        new_global->values.push_back(t);


        father->float_map[t] = new_global;
        father->global_objects.push_back(new_global);

        Reg* address = new Reg(reg_num++, Reg::VIRTUAL);
        move_global_list.push_back(new LoadGlobal(address, new_global->name));
        float_address_map[new_global->name] = address;

        Reg* dst = new Reg(vreg_num++, Reg::VIRTUAL, Reg::VREG);
        block->instructions.push_back(new LoadImm(LoadImm::VLOAD, dst, address, 0));
        block->const_float_map[t] = dst;
        
        return dst;
    }
}

Reg* Function::get_reg(ir::Value* value, Block* block) {
    if (auto arg = dynamic_cast<ir::Function::Arg*>(value)) {
        // std::cout << "here value is arg\n";
        auto it = value_map.find(arg);
        if (it != value_map.end()) {
            // std::cout << "find\n";
            return it->second;
        }
        else {
            assert(0);
            // std::cout << "not find\n";
            // int idx = arg_idx[arg];
            // // std::cout << "arg idx is " << idx << "\n";
            // if (idx <= 3) {
            //     assert(0);
            //     Reg* new_reg = new Reg(idx, Reg::PHY);
            //     value_map[value] = new_reg;
            //     return new_reg;
            // }
            // else {
            //     Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
            //     Reg* sp = new Reg(13, Reg::PHY);
            //     int offset = (idx-4)*4 + stack_size;
            //     Load* load = new Load(new_reg, sp, offset, false);
            //     // stack_object_offset[load] = -(idx-4)*4;
            //     arg_offset[load] = -(idx-4)*4;
            //     block->instructions.push_back(load);
            //     value_map[value] = new_reg;
            //     return new_reg;
            // }
        }
    } 
    else if (auto gv = dynamic_cast<ir::GlobalValue*>(value)) {
        // std::cout << "here value is global value\n";
        auto it = value_map.find(gv);
        if (it != value_map.end())
            return it->second;
        else {
            Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
            block->instructions.push_back(new LoadGlobal(new_reg, father->global_map[gv]->name));
            value_map[value] = new_reg;
            return new_reg;
        }
    }
    else if (auto constant = dynamic_cast<ir::ConstantInt*>(value)) {
        return get_reg(constant->getValue(), block);
    }
    else if (auto constant = dynamic_cast<ir::ConstantFP*>(value)) {
        return get_reg(constant->getValue(), block);
    }
    else if (auto alloc = dynamic_cast<ir::AllocaInst*>(value)) {
        // std::cout << "here value is alloc value\n";
        auto it = value_map.find(alloc);
        if (it != value_map.end()) 
            return it->second;
        else {
            assert(0);
            // Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
            // Reg* sp = new Reg(13, Reg::PHY);
            // auto alloc_address = new BinaryImm(BinaryImm::ADD, new_reg, sp, stack_size - local_value_offset[alloc]);
            // block->instructions.push_back(alloc_address);
            // stack_object_offset[alloc_address] = local_value_offset[alloc];

            // return new_reg;
        }
    }
    else {
        // std::cout << "here value is normal value\n";
        auto it = value_map.find(value);
        if (it != value_map.end()) 
            return it->second;
        else if (value->getType()->isFloatTy()) {
            Reg* new_reg = new Reg(vreg_num++, Reg::VIRTUAL, Reg::VREG);
            value_map[value] = new_reg;
            return new_reg;
        }   
        else {
            Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
            value_map[value] = new_reg;
            return new_reg;
        }
    }
}    


void Function::recompute_offset() {
    for (auto i : arg_offset) {
        if (auto load = dynamic_cast<LoadImm*>(i.first)) {
            load->offset = push_reg_num*4 + align8 + stack_size + (sp-1) - i.second;
        }
        else
            assert(0);
    }
    for (auto i : alloc_offset) {
        if (auto move = dynamic_cast<MoveImm*>(i.first)) {
            move->lhs = stack_size + (sp-1) - i.second;
        }
        else if (auto add = dynamic_cast<BinaryImm*>(i.first)) {
            add->rhs = stack_size + (sp-1) - i.second;
        }
        else
            assert(0);
    }
    for (auto i : gep_list) {
        if (auto add = dynamic_cast<BinaryImm*>(i.first)) {
            add->rhs = stack_size + (sp-1) - i.second;
            assert(Function::analyseImm(add->rhs));
        }
        else if (auto store = dynamic_cast<StoreImm*>(i.first)) {
            store->offset = stack_size + (sp-1) - i.second;
        }
        else if (auto load = dynamic_cast<LoadImm*>(i.first)) {
            load->offset = stack_size + (sp-1) - i.second;
        }
        else if (auto mov = dynamic_cast<MoveImm*>(i.first)) {
            mov->lhs = stack_size + (sp-1) - i.second;
        }
        else
            assert(0);
    }
}

int Function::nearestLegalImm(int x) {
    if (analyseImm(x) || x < 256)
        return x;

    int i = 31;
    while ( !( ( (1 << i) & x) >> i ) ) 
        i--;

    int result = 0xff;
    if (i % 2) 
        result = result << (i-7);
    else 
        result = result << (i-6);
    return x & result;
}

int Function::getFloatInt(float x) {
    int t = 1; //用来按位与操作
    int *f = (int*)(&x);//将float的解释成int，即float的地址转成int*
    int res = 0;
    for(int i = 0; i < 32; i++) {
        //从最高位开始按位与，如果为1，则bin[i]=1，如果为0，则bin[i]=0
        //这里没有将bin存成字符，而是数字1 0
        res = res | (*f)&(t<<31-i);
    }
   return res;
}

void Function::splitSP() {
    // B(name);
    Reg* sp_reg = new Reg(13, Reg::PHY);

    if ( (push_reg_num*4 + stack_size + sp-1) & 4 ) 
        align8 = 4;
    int x = stack_size + sp-1 + align8;

    int t = nearestLegalImm(x);
    while (t) {
        sub_sp_list.push_back(new BinaryImm(BinaryImm::SUB, sp_reg, sp_reg, t));
        add_sp_list.push_back(new BinaryImm(BinaryImm::ADD, sp_reg, sp_reg, t));
        x -= t;
        t = nearestLegalImm(x);
    }
    if (blocks.size()) {
        entry->insertFront(sub_sp_list);
        for (auto block : return_blocks) {
            block->insertBack(add_sp_list);
        }
    }
}

void Function::analyseFunc(ir::Function *func) {
    int idx = 0;
    for (auto& arg : func->getArgs()) {
        rev_arg_idx[idx] = arg.get();
        arg_idx[arg.get()] = idx++;
    }

    for (auto i : func->getBasicBlockList()) {
        std::string name = i->getName() + "__" + std::to_string(Block::num++);
        
        int loop_depth;
        if(auto loop = func->getLoopOfBB(i))
            loop_depth = loop->getLoopDepth();
        else
            loop_depth = 0;

        Block* block = new Block(name, this, loop_depth);
        blocks.push_back(block);
        block_map.insert({i, block});
    }

    move_arg_list = {};
    for (int j = 0; j < arg_num; j++) {
        ir::Value* arg = rev_arg_idx[j];
        if (j <= 3) {
            if (arg->getType()->isFloatTy()) {
                Reg* new_reg = new Reg(vreg_num++, Reg::VIRTUAL, Reg::VREG);
                value_map[arg] = new_reg;
                move_arg_list.push_back(new MoveReg(MoveReg::SVMOV, new_reg, new Reg(j, Reg::PHY)));
            }
            else {
                Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
                value_map[arg] = new_reg;
                move_arg_list.push_back(new MoveReg(MoveReg::MOV, new_reg, new Reg(j, Reg::PHY)));
            }
        }
        else {
            Reg* sp = new Reg(13, Reg::PHY);
            int offset = (j-4)*4;
            if (arg->getType()->isFloatTy()) {
                Reg* new_reg = new Reg(vreg_num++, Reg::VIRTUAL, Reg::VREG);
                LoadImm* load = new LoadImm(LoadImm::VLOAD, new_reg, sp, offset);
                arg_offset[load] = -(j-4)*4;
                value_map[arg] = new_reg;
                move_arg_list.insert(move_arg_list.begin(), load);
            }
            else {
                Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
                LoadImm* load = new LoadImm(LoadImm::SLOAD, new_reg, sp, offset);
                arg_offset[load] = -(j-4)*4;
                value_map[arg] = new_reg;
                move_arg_list.insert(move_arg_list.begin(), load);
            }
        }
    }


    move_global_list = {};
    for (auto gb : father->global_objects) {
        if (gb->is_float)
            continue;
        Reg* new_reg = new Reg(reg_num++, Reg::VIRTUAL);
        value_map[father->rev_global_map[gb]] = new_reg;
        move_global_list.push_back(new LoadGlobal(new_reg, gb->name));
    }

    auto fpm = new ir::FunctionPassManager;
    fpm->addPass(new ir::LoopInfo);
    fpm->run(func);


    std::function<void(ir::BasicBlock*)> dfs = [&](ir::BasicBlock* bb) -> void {
        // do something
        // ir::dumpBasickBlock(bb);
        block_map[bb]->instSelection(bb);
        block_map[bb]->setPredecessor(bb->getPredecessor());
        block_map[bb]->setSuccessor(bb->getSuccessor());

        for(auto succ : bb->getDomTree())
            dfs(succ);
    };



    dfs(func->getEntryBlock());

    entry = block_map[func->getEntryBlock()];
    entry->insertFront(move_global_list);
    entry->insertFront(move_arg_list);
    entry->insertFront(alloc_list);

}

std::vector<int> Function::reg_allocate(RegAllocStat *stat) {
    stat->spill_cnt = 0;
    while (true) {
      Regallocator allocator(this);
    //   std::cout <<"finish reg allocate2\n";
      std::vector<int> ret = allocator.run(stat);
      if (stat->succeed) return ret;
    }
}

void Function::reg_alloc(){
    RegAllocStat stat;
    std::vector<int> reg_alloc;
    if(vreg_num==0) stat.ifonly=true;
    reg_alloc = reg_allocate(&stat);
    int32_t spill_size = stat.spill_cnt*4;
    std::vector<Reg*> save_regs;
    std::vector<Reg*> save_regs_v;
    bool used[16] = {};
    bool used_v[32] = {};
    // std::cout<<"ans:  ";
    // int k=0;
    // for (int i : reg_alloc){
    //     std::cout<<k<<":"<<i<<" ";
    //     k++;
    // }
    // std::cout<<"\n";
    
    for (int i = 0; i < reg_num; ++i)
        if (reg_alloc[i] >= 0) used[reg_alloc[i]] = true;
    for (int i = 0; i < 16; ++i)
        if (REGISTER_USAGE[i] == 2 && used[i])
            save_regs.emplace_back(new Reg(i,Reg::PHY,Reg::SREG));
    for (int i = reg_num; i < reg_num+vreg_num; ++i)
        if (reg_alloc[i] >= 0) used_v[reg_alloc[i]] = true;
    for (int i = 0; i < 32; ++i)
        if ( used_v[i])
            save_regs_v.emplace_back(new Reg(i,Reg::PHY,Reg::VREG));
    
    
    std::vector<Reg*> regs;
    for (size_t i = 0; i < save_regs.size(); ++i) {
        if(save_regs[i]->id!=14 && REGISTER_USAGE[save_regs[i]->id] == 2){
            regs.push_back(save_regs[i]);
            //stack_size+=4;
            push_reg_num++;
        }
        
    }
    // std::cout<<"here1\n";
    regs.push_back(new Reg(14,Reg::PHY,Reg::SREG));
    push_reg_num++;
    auto position=this->blocks.front()->instructions.begin();
    this->blocks.front()->instructions.insert(position,new Push(Push::SPUSH ,regs));
    
    std::vector<Reg*> regs_v;
    // for (size_t i = 0; i < save_regs_v.size(); ++i) {
       
    //         regs_v.push_back(save_regs_v[i]);
    //         push_reg_num++;
        
        
    // }
    for (size_t i = 0; i < 16; ++i) {
        if(i < save_regs_v.size()){
            regs_v.push_back(save_regs_v[i]);
            push_reg_num++;
        }
        
    }
    std::vector<Reg*> regs_v2;
    for (size_t i = 16; i < save_regs_v.size(); ++i) {
        regs_v2.push_back(save_regs_v[i]);
        //stack_size+=4;
        push_reg_num++;
    }
    if(regs_v.size())
        this->blocks.front()->instructions.insert(position,new Push(Push::VPUSH ,regs_v));
    if(regs_v2.size())
        this->blocks.front()->instructions.insert(position,new Push(Push::VPUSH ,regs_v2));

    for (auto &block : this->blocks){
        for (auto i = block->instructions.begin(); i != block->instructions.end();i++) {
            if(Return * ifff = dynamic_cast<Return *>(*i)){
                    if(regs_v2.size())
                        block->instructions.insert(i,new Pop(Pop::VPOP, regs_v2));
                    if(regs_v.size())
                        block->instructions.insert(i,new Pop(Pop::VPOP, regs_v));
                    block->instructions.insert(i,new Pop(Pop::SPOP, regs));
                break; 
            }
        }
    }  
    
    for (auto &block : this->blocks){
        for (auto i = block->instructions.begin(); i != block->instructions.end();i++) {
            if(BinaryReg * ifff = dynamic_cast<BinaryReg *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
                if(ifff->rhs->type==Reg::VIRTUAL) {
                    if(ifff->rhs->kind==Reg::SREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id];
                        ifff->rhs->type=Reg::PHY;
                    }
                    if(ifff->rhs->kind==Reg::VREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id+reg_num];
                        ifff->rhs->type=Reg::PHY;
                    }
                }
            }else if(BinaryImm * ifff = dynamic_cast<BinaryImm *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    ifff->dst->id=reg_alloc[ifff->dst->id];
                    ifff->dst->type=Reg::PHY;
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    ifff->lhs->id=reg_alloc[ifff->lhs->id];
                    ifff->lhs->type=Reg::PHY;
                }
            }else if(MoveReg * ifff = dynamic_cast<MoveReg *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
            }else if(ShiftInstr * ifff = dynamic_cast<ShiftInstr *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    ifff->dst->id=reg_alloc[ifff->dst->id];
                    ifff->dst->type=Reg::PHY;
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    ifff->lhs->id=reg_alloc[ifff->lhs->id];
                    ifff->lhs->type=Reg::PHY;
                }
            }else if(MoveImm * ifff = dynamic_cast<MoveImm *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    ifff->dst->id=reg_alloc[ifff->dst->id];
                    ifff->dst->type=Reg::PHY;
                }
            }else if(CompareReg * ifff = dynamic_cast<CompareReg *>(*i)){
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
                if(ifff->rhs->type==Reg::VIRTUAL) {
                    if(ifff->rhs->kind==Reg::SREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id];
                        ifff->rhs->type=Reg::PHY;
                    }
                    if(ifff->rhs->kind==Reg::VREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id+reg_num];
                        ifff->rhs->type=Reg::PHY;
                    }
                }
            }else if(CompareImm * ifff = dynamic_cast<CompareImm *>(*i)){
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    ifff->lhs->id=reg_alloc[ifff->lhs->id];
                    ifff->lhs->type=Reg::PHY;
                }
            }else if(LoadImm * ifff = dynamic_cast<LoadImm *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
            }else if(StoreImm * ifff = dynamic_cast<StoreImm *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
            }else if(LoadReg * ifff = dynamic_cast<LoadReg *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
                if(ifff->rhs->type==Reg::VIRTUAL) {
                    if(ifff->rhs->kind==Reg::SREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id];
                        ifff->rhs->type=Reg::PHY;
                    }
                    if(ifff->rhs->kind==Reg::VREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id+reg_num];
                        ifff->rhs->type=Reg::PHY;
                    }
                }
            }else if(StoreReg * ifff = dynamic_cast<StoreReg *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    if(ifff->lhs->kind==Reg::SREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id];
                        ifff->lhs->type=Reg::PHY;
                    }
                    if(ifff->lhs->kind==Reg::VREG){
                        ifff->lhs->id=reg_alloc[ifff->lhs->id+reg_num];
                        ifff->lhs->type=Reg::PHY;
                    }
                }
                if(ifff->rhs->type==Reg::VIRTUAL) {
                    if(ifff->rhs->kind==Reg::SREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id];
                        ifff->rhs->type=Reg::PHY;
                    }
                    if(ifff->rhs->kind==Reg::VREG){
                        ifff->rhs->id=reg_alloc[ifff->rhs->id+reg_num];
                        ifff->rhs->type=Reg::PHY;
                    }
                }
            }else if(LoadGlobal * ifff = dynamic_cast<LoadGlobal *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                }
            }else if(ComplexMul * ifff = dynamic_cast<ComplexMul *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    ifff->dst->id=reg_alloc[ifff->dst->id];
                    ifff->dst->type=Reg::PHY;
                }
                if(ifff->lhs->type==Reg::VIRTUAL) {
                    ifff->lhs->id=reg_alloc[ifff->lhs->id];
                    ifff->lhs->type=Reg::PHY;
                }
                if(ifff->rhs->type==Reg::VIRTUAL) {
                    ifff->rhs->id=reg_alloc[ifff->rhs->id];
                    ifff->rhs->type=Reg::PHY;
                }
                if(ifff->extra->type==Reg::VIRTUAL) {
                    ifff->extra->id=reg_alloc[ifff->extra->id];
                    ifff->extra->type=Reg::PHY;
                }
            }else if(LoadFP * ifff = dynamic_cast<LoadFP *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                    ifff->dst->type=Reg::PHY;
                }
            }
            else if(Convert * ifff = dynamic_cast<Convert *>(*i)){
                if(ifff->dst->type==Reg::VIRTUAL) {
                    if(ifff->dst->kind==Reg::SREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id];
                        ifff->dst->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->dst->id=reg_alloc[ifff->dst->id+reg_num];
                        ifff->dst->type=Reg::PHY;
                    }
                    
                }
                if(ifff->src->type==Reg::VIRTUAL) {
                    if(ifff->src->kind==Reg::SREG){
                        ifff->src->id=reg_alloc[ifff->src->id];
                        ifff->src->type=Reg::PHY;
                    }
                    if(ifff->dst->kind==Reg::VREG){
                        ifff->src->id=reg_alloc[ifff->src->id+reg_num];
                        ifff->src->type=Reg::PHY;
                    }
                }
            }
        }
    }  
    for(auto block:this->blocks){
        std::set<Instruction*> del;
        for(auto inst:block->instructions){
            if(MoveReg * ifff = dynamic_cast<MoveReg *>(inst)){
                if(ifff->dst->id == ifff->lhs->id && ifff->dst->kind == ifff->lhs->kind) del.insert(inst);
            }
        }
        for(auto delt:del){
            block->instructions.remove(delt);
        }
    }
    
}

void Function::calc_live(){
    std::deque<std::pair<Block*, Reg*>> update;
    std::deque<std::pair<Block*, Reg*>> update_v;
    for (auto &block : blocks) {
        block->live_use.clear();
        block->live_def.clear();
        block->live_use_v.clear();
        block->live_def_v.clear();
        for (auto it = block->instructions.rbegin(); it != block->instructions.rend(); ++it) {
            for (auto r : (*it)->def_reg)
            {
                if(r->kind==Reg::SREG)
                    if(r->type==Reg::VIRTUAL || allocable(r->id)){
                        int flag=0;
                        std::set<Reg*> del;
                        for(auto rr:block->live_use){
                            if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                                
                        }
                        for(auto delt:del){
                            block->live_use.erase(delt);
                        }
                        for(auto rr:block->live_def){
                            if(r->id==rr->id&&r->type==rr->type)
                                flag=1;
                        }
                        if(flag==0) block->live_def.insert(r);
                    }
                if(r->kind==Reg::VREG)
                    {
                        int flag=0;
                        std::set<Reg*> del;
                        for(auto rr:block->live_use_v){
                            if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                                
                        }
                        for(auto delt:del){
                            block->live_use_v.erase(delt);
                        }
                        for(auto rr:block->live_def_v){
                            if(r->id==rr->id&&r->type==rr->type)
                                flag=1;
                        }
                        if(flag==0) block->live_def_v.insert(r);
                    }
            }
            for (auto r : (*it)->use_reg)
            {
                if(r->kind==Reg::SREG)
                    if(r->type==Reg::VIRTUAL || allocable(r->id)){
                        int flag=0;
                        std::set<Reg*> del;
                        for(auto rr:block->live_def){
                            if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                                
                        }
                        for(auto delt:del){
                            block->live_def.erase(delt);
                        }
                        for(auto rr:block->live_use){
                            if(r->id==rr->id&&r->type==rr->type)
                                flag=1;
                        }
                        if(flag==0) block->live_use.insert(r);
                    }
                if(r->kind==Reg::VREG)
                    {
                        int flag=0;
                        std::set<Reg*> del;
                        for(auto rr:block->live_def_v){
                            if(r->id==rr->id&&r->type==rr->type)
                                del.insert(rr);
                                
                        }
                        for(auto delt:del){
                            block->live_def_v.erase(delt);
                        }
                        for(auto rr:block->live_use_v){
                            if(r->id==rr->id&&r->type==rr->type)
                                flag=1;
                        }
                        if(flag==0) block->live_use_v.insert(r);
                    }
            } 
        }
        for (auto r : block->live_use) update.emplace_back(block, r);
        for (auto r : block->live_use_v) update_v.emplace_back(block, r);
        block->live_in = block->live_use;
        block->live_out.clear();
        block->live_in_v = block->live_use_v;
        block->live_out_v.clear();
        // for(auto r:block->live_in){
        //     std::cout<<r->id<<"\n";
        // }
    }
    while (!update.empty()) {
        auto cur = update.front();
        update.pop_front();
        for (auto t : cur.first->predecessor){
            auto prev=t.first;
            if (prev->live_out.find(cur.second) == prev->live_out.end()) {
                int flag=0;
                for(auto r:prev->live_out){
                    if(r->id==cur.second->id&&r->type==cur.second->type) flag=1;
                }
                if(flag==0) {
                    prev->live_out.insert(cur.second);
                    if (prev->live_def.find(cur.second) == prev->live_def.end() && prev->live_in.find(cur.second) == prev->live_in.end()) {
                        int flag1=0;
                        for(auto r:prev->live_def){
                            if(r->id==cur.second->id&&r->type==cur.second->type) flag1=1;
                        }
                        for(auto r:prev->live_in){
                            if(r->id==cur.second->id&&r->type==cur.second->type) flag1=1;
                        }
                        if(flag==0){
                            prev->live_in.insert(cur.second);
                            update.emplace_back(prev, cur.second);
                        }
                        
                    }
                }
            }
        }
            
    }
    while (!update_v.empty()) {
        auto cur = update_v.front();
        update_v.pop_front();
        for (auto t: cur.first->predecessor){
            auto prev=t.first;
            if (prev->live_out_v.find(cur.second) == prev->live_out_v.end()) {
                int flag=0;
                for(auto r:prev->live_out_v){
                    if(r->id==cur.second->id&&r->type==cur.second->type) flag=1;
                }
                if(flag==0) {
                    prev->live_out_v.insert(cur.second);
                    if (prev->live_def_v.find(cur.second) == prev->live_def_v.end() && prev->live_in_v.find(cur.second) == prev->live_in_v.end()) {
                        int flag1=0;
                        for(auto r:prev->live_def_v){
                            if(r->id==cur.second->id&&r->type==cur.second->type) flag1=1;
                        }
                        for(auto r:prev->live_in_v){
                            if(r->id==cur.second->id&&r->type==cur.second->type) flag1=1;
                        }
                        if(flag==0){
                            prev->live_in_v.insert(cur.second);
                            update_v.emplace_back(prev, cur.second);
                        }
                        
                    }
                }
            }
        }
            
    }
    
}


std::string Function::gen_asm() {
    if (is_declared)
        return "";

    std::string code = "";
    code += name + ":\n";
    for (auto b : blocks)
        code += b->gen_asm();
    
    // std::cout << "finish blocks ASM generate\n";
   
    return code;
}

}
}