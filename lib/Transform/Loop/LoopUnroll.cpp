#include "sysy/Transform/Loop/LoopUnroll.h"
#include "sysy/Transform/Utils/LCSSA.h"
#include "sysy/Analysis/LoopInfo.h"
#include "sysy/Analysis/SCEV.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/IRCopy.h"
#include "sysy/IR/IRDump.h"
#include <queue>
namespace sysy
{
namespace ir
{

constexpr int maxTripCount = 80;

LoopUnrollAll::LoopUnrollAll()
{
    getRelated().push_back(new LoopInfo);
    getRelated().push_back(new LCSSA);
    getRelated().push_back(new ScalarEvolution);
    getFinish().push_back(new ScalarEvolutionClean);
    // getFinish().push_back(new FunctionVertify);
}

void LoopUnrollAll::runOnLoop(Loop* loop)
{
    auto legalLoop = [&](Loop* loop) -> bool {
        // 1. Loop can not have break. At our simplify loop continue become 
        // to the common for loop
        if(loop->getExitingBlocks().size() != 1 || loop->getExitBlocks().size() != 1) 
            return false;                           // It must to be 1 at any time.
        
        if(loop->getLatchBlock() != *loop->getExitingBlocks().begin())
            return false; // while(true) { ...if(..) break; ... } situation.
        
        // 2. Loop have the constant tirp and it is smaller than the limit.
        if(auto trip = loop->getTripCount(); !trip)
            return false;
        else if(trip > maxTripCount)
            return false;
        
        // 3. Do not have any function call.
        for(auto bb : loop->getLoopBasicBlocks())
            for(auto instr : bb->getInstructions())
                if(auto call = dynamic_cast<CallInst*>(instr))
                    return false;
        return true;
    };

    if(!legalLoop(loop))
        return;
    
    auto header = loop->getHeader();
    auto latch = loop->getLatchBlock();
    auto exit = *loop->getExitBlocks().begin();

    // PrintMsg("header");
    // dumpBasickBlock(header);
    // PrintMsg("latch");
    // dumpBasickBlock(latch);
    // PrintMsg("exit");
    // dumpBasickBlock(exit);

    //    |-----                a <- a0
    //  a <- phi(a0, a1)        header
    //  header |                  |  
    //    |    |  -->           a1 <- ..
    //  a0 <- ...               latch
    //  latch  |                  |
    //    |    |                a2 <- a1
    //  exit ---                header'
    //                            |
    //                          exit

    std::unordered_map<PhiInst*, Value*> CurrentIncomingValue;
    std::vector<PhiInst*> headerPhi;
    for(auto instr : header->getInstructions())
        if(auto phi  = dynamic_cast<PhiInst*>(instr))
        {
            headerPhi.push_back(phi);
            for(auto [val, pred] : phi->getPredecessors())
                if(pred == latch)
                    CurrentIncomingValue[phi] = val;
        }
        else
            break;

    auto latchCondBr = *(--latch->getInstructions().end());
    latch->getInstructions().remove(latchCondBr);
    latchCondBr->UnUseAll();
    latch->getSuccessor().clear();

    std::vector<std::pair<BasicBlock*, BasicBlock*>> latchHeaderEdge;
    BasicBlock* preLatch = latch;
    BasicBlock* nowHeader = nullptr;

    auto trip = loop->getTripCount();

    std::unordered_map<Value*, Value*> copyMap;
    for(int i = 1; i < trip; i++)
    {
        IRCopier copyier;
        copyMap.clear();
        auto loopCpy = copyier.getLoopCopy(loop, copyMap, CurrentIncomingValue);

        // PrintMsg("Loop copy " + std::to_string(i));
        // for(auto bb : loopCpy->getLoopBasicBlocks())
        //     dumpBasickBlock(bb);

        auto headerCpy = loopCpy->getHeader();
        auto latchCpy = loopCpy->getLatchBlock();

        nowHeader = headerCpy;
        latchHeaderEdge.push_back(std::make_pair(preLatch, nowHeader));
        preLatch = latchCpy;
    }

    latchHeaderEdge.push_back(std::make_pair(preLatch, exit));

    for(auto instr : exit->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            Value* incomingVal = nullptr;
            for(auto [val, pred] : phi->getPredecessors())
                if(pred == latch)
                    incomingVal = val;
            if(!latch)
                continue;
            phi->removeIncomingByBB(latch);
            phi->setInComingValue(copyMap[incomingVal], latch);
        }
        else
            break;
    }

    for(auto phi : headerPhi)
    {
        for(auto [val, pred] : phi->getPredecessors())
        {
            if(pred == loop->getPreHeader())
            {
                phi->replaceAllUseWith(val);
                phi->UnUseAll();
                phi->getBasicBlock()->getInstructions().remove(phi);
                break;
            }
        }
    }

    exit->getPredecessor().erase(
        std::find(exit->getPredecessor().begin(), exit->getPredecessor().end(), latch)
    );

    for(auto [latch, header] : latchHeaderEdge)
    {
        latch->getSuccessor().push_back(header);
        header->getPredecessor().push_back(latch);

        auto br = new BrInstruction(header, latch);
        latch->insertBack(br);
    }

    header->getPredecessor().erase(
        std::find(header->getPredecessor().begin(), header->getPredecessor().end(), latch)
    );
}

void LoopUnrollAll::run(Function* func)
{
    PrintMsg("LoopUnrollAll");
    // dumpFunction(func);
    std::queue<Loop*> QueWorkList;
    for(auto loop : func->getTopLoops())
        QueWorkList.push(loop);

    while(!QueWorkList.empty())
    {
        auto loop = QueWorkList.front();
        QueWorkList.pop();
        runOnLoop(loop);
        for(auto subLoop : loop->getSubLoops())
            QueWorkList.push(subLoop);
    }

    // dumpFunction(func);
}

} // namespace ir
    
} // namespace sysy
