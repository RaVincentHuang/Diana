#include "sysy/Transform/Utils/SplitCriticalEdges.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include <unordered_set>
namespace sysy
{
namespace ir
{

bool SplitCriticalEdge::isNeedToCut(BasicBlock* pre, BasicBlock* bb)
{
    return pre->getSuccessor().size() > 1 && bb->getPredecessor().size() > 1;
}

void SplitCriticalEdge::cutKeyEdge(BasicBlock* from, BasicBlock* to, Function* func)
{
    auto innerBB = BasicBlock::Create("innerBB", func);
    auto terminatorInstr = *(--from->getInstructions().end());
    if(auto br = dynamic_cast<BrInst*>(terminatorInstr))
    {
        br->replaceTarget(to, innerBB);
        auto succIter = std::find(from->getSuccessor().begin(), from->getSuccessor().end(), to);
        *succIter = innerBB;
        innerBB->getPredecessor().push_back(from);
        auto instr = new BrInstruction(to, innerBB);
        innerBB->insertBack(instr);
        innerBB->getSuccessor().push_back(to);
        auto predIter = std::find(to->getPredecessor().begin(), to->getPredecessor().end(), from);
        *predIter = innerBB;

        for(auto instr : to->getInstructions())
            if(auto phi = dynamic_cast<PhiInst*>(instr))
            {
                for(int i = 0; i < phi->size(); i++)
                {
                    auto prBB = phi->getPredecessors(i).second;
                    if(prBB == from)
                        phi->setIncomingBB(i, innerBB);
                }
            }
            else
                break;
    }
    else
        ErrorMsg("bad terminator instr");
}

void SplitCriticalEdge::run(Function* func)
{
    std::unordered_set<BasicBlock*> vis;
    std::vector<std::pair<BasicBlock*, BasicBlock*>> edgeNeedToCut;
    BasicBlock::BasicBlockDfs(nullptr, func->getEntryBlock(), [&](BasicBlock* pre, BasicBlock* bb) -> bool{
        if(pre && isNeedToCut(pre, bb))
            edgeNeedToCut.push_back(std::make_pair(pre, bb));
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        return false;
    });

    for(auto [from, to] : edgeNeedToCut)
        cutKeyEdge(from, to, func);
}


} // namespace ir
    
} // namespace sysy