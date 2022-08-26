#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Instructions.h"
#include <unordered_set>

namespace sysy
{
namespace ir
{
void BasicBlock::insertBeforeTerminal(Instruction* instr)
{
    auto iter = --instructions.end();
    instructions.insert(iter, instr);
    instr->reSetBasicBlock(this);
}

void BasicBlock::insertAfterPhi(Instruction* instr)
{
    auto iter = getInstructions().begin();
    while(dynamic_cast<PhiInst*>(*iter))
        iter++;
    getInstructions().insert(iter, instr);
}

BasicBlock* BasicBlock::Create(const std::string& name, bool isPreHeader) 
{
    return new BasicBlock(name, isPreHeader);
}

BasicBlock* BasicBlock::Create(const std::string& name, Function* theFunction, bool isPreHeader) 
{
    auto ptr = new BasicBlock(name, theFunction, isPreHeader);
    // TODO change it to insert the next of current point.
    theFunction->getBasicBlockList().push_back(ptr);
    return ptr;
}

void BasicBlock::BasicBlockDfs(BasicBlock* bb, std::function<bool(BasicBlock*)> func)
{
    if(func(bb))
        return;
    for(auto succ : bb->getSuccessor())
        BasicBlockDfs(succ, func);
}

void BasicBlock::BasicBlockDfs(BasicBlock* pre, BasicBlock* bb, std::function<bool(BasicBlock*, BasicBlock*)> func)
{
    if(func(pre, bb))
        return;
    for(auto succ : bb->getSuccessor())
        BasicBlockDfs(bb, succ, func);
}

void BasicBlock::BasicBlockDfsInv(BasicBlock* bb, std::function<bool(BasicBlock*)> func)
{
    if(func(bb))
        return;
    for(auto pre : bb->getPredecessor())
        BasicBlockDfsInv(pre, func);
}

void BasicBlock::BasicBlockDfsInv(BasicBlock* pre, BasicBlock* bb, std::function<bool(BasicBlock*, BasicBlock*)> func)
{
    if(func(pre, bb))
        return;
    for(auto succ : bb->getPredecessor())
        BasicBlockDfsInv(bb, succ, func);
}

// static std::unordered_set<BasicBlock*> vis;
void BasicBlock::BasicBlockDfsPost(BasicBlock* bb, std::function<bool(BasicBlock*)> cond, std::function<void(BasicBlock*)> action)
{
    if(cond(bb))
        return;
    for(auto succ : bb->getSuccessor())
        BasicBlockDfsPost(succ, cond, action);
    action(bb);
}

void BasicBlock::DomTreeDfsPost(BasicBlock* bb, std::function<bool(BasicBlock*)> cond, std::function<void(BasicBlock*)> action)
{
    if(cond(bb))
        return;
    for(auto succ : bb->getDomTree())
        DomTreeDfsPost(succ, cond, action);
    action(bb);
}

void BasicBlock::setRank(Instruction* instr, int _rank)
{
    rank[instr] = _rank;
}

void BasicBlock::rankReorder()
{
    rank.clear();
    int r = 0;
    for(auto instr : instructions)
        rank[instr] = r++;
}


} // namespace ir
    
} // namespace sysy
