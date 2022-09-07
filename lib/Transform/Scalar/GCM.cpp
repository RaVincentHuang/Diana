#include "sysy/Transform/Scalar/GCM.h"
#include "sysy/Transform/Utils/MemorySSA.h"
#include "sysy/Analysis/DomInfo.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/Analysis/LoopInfo.h"
#include "sysy/Analysis/Analysis.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/IRDump.h"
#include "sysy/Support/Debug.h"


namespace sysy
{
namespace ir
{

void LocalGCM::run(Module* module)
{
    auto pm = new FunctionPassManager;
    pm->addPass(new GCM);
    for(auto func : module->__function)
        if(!func->isDeclare())
            pm->run(func);
}

GCM::GCM()
{
    getRelated().push_back(new MemorySSA);
    getRelated().push_back(new LoopInfo); // Dominfo is before the LoopInfo
    getRelated().push_back(new DomInfo);
    getFinish().push_back(new ClearMemorySSA);
    // getFinish().push_back(new FunctionVertify);
}

static std::unordered_set<Instruction*> instrSchduled;

auto canBeSchduled = [](Instruction* instr) -> bool {
    if(auto binary = dynamic_cast<BinaryInst*>(instr))
        return true;
    else if(auto load = dynamic_cast<LoadInst*>(instr))
        return true;
    else if(auto gep = dynamic_cast<GEPInst*>(instr))
        return true;
    else if(auto unary = dynamic_cast<UnaryInst*>(instr))
        return true;
    else if(auto call = dynamic_cast<CallInst*>(instr); call
                && call->isPureCall())
        return true;
    return false;
    };

void GCM::schduleEarly(Instruction* instr)
{
    if(instrSchduled.count(instr) || !canBeSchduled(instr))
        return;
    instrSchduled.insert(instr);

    auto entry = instr->getBasicBlock()->getParent()->getEntryBlock(); 

    instr->getBasicBlock()->getInstructions().remove(instr);
    entry->insertBeforeTerminal(instr);

    for(auto use : instr->getOprands())
    {
        auto operand = use->getValue();
        if(auto opInstr = dynamic_cast<Instruction*>(operand))
        {
            schduleEarly(opInstr);
            if(!instr->getBasicBlock()->getDoms().count(opInstr->getBasicBlock()))
            {
                instr->getBasicBlock()->getInstructions().remove(instr);
                opInstr->getBasicBlock()->insertBeforeTerminal(instr);
            }
        }
    }
}

void GCM::schduleLate(Instruction* instr)
{
    if(instrSchduled.count(instr) || !canBeSchduled(instr)) // the phi could in
        return;
    instrSchduled.insert(instr);

    // PrintMsg(instr->getSignature());
    // dumpBasickBlock(instr->getBasicBlock());

    BasicBlock* lca = nullptr;

    for(auto use : instr->getUses())
    {
        auto user = use->getUser();
        if(auto userInstr = dynamic_cast<Instruction*>(user))
        {
            schduleLate(userInstr);
            auto bb = userInstr->getBasicBlock();
            // dumpBasickBlock(bb);

            if(auto phi = dynamic_cast<PhiInst*>(userInstr))
            {
                for(auto [val, pred] : phi->getPredecessors())
                    if(auto income = dynamic_cast<Instruction*>(val); income
                        && income == instr)
                        lca = lca ? lcaCompute(lca, pred) : pred;
            }
            else if(auto memphi = dynamic_cast<MemPhiInst*>(userInstr))
            {
                for(auto [val, pred] : memphi->getPredecessors())
                    if(auto income = dynamic_cast<Instruction*>(val); income
                        && income == instr)
                        lca = lca ? lcaCompute(lca, pred) : pred;   
            }
            else
                lca = lca ? lcaCompute(lca, bb) : bb;
        }
    }

    // PrintMsg(instr->getSignature());
    // dumpBasickBlock(instr->getBasicBlock());

    auto func = instr->getBasicBlock()->getParent();

    auto best = lca;
    auto getBBDepth = [&func](BasicBlock* bb) -> int {
        if(auto loop = func->getLoopOfBB(bb))
            return loop->getLoopDepth();
        return 0;
    };
    auto bestDepth = getBBDepth(best);
    while(lca != instr->getBasicBlock())
    {
        lca = lca->getIdom();

        auto currDepth = getBBDepth(lca);
        if(currDepth < bestDepth)
        {
            best = lca;
            bestDepth = currDepth;
        }
    }

    instr->getBasicBlock()->getInstructions().remove(instr);
    best->insertBeforeTerminal(instr);

    best->rankReorder();

    for(auto use : instr->getUses())
    {
        auto user = use->getUser();
        if(auto userInstr = dynamic_cast<Instruction*>(user); userInstr
            && userInstr->getBasicBlock() == best && instr->getRank() > userInstr->getRank())
        {
            if(auto phi = dynamic_cast<PhiInst*>(userInstr))
                continue;
            else if(auto memphi = dynamic_cast<MemPhiInst*>(userInstr))
                continue;
            // PrintMsg(userInstr->getSignature());
            instr->getBasicBlock()->getInstructions().remove(instr);

            auto iter = std::find(userInstr->getBasicBlock()->begin(), 
                                    userInstr->getBasicBlock()->end(), 
                                    userInstr);
            
            userInstr->getBasicBlock()->insertInstr(iter, instr);
            instr->reSetBasicBlock(userInstr->getBasicBlock());
            best->rankReorder();
        }
    }
    // dumpBasickBlock(best);
}

void GCM::run(Function* func)
{
    PrintMsg("GCM at " + func->getName());
    std::vector<Instruction*> instructions;

    std::unordered_set<BasicBlock*> vis;
    BasicBlock::BasicBlockDfs(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);

        for(auto instr : bb->getInstructions())
            if(canBeSchduled(instr))
                instructions.push_back(instr);
        return false;
    });

    instrSchduled.clear();
    for(auto instr : instructions)
        schduleEarly(instr);
    instrSchduled.clear();

    PrintMsg("After schduleEarly");
    // dumpFunction(func);

    for(auto instr : instructions)
        schduleLate(instr);
    
    PrintMsg("After schduleLate");
    // dumpFunction(func);
}

} // namespace ir
} // namespace sysy
