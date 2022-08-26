#include "sysy/Transform/Scalar/DCE.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/IRDump.h"
#include <unordered_set>
namespace sysy
{
namespace ir
{

void LoaclDCE::run(Module* module)
{
    auto pm = new FunctionPassManager;
    // PrintMsg("Runnig DCE");
    pm->addPass(new DCE);
    for(auto func : module->__function)
        if(!func->isDeclare())
            pm->run(func);
}

static std::unordered_set<Instruction*> AliveCode;

bool DCE::isUsefulInstr(Instruction* instr)
{
    if(auto store = dynamic_cast<StoreInst*>(instr))
        return true;
    else if(auto br = dynamic_cast<BrInst*>(instr))
        return true;
    else if(auto ret = dynamic_cast<RetInst*>(instr))
        return true;
    else if(auto call = dynamic_cast<CallInst*>(instr); call && !call->isPureCall())
        return true;
    else
        return false;
}

void DCE::findAliveCode(Instruction* instr)
{
    AliveCode.insert(instr);
    for(auto operand : instr->getOprands())
        if(auto ins = dynamic_cast<Instruction*>(operand->getValue()); ins && !AliveCode.count(ins))
            findAliveCode(ins);
}

void DCE::removeUselessStore(BasicBlock* bb)
{
    for(auto iter = bb->getInstructions().begin(); iter != bb->getInstructions().end();)
    {
        if(auto store = dynamic_cast<StoreInst*>(*iter); store && store->getPtr()->getUses().size() == 1)
        {
            iter = bb->getInstructions().erase(iter);
            store->UnUseAll();
        }
        else
            ++iter;
    }

    for(auto iter = bb->getInstructions().begin(); iter != bb->getInstructions().end();)
    {
        if(auto alloca = dynamic_cast<AllocaInst*>(*iter); alloca && alloca->getUses().size() == 0)
        {
            iter = bb->getInstructions().erase(iter);
        }
        else
            ++iter;
    }    
}

void DCE::run(Function* func)
{
    // TODO Use the RPO order will be better.
    PrintMsg("DCE in function" + func->getName());
    AliveCode.clear();
    std::unordered_set<BasicBlock*> vis;
    BasicBlock::BasicBlockDfs(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);

        // removeUselessStore(bb);


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
    
    PrintMsg("Erase all the dead code.");
    for(auto instr : DeadCode)
    {
        instr->removeAllUseWith();
        instr->UnUseAll();
        instr->getBasicBlock()->getInstructions().remove(instr);
    }
    // dumpFunction(func);
}

} // namespace ir

} // namespace sysy
