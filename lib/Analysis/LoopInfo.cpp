#include "sysy/Analysis/LoopInfo.h"
#include "sysy/Analysis/DomInfo.h"
#include "sysy/Analysis/SCEV.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/IRDump.h"
#include <unordered_set>
namespace sysy
{
namespace ir
{

LoopInfo::LoopInfo()
{
    getRelated().push_back(new DomInfo);
}

void LoopInfo::discoverAndMapSubloop(Loop* loop, Function* func, std::stack<BasicBlock*>& BackedgeTo)
{
    while(!BackedgeTo.empty())
    {
        auto pred = BackedgeTo.top();
        BackedgeTo.pop();
        auto subLoop = func->getLoopOfBB(pred);
        if(!subLoop)
        {
            func->linkBBToLoop(pred, loop);
            if(pred == loop->getHeader())
                continue;
            for(auto predOfPred : pred->getPredecessor())
                BackedgeTo.push(predOfPred);
        }
        else
        {
            while(auto parent = subLoop->getParentLoop())
                subLoop = parent;
            if(subLoop == loop)
                continue;
            subLoop->setParentLoop(loop);
            pred = subLoop->getHeader();
            for(auto predOfPred : pred->getPredecessor())
                if(!func->getLoopOfBB(predOfPred) 
                    || func->getLoopOfBB(predOfPred) != subLoop)
                    BackedgeTo.push(predOfPred);
        }
    }
}

int LoopInfo::getLoopDepth(BasicBlock* bb)
{
    auto func = bb->getParent();
    if(auto loop = func->getLoopOfBB(bb))
        return loop->getLoopDepth();
    return 0;
}

void LoopInfo::computeExitingBlocks(Function* func)
{
    for (auto loop : func->getAllLoops()) 
    {
        for (auto bb : loop->getLoopBasicBlocks()) 
        {
            auto lastInstr = *(--bb->getInstructions().end());
            if(auto br = dynamic_cast<BrInst*>(lastInstr); br && br->getCond())
            {
                auto bb1 = br->getTrueBlock();
                auto bb2 = br->getFalseBlock();
                if(!loop->BBSet.count(bb1) || !loop->BBSet.count(bb2))
                    loop->getExitingBlocks().insert(bb);
            }
        }
    }

}

void LoopInfo::computeExitBlocks(Function* func)
{
    for (auto loop : func->getAllLoops()) 
        for (auto bb : loop->getLoopBasicBlocks()) 
            for (auto succBB : bb->getSuccessor()) 
                if (!loop->BBSet.count(succBB))
                    loop->getExitBlocks().insert(succBB);
}

void LoopInfo::computePreHeader(Function* func)
{
    for (auto loop : func->getAllLoops())
    {
        BasicBlock* preHeader = nullptr;
        for (auto pred : loop->getHeader()->getPredecessor()) 
        {
            if (getLoopDepth(pred) != loop->getLoopDepth()) 
            {
                preHeader = pred;
                break;
            }
        }
        loop->setPreHeader(preHeader);
    }
}

void LoopInfo::computeLatch(Function* func)
{
    for (auto loop : func->getAllLoops())
    {
        BasicBlock* latch = nullptr;
        for (auto pred : loop->getHeader()->getPredecessor()) 
        {
            if (loop->BBSet.count(pred)) 
            {
                latch = pred;
                break;
            }
        }
        loop->setLatchBlock(latch);
    }
}

void LoopInfo::computeLoopIndVar(Function* func)
{
    for (auto loop : func->getAllLoops())
    {
        auto latch = loop->getLatchBlock();
        auto lastInstr = *(--latch->end());
        // PrintMsg(lastInstr->getSignature());
        if(auto br = dynamic_cast<BrInst*>(lastInstr))
        {
            auto icmp = dynamic_cast<BinaryInst*>(br->getCond());
            if(!icmp)
                continue;
            loop->setIndexCondInstr(icmp);
            loop->setICmpKind(icmp->getKind());

            std::function<PhiInst*(Instruction*)> getPhi = [&](Instruction* instr) -> PhiInst* {
                if(auto phi = dynamic_cast<PhiInst*>(instr))
                    return phi;
                else if(auto binary = dynamic_cast<BinaryInst*>(instr))
                {
                    if(auto defInstr = dynamic_cast<Instruction*>(binary->getLhs()); 
                        defInstr && !loop->isLoopInvariant(defInstr))
                        if(auto phi = getPhi(defInstr))
                            return phi;
                    else if(auto defInstr = dynamic_cast<Instruction*>(binary->getRhs()); 
                        defInstr && !loop->isLoopInvariant(defInstr))
                        if(auto phi = getPhi(defInstr))
                            return phi;
                }
                return nullptr;
            };
            if(loop->isLoopInvariant(icmp->getLhs()))
            {
                loop->setIndexEnd(icmp->getLhs());
                if(auto instr = dynamic_cast<Instruction*>(icmp->getRhs());
                    instr && !loop->isLoopInvariant(instr))
                {
                    auto phi = getPhi(instr);
                    if(phi)
                        loop->setIndexPhi(phi);
                }
            }
            else if(loop->isLoopInvariant(icmp->getRhs()))
            {
                // dumpBasickBlock(loop->getHeader());
                loop->setIndexEnd(icmp->getRhs());
                if(auto instr = dynamic_cast<Instruction*>(icmp->getLhs());
                    instr && !loop->isLoopInvariant(instr))
                {
                    // PrintMsg(instr->getSignature());
                    auto phi = getPhi(instr);
                    if(phi)
                        loop->setIndexPhi(phi);
                }
            }
        }
    }
}

void LoopInfo::run(Function* func)
{
    PrintMsg("Start LoopInfo");
    func->clearLinkOfBBToLoop();
    func->getAllLoops().clear();
    func->getTopLoops().clear();
    std::vector<BasicBlock*> PostOrderWorkList;
    std::unordered_set<BasicBlock*> vis;
    BasicBlock::DomTreeDfsPost(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        return false;
    }, [&](BasicBlock* bb) -> void {
        PostOrderWorkList.push_back(bb);
    });

    std::stack<BasicBlock*> BackedgeTo;
    for(auto header : PostOrderWorkList)
    {
        for(auto pred : header->getPredecessor())
            if(pred->getDoms().count(header))
            {
                BackedgeTo.push(pred);
            }
        
        if(!BackedgeTo.empty())
        {
            auto loop = new Loop(header);
            discoverAndMapSubloop(loop, func, BackedgeTo);
        }
    }

    vis.clear();
    BasicBlock::BasicBlockDfsPost(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        return false;
    }, [&](BasicBlock* bb) -> void {
        auto subLoop = func->getLoopOfBB(bb);
        if (subLoop && bb == subLoop->getHeader()) 
        {
            if (subLoop->getParentLoop())
                subLoop->getParentLoop()->getSubLoops().push_back(subLoop);
            else
            {
                func->getTopLoops().push_back(subLoop);
                subLoop->setLoopDepth(1); // topLoop is 1
            }

            // reverse but no header
            std::reverse(subLoop->getLoopBasicBlocks().begin() + 1, subLoop->getLoopBasicBlocks().end());
            std::reverse(subLoop->getSubLoops().begin(), subLoop->getSubLoops().end());
            subLoop = subLoop->getParentLoop();
        }

        for(; subLoop; subLoop = subLoop->getParentLoop())
        {
            subLoop->getLoopBasicBlocks().push_back(bb);
            subLoop->BBSet.insert(bb);
        }
    });

    std::stack<Loop*, std::vector<Loop*>> loopStk(func->getTopLoops());
    while (!loopStk.empty())
    {
        auto loop = loopStk.top();
        loopStk.pop();
        func->getAllLoops().push_back(loop);
        for(auto subLoop : loop->getSubLoops())
        {
            subLoop->setLoopDepth(loop->getLoopDepth() + 1);
            loopStk.push(subLoop);
        }
    }

    

    computeExitingBlocks(func);
    computeExitBlocks(func);
    computeLatch(func);
    computePreHeader(func);
    computeLoopIndVar(func);

    // dumpFunction(func);
    // auto scev = new ScalarEvolution;
    // scev->run(func);
    // int cnt = 0;
    // for(auto loop : func->getAllLoops())
    // {
    //     PrintMsg("Loop: " + std::to_string(cnt++));
    //     PrintMsg("-->Header");
    //     dumpBasickBlock(loop->getHeader());
    //     PrintMsg("-->ExitingBlocks:");
    //     for(auto bb : loop->getExitingBlocks())
    //         dumpBasickBlock(bb);
    //     PrintMsg("-->ExitBlocks:");
    //     for(auto bb : loop->getExitBlocks())
    //         dumpBasickBlock(bb);
    //     PrintMsg("-->PreHeader");
    //     dumpBasickBlock(loop->getPreHeader());
    //     PrintMsg("-->Latch");
    //     dumpBasickBlock(loop->getLatchBlock());

    //     for(auto [instr, scev] : loop->getSCEV())
    //     {
    //         PrintMsg(instr->getSignature() + " SCEV: " + scev.getSignature());
    //     }
    // }

}

} // namespace ir
    
} // namespace sysy
