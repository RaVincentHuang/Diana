#include "sysy/Transform/Utils/LCSSA.h"
#include "sysy/Analysis/LoopInfo.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/IRDump.h"
namespace sysy
{
namespace ir
{

LCSSA::LCSSA()
{
    // getRelated().push_back(new LoopInfo);
    // getFinish().push_back(new FunctionVertify);
}

void LCSSA::computeDefInUseOut(Loop* loop, std::unordered_set<Instruction*>& DefInUseOut)
{
    for (auto bb : loop->getLoopBasicBlocks()) 
    {
        for (auto instr : bb->getInstructions()) 
        {
            for (auto use : instr->getUses()) 
            {
                auto user = use->getUser();
                if(auto userInstr = dynamic_cast<Instruction*>(user))
                {
                    auto userBB = userInstr->getBasicBlock();
                    if(auto phi = dynamic_cast<PhiInst*>(userInstr))
                    {
                        for(auto [val, pred] : phi->getPredecessors())
                            if(val == instr)
                                userBB = pred;
                    }
                    if (bb != userBB && !loop->BBSet.count(userBB)) 
                    {
                        // PrintMsg(instr->getSignature());
                        DefInUseOut.insert(instr);
                        break;
                    }
                }
            }
        }
    }
}

Value* LCSSA::getIncomingValAtBB(BasicBlock* bb, Instruction* instr, std::unordered_map<BasicBlock*, PhiInst*>& BBToPhi, Loop* loop)
{
    if (!bb)
        return UndefValue::Create();

    if(BBToPhi.count(bb))
        return BBToPhi[bb];

    auto idom = bb->getIdom();
    if(!loop->BBSet.count(idom))
    {
        auto value = getIncomingValAtBB(idom, instr, BBToPhi, loop);
        return value;
    }

    auto phi = new PhiInst(instr->getType(), bb, instr->getName() + ".lcphi");
    bb->insertFront(phi);
    BBToPhi[bb] = phi;

    for(auto [val, pred] : phi->getPredecessors())
        phi->setInComingValue(getIncomingValAtBB(pred, instr, BBToPhi, loop), pred);
    
    return phi;
}

void LCSSA::insertLoopClosedPhi(Loop* loop, Instruction* instr)
{
    std::unordered_map<BasicBlock*, PhiInst*> BBToPhi;
    auto bb = instr->getBasicBlock();

    for (auto exit : loop->getExitBlocks()) 
    {
        if (!BBToPhi.count(exit) && exit->getDoms().count(bb)) 
        {
            auto phi = new PhiInst(instr->getType(), exit, instr->getName() + ".lcphi");
            exit->insertFront(phi);
            BBToPhi[exit] = phi;
            for(auto pred : exit->getPredecessor())
                phi->setInComingValue(instr, pred);
        }
    }

    std::vector<Value*> users;
    for(auto use : instr->getUses())
        users.push_back(use->getUser());

    for(auto user : users)
    {
        if(auto userInstr = dynamic_cast<Instruction*>(user))
        {
            auto userBB = userInstr->getBasicBlock();
            if(auto phi = dynamic_cast<PhiInst*>(userInstr))
            {
                for(auto [val, pred] : phi->getPredecessors())
                    if(val == instr)
                        userBB = pred;
            }
            if(userBB == bb || loop->BBSet.count(userBB))
                continue;
        
            auto value = getIncomingValAtBB(userBB, instr, BBToPhi, loop);
            for(auto i = 0; i < userInstr->getOprands().size(); i++)
            {
                if(userInstr->getOprands().at(i)->getValue() == instr)
                    userInstr->replaceOperandWith(i, value);
            }
        }
    }
}

void LCSSA::runOnLoop(Loop* loop)
{
    if(!loop)
        return; // Postorder to travel the loop tree.
    for(auto subLoop : loop->getSubLoops())
        runOnLoop(subLoop);
    
    std::unordered_set<Instruction*> DefInUseOut;
    computeDefInUseOut(loop, DefInUseOut);

    if(DefInUseOut.empty())
        return;
    
    if(loop->getExitBlocks().empty())
        return;

    for(auto instr : DefInUseOut)
        insertLoopClosedPhi(loop, instr);
}

void LCSSA::run(Function* func) 
{
    PrintMsg("LCSSA in Function " + func->getName());
    for(auto loop : func->getTopLoops())
        runOnLoop(loop);
    // dumpFunction(func);
} 

} // namespace ir
    
} // namespace sysy
