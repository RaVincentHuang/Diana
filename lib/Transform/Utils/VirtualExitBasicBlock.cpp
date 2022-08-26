#include "sysy/Transform/Utils/VirtualExitBasicBlock.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/IRDump.h"

namespace sysy
{
namespace ir
{

void ConstructExitBasicBlock::run(Function* func)
{
    PrintMsg("ConstructExitBasicBlock");
    BasicBlock* exit = func->getExitBlock();
    if(!exit)
    {
        exit = BasicBlock::Create("virtual.exit", func);
        func->setExitBlock(exit);
    }

    for(auto bb : func->getBasicBlockList())
    {
        auto lastInstr = *(--bb->getInstructions().end());
        if(auto ret = dynamic_cast<RetInst*>(lastInstr))
        {
            bb->getSuccessor().push_back(exit);
            exit->getPredecessor().push_back(bb);
        }
    }

    dumpFunction(func);

}

void CleanExitBasicBlock::run(Function* func)
{
    auto exit = func->getExitBlock();
    for(auto pred : exit->getPredecessor())
    {
        dumpBasickBlock(pred);
        auto lastInstr = *(--pred->getInstructions().end());
        if(auto ret = dynamic_cast<RetInst*>(lastInstr))
        {
            pred->getSuccessor().erase(
                std::find(pred->getSuccessor().begin(), pred->getSuccessor().end(), exit)
            );

            exit->getPredecessor().erase(
                std::find(exit->getPredecessor().begin(), exit->getPredecessor().end(), pred)
            );
        }
    }

    if(!exit->getPredecessor().empty())
    {
        if(func->getReturnType()->isIntegerTy())
            exit->insertBack(new RetInst(ConstantInt::get(32, 0), exit));
        else if(func->getReturnType()->isFloatTy())
            exit->insertBack(new RetInst(ConstantFP::get(0), exit));
        else
            exit->insertBack(new RetInst(exit));
    }
    else
    {
        func->setExitBlock(nullptr);
        func->getBasicBlockList().remove(exit);
        delete exit;
    }

    dumpFunction(func);
    // delete exit;
}

} // namespace ir
    
} // namespace sysy
