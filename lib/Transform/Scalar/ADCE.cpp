#include "sysy/Transform/Scalar/ADCE.h"
#include "sysy/Transform/Utils/VirtualExitBasicBlock.h"
#include "sysy/Transform/Utils/SimplifyCFG.h"
#include "sysy/Analysis/PostDomInfo.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/IRDump.h"
#include <unordered_set>
namespace sysy
{
namespace ir
{

ADCE::ADCE()
{
    getRelated().push_back(new ConstructExitBasicBlock);
    getRelated().push_back(new PostDomInfo);
    getFinish().push_back(new CleanExitBasicBlock);
    // getFinish().push_back(new FunctionVertify);
}

static std::unordered_set<Instruction*> AliveCode;
static std::unordered_set<BasicBlock*> AliveBB;

bool ADCE::isUsefulInstr(Instruction* instr)
{
    if(auto store = dynamic_cast<StoreInst*>(instr))
    {
        return true;
    }
    else if(auto ret = dynamic_cast<RetInst*>(instr))
    {
        return true;
    }
    else if(auto call = dynamic_cast<CallInst*>(instr); call && !call->isPureCall())
    {
        return true;
    }
    else
        return false;
}

void ADCE::findAliveCode(Instruction* instr)
{
    AliveCode.insert(instr);
    AliveBB.insert(instr->getBasicBlock());
    auto bb = instr->getBasicBlock();


    for(auto rdf : bb->getPdomFrontier())
    {
        PrintMsg("BB is Active");
        dumpBasickBlock(bb);
        PrintMsg("RDF");
        dumpBasickBlock(rdf);
        auto lastInstr = *(--rdf->getInstructions().end());
        PrintMsg(lastInstr->getSignature());
        if(!AliveCode.count(lastInstr))
            if(auto br = dynamic_cast<BrInst*>(lastInstr))
                findAliveCode(br);
    }

    for(auto operand : instr->getOprands())
        if(auto ins = dynamic_cast<Instruction*>(operand->getValue()); ins && !AliveCode.count(ins))
            findAliveCode(ins);
}

void ADCE::run(Function* func)
{
    // TODO Use the RPO order will be better.
    PrintMsg("ADCE in function" + func->getName());
    AliveCode.clear();
    std::unordered_set<BasicBlock*> vis;
    BasicBlock::BasicBlockDfs(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);

        for(auto instr : bb->getInstructions())
            if(isUsefulInstr(instr))
            {
                findAliveCode(instr);
            }
        return false;
    });

    std::vector<Instruction*> DeadCode;
    for(auto bb : func->getBasicBlockList())
        for(auto instr : bb->getInstructions())
            if(!AliveCode.count(instr))
                DeadCode.push_back(instr);

    // dumpFunction(func);
    
    PrintMsg("Erase all the dead code.");
    for(auto instr : DeadCode)
    {
        if(auto br = dynamic_cast<BrInst*>(instr))
        {
            PrintMsg(br->getSignature());
            auto bb = br->getBasicBlock()->getIpdom();
            while(!AliveBB.count(bb))
                bb = bb->getIpdom();
            if(br->getCond())
            {
                SimplifyCFG::deleteEdge(br->getBasicBlock(), br->getTrueBlock());
                SimplifyCFG::deleteEdge(br->getBasicBlock(), br->getFalseBlock()); 
            }
            else
            {
                auto target = br->getTrueBlock();
                br->getBasicBlock()->getSuccessor().clear();

                target->getPredecessor().erase(
                    std::find(target->getPredecessor().begin(), target->getPredecessor().end(), br->getBasicBlock())
                );
            }
            br->setUnconditional(bb);
            br->getBasicBlock()->getSuccessor().push_back(bb);
            bb->getPredecessor().push_back(br->getBasicBlock());
            continue;
        }
        instr->removeAllUseWith();
        instr->UnUseAll();
        instr->getBasicBlock()->getInstructions().remove(instr);
    }
    // dumpFunction(func);
}

} // namespace ir
    
} // namespace sysy
