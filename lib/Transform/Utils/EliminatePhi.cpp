#include "sysy/Transform/Utils/EliminatePhi.h"
#include "sysy/Transform/Utils/SimplifyCFG.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/IRDump.h"
#include "sysy/Support/Debug.h"
#include <unordered_set>
#include <vector>
#include <algorithm>

namespace sysy
{
namespace ir
{

EliminatePhi::EliminatePhi() 
{
    getFinish().push_back(new SimplifyCFG(true));
}

void UnsetSSA::run(Module* module)
{
    dumpIR(module);
    auto pm = new FunctionPassManager;
    pm->addPass(new EliminatePhi);
    for(auto func : module->__function)
        if(!func->isDeclare())
            pm->run(func);
}

bool EliminatePhi::isNeedToCut(BasicBlock* pre, BasicBlock* bb)
{
    auto firstInstr = *bb->getInstructions().begin();
    if(auto phi = dynamic_cast<PhiInst*>(firstInstr); phi && pre->getSuccessor().size() > 1)
        return true;
    return false;
}

void EliminatePhi::cutKeyEdge(BasicBlock* from, BasicBlock* to, Function* func)
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

void EliminatePhi::run(Function* func)
{
    // PrintMsg("EliminatePhi at func " + func->getName() );
    // Cut the key edge.
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
    
    for(auto bb : func->getBasicBlockList())
    {
        std::vector<PhiInst*> phiToRemove;
        for(auto instr : bb->getInstructions())
        {
            if(auto phi = dynamic_cast<PhiInst*>(instr))
            {
                for(auto [val, prBB] : phi->getPredecessors())
                    prBB->insertBeforeTerminal(new MoveInst(phi, val, prBB));
                phiToRemove.push_back(phi);
            }
        }
        for(auto phi : phiToRemove)
            bb->getInstructions().remove(phi);
    }

}


} // namespace ir
    
} // namespace sysy
