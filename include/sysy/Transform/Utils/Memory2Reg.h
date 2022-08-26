#ifndef SYSY_TRANSFORM_MEMORY2REG_H
#define SYSY_TRANSFORM_MEMORY2REG_H

#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/Support/iterator_range.h"
#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace sysy
{

namespace ir
{

class SSASet : public ModulePass
{
public:
    std::string getName() override { return "SSASet"; }
    void run(Module* mod) override;
};

bool less(LoadInst* load, StoreInst* store);

bool less(StoreInst* store, LoadInst* load);
// need dominfo
class Memory2Reg : public FunctionPass
{
private:
    bool isAllocaPromotable(AllocaInst* alloca);
    bool allocaSimplify(AllocaInst* alloca);
    void analysisAlloca(AllocaInst* alloca);
    void computeLiveIn(AllocaInst* alloca, std::set<BasicBlock*>& liveInBasicBlock);
    void renameAllocaInBB(AllocaInst* alloca, BasicBlock* bb, BasicBlock* pred);
    void queuePhi(AllocaInst* alloca, BasicBlock* father);
public:
    Memory2Reg();
    std::string getName() override { return "mem2reg"; }
    void run(Function* func) override;
};

struct cmps
{
    bool operator()(const StoreInst* x, const StoreInst* y) const { return x->getBasicBlock()->getRank(x) < y->getBasicBlock()->getRank(y); }
};

struct cmpl
{
    bool operator()(const LoadInst* x, const LoadInst* y) const { return x->getBasicBlock()->getRank(x) < y->getBasicBlock()->getRank(y); }
};

class AllocaInfo
{
private:
    std::set<BasicBlock*> defBlocks;
    std::set<BasicBlock*> useBlocks;
    std::set<BasicBlock*> block; 
    std::unordered_map<BasicBlock*, std::set<StoreInst*, cmps>> store;
    std::unordered_map<BasicBlock*, std::set<LoadInst*, cmpl>> load;
    std::unordered_map<BasicBlock*, PhiInst*> phi;
public:
    size_t storeSize() 
    { 
        size_t res = 0;
        for(auto s : store)
            res += s.second.size();
        return res;
    }

    std::unordered_set<LoadInst*> unLockLoad;
    StoreInst* getOnlyStore();

    using iterator = std::unordered_map<BasicBlock*, std::set<StoreInst*, cmps>>::iterator;
    iterator_range<iterator> get() { return make_range(store.begin(), store.end()); } 

    std::set<BasicBlock*>& getBlock() { return block; }
    std::set<BasicBlock*>& getDefBlocks() { return defBlocks; }
    std::set<BasicBlock*>& getUseBlocks() { return useBlocks; }

    std::set<StoreInst*, cmps>& getStore(BasicBlock* bb) { return store[bb]; }
    std::set<LoadInst*, cmpl>& getLoad(BasicBlock* bb) { return load[bb]; }
    void setPhi(BasicBlock* bb, PhiInst* _phi) { phi[bb] = _phi; }
    PhiInst* getPhi(BasicBlock* bb) { return phi.count(bb) ? phi[bb] : nullptr; }
    size_t phiSize() { return phi.size(); }
};

} // namespace ir


} // namespace sysy


#endif