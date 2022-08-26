#ifndef SYSY_TRANSFORM_MEMORYSSA_H
#define SYSY_TRANSFORM_MEMORYSSA_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/GlobalValue.h"
#include <unordered_set>
#include <unordered_map>
#include <set>
namespace sysy
{
namespace ir
{

class ArrayInfo
{
private:
    struct cmps
    {
        bool operator()(const Instruction* x, const Instruction* y) const 
        { return x->getBasicBlock()->getRank(x) < y->getBasicBlock()->getRank(y); }
    };

    struct cmpl
    {
        bool operator()(const LoadInst* x, const LoadInst* y) const 
        { return x->getBasicBlock()->getRank(x) < y->getBasicBlock()->getRank(y); }
    };
    std::set<BasicBlock*> defBlocks;
    std::set<BasicBlock*> useBlocks;
    std::set<BasicBlock*> block; 
    std::unordered_map<BasicBlock*, std::set<Instruction*, cmps>> store;
    std::unordered_map<BasicBlock*, std::set<LoadInst*, cmpl>> load;
    std::unordered_map<BasicBlock*, MemPhiInst*> storePhi;
    std::unordered_map<BasicBlock*, MemPhiInst*> loadPhi;
public:
    size_t storeSize() 
    { 
        size_t res = 0;
        for(auto s : store)
            res += s.second.size();
        return res;
    }
    Instruction* getOnlyStore();

    using iterator = std::unordered_map<BasicBlock*, std::set<Instruction*, cmps>>::iterator;
    iterator_range<iterator> get() { return make_range(store.begin(), store.end()); } 
    
    std::set<BasicBlock*>& getBlock() { return block; }
    std::set<BasicBlock*>& getDefBlocks() { return defBlocks; }
    std::set<BasicBlock*>& getUseBlocks() { return useBlocks; }

    std::set<Instruction*, cmps>& getStore(BasicBlock* bb) { return store[bb]; }
    std::set<LoadInst*, cmpl>& getLoad(BasicBlock* bb) { return load[bb]; }
    void setStorePhi(BasicBlock* bb, MemPhiInst* _phi) { storePhi[bb] = _phi; }
    MemPhiInst* getStorePhi(BasicBlock* bb) { return storePhi.count(bb) ? storePhi[bb] : nullptr; }
    size_t storephiSize() { return storePhi.size(); }
    void setLoadPhi(BasicBlock* bb, MemPhiInst* _phi) { loadPhi[bb] = _phi; }
    MemPhiInst* getLoadPhi(BasicBlock* bb) { return loadPhi.count(bb) ? loadPhi[bb] : nullptr; }
    size_t loadPhiSize() { return loadPhi.size(); }
};

class GVInfo
{
private:
    struct cmps
    {
        bool operator()(const Instruction* x, const Instruction* y) const 
        { return x->getBasicBlock()->getRank(x) < y->getBasicBlock()->getRank(y); }
    };

    struct cmpl
    {
        bool operator()(const LoadInst* x, const LoadInst* y) const 
        { return x->getBasicBlock()->getRank(x) < y->getBasicBlock()->getRank(y); }
    };
    std::set<BasicBlock*> defBlocks;
    std::set<BasicBlock*> useBlocks;
    std::set<BasicBlock*> block; 
    std::unordered_map<BasicBlock*, std::set<Instruction*, cmps>> store;
    std::unordered_map<BasicBlock*, std::set<LoadInst*, cmpl>> load;
    std::unordered_map<BasicBlock*, MemPhiInst*> storePhi;
    std::unordered_map<BasicBlock*, MemPhiInst*> loadPhi;
public:
    size_t storeSize() 
    { 
        size_t res = 0;
        for(auto s : store)
            res += s.second.size();
        return res;
    }

    using iterator = std::unordered_map<BasicBlock*, std::set<Instruction*, cmps>>::iterator;
    iterator_range<iterator> get() { return make_range(store.begin(), store.end()); } 
    
    std::set<BasicBlock*>& getBlock() { return block; }
    std::set<BasicBlock*>& getDefBlocks() { return defBlocks; }
    std::set<BasicBlock*>& getUseBlocks() { return useBlocks; }

    std::set<Instruction*, cmps>& getStore(BasicBlock* bb) { return store[bb]; }
    std::set<LoadInst*, cmpl>& getLoad(BasicBlock* bb) { return load[bb]; }
    void setStorePhi(BasicBlock* bb, MemPhiInst* _phi) { storePhi[bb] = _phi; }
    MemPhiInst* getStorePhi(BasicBlock* bb) { return storePhi.count(bb) ? storePhi[bb] : nullptr; }
    size_t storephiSize() { return storePhi.size(); }
    void setLoadPhi(BasicBlock* bb, MemPhiInst* _phi) { loadPhi[bb] = _phi; }
    MemPhiInst* getLoadPhi(BasicBlock* bb) { return loadPhi.count(bb) ? loadPhi[bb] : nullptr; }
    size_t loadPhiSize() { return loadPhi.size(); }
};

class MemorySSA : public FunctionPass
{
private:
    void analysisScalarGV(GlobalValue* gv);
    void computeScalarGVInfo(Function* func);
    void queueStoreMemPhi(GlobalValue* gv, BasicBlock* father);
    void queueLoadMemPhi(GlobalValue* gv, BasicBlock* father);
    void renameLoadOfGVInBB(GlobalValue* gv, BasicBlock* bb, BasicBlock* pred);
    void renameStoreOfGVInBB(GlobalValue* gv, BasicBlock* bb, BasicBlock* pred);

    void analysisArray(Value* array);
    void computeArrayInfo(Function* func);
    void queueStoreMemPhi(Value* array, BasicBlock* father);
    void queueLoadMemPhi(Value* array, BasicBlock* father);
    void renameLoadOfArrayInBB(Value* array, BasicBlock* bb, BasicBlock* pred);
    void renameStoreOfArrayInBB(Value* array, BasicBlock* bb, BasicBlock* pred);

    void UseDefCompute(Function* func);
    void DefUseCompute(Function* func);
public:
    MemorySSA();
    void run(Function* func) override;
    std::string getName() { return "Memory SSA"; }
};

class ClearMemorySSA : public FunctionPass
{
public:
    void run(Function* func) override;
    std::string getName() { return "Clear Memory SSA"; }
};

} // namespace ir
    
} // namespace sysy


#endif