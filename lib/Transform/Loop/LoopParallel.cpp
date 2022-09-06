#include "sysy/Transform/Loop/LoopParallel.h"
#include "sysy/Transform/Utils/LCSSA.h"
#include "sysy/Analysis/LoopInfo.h"
#include "sysy/Analysis/SCEV.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/Analysis/AliasAnalysis.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/IRCopy.h"
#include "sysy/IR/IRDump.h"
#include "sysy/IR/Module.h"
namespace sysy
{
namespace ir
{

LoopParallel::LoopParallel()
{
    getRelated().push_back(new LoopInfo);
    getRelated().push_back(new LCSSA);
    getRelated().push_back(new ScalarEvolution);
    getFinish().push_back(new ScalarEvolutionClean);
    getFinish().push_back(new FunctionVertify);
}

bool LoopParallel::isParallelizable(Loop* loop)
{
    // trivial analysis, as more strict condition, need dependence analysis.
    // 1. As for the Loop, It must be the simple for-loop, the MAXN Count of the Loop
    // 2. Do not has any non-prue function call.
    // need to be loopInvarilant.
    // 3. In the Loop, there is no alias
    // 4. For enery store, there must be a single gep.
    // 5. There is no gep for the scalar GV.
    // 6. 

    if(loop->getExitingBlocks().size() != 1 || loop->getExitBlocks().size() != 1) 
            return false;
    
    if(loop->getLatchBlock() != *loop->getExitingBlocks().begin())
            return false; // while(true) { ...if(..) break; ... } situation.
        
    auto index = loop->getIndexPhi();
    if(!index || !loop->hasSCEV(index))
        return false;
    
    if(auto loopEnd = loop->getIndexEnd(); !loopEnd || !loop->isLoopInvariant(loopEnd))
        return false;
    
    if(auto trip = loop->getTripCount(); trip && trip < 400)
        return false;

    if(auto c = dynamic_cast<ConstantInt*>(loop->getSCEV(index).at(1)); !c)
        return false;

    for(auto bb : loop->getLoopBasicBlocks())
        for(auto instr : bb->getInstructions()) // FIXME  ; call && !call->isPureCall() it will faild at sort
            if(auto call = dynamic_cast<CallInst*>(instr); call && (!call->isPureCall() || call->getCallee()->isRecursive()))
                return false;
        
    // alias analysis
    std::vector<Value*> arrays;
    std::unordered_map<Value*, GEPInst*> ArrayIndex;
    std::unordered_map<Value*, std::multiset<GEPInst*>> gepUseByStore;
    for(auto bb : loop->getLoopBasicBlocks())
        for(auto instr : bb->getInstructions())
        {
            if(auto load = dynamic_cast<LoadInst*>(instr))
            {
                auto ptr = load->getPtr();
                if(auto alloca = dynamic_cast<AllocaInst*>(ptr); alloca 
                    && !(alloca->getAllocatedType()->isArrayTy() && alloca->getAllocatedType()->isPointerTy()))
                    continue;
                if(auto gv = dynamic_cast<GlobalValue*>(ptr); gv 
                    && !gv->getValueType()->isArrayTy())
                    continue;
                auto array = getOriginArray(ptr);
                if(auto gep = dynamic_cast<GEPInst*>(ptr))
                {
                    if(ArrayIndex.count(array) && ArrayIndex[array]->getPtr() != gep->getPtr())
                        return false;
                    else
                        ArrayIndex[array] = gep;
                }
            }
            else if(auto store = dynamic_cast<StoreInst*>(instr))
            {
                auto ptr = store->getPtr();
                auto val = store->getValue();
                if(!loop->isLoopInvariant(val))
                    return false;
                if(auto alloca = dynamic_cast<AllocaInst*>(ptr); alloca 
                    && !(alloca->getAllocatedType()->isArrayTy() && alloca->getAllocatedType()->isPointerTy()))
                    continue;
                if(auto gv = dynamic_cast<GlobalValue*>(ptr); gv 
                    && !gv->getValueType()->isArrayTy())
                    return false;
                auto array = getOriginArray(ptr);
                if(auto gep = dynamic_cast<GEPInst*>(ptr))
                {
                    if(ArrayIndex.count(array) && ArrayIndex[array]->getPtr() != gep->getPtr())
                        return false;
                    else
                        ArrayIndex[array] = gep;

                    gepUseByStore[array].insert(gep);
                }

            }
        }
    
    // gep analysis
    for(auto& [array, getSet] : gepUseByStore)
        if(getSet.size() > 1)
            return false;
    
    for(auto instr : (*loop->getExitBlocks().begin())->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
            return false;
        else
            break;
    }

    return true;
}

void LoopParallel::runOnLoop(Loop* loop)
{
    if(!isParallelizable(loop))
        return;

    auto& scev = loop->getSCEV(loop->getIndexPhi());
    
    auto preHeader = loop->getPreHeader();

    auto start = scev.at(0);
    auto end = loop->getIndexEnd();
    auto step = scev.at(1);

    auto __thread_create = Module::getInstance().getFunction("__thread_create");
    auto tid = new CallInst(__thread_create, {}, preHeader, "tid");
    preHeader->insertBeforeTerminal(tid);

    // end - start
    auto subtmp = new BinaryInst(Type::getInt32Ty(), Instruction::Sub, end, start, preHeader, "subtmp");
    preHeader->insertBeforeTerminal(subtmp);

    // (end -start) / 4
    auto divtmp = new BinaryInst(Type::getInt32Ty(), Instruction::SDiv, subtmp, ConstantInt::get(32, 4), preHeader, "divtmp");
    preHeader->insertBeforeTerminal(divtmp);

    // 
    auto multmp = new BinaryInst(Type::getInt32Ty(), Instruction::Mul, tid, divtmp, preHeader, "multmp");
    preHeader->insertBeforeTerminal(multmp);

    auto localStart = new BinaryInst(Type::getInt32Ty(), Instruction::Add, start, multmp, preHeader, "localStart");
    preHeader->insertBeforeTerminal(localStart);

    auto addtmp = new BinaryInst(Type::getInt32Ty(), Instruction::Add, tid, ConstantInt::get(32, 1), preHeader, "addtmp");
    preHeader->insertBeforeTerminal(addtmp);

    auto multmp1 = new BinaryInst(Type::getInt32Ty(), Instruction::Mul, addtmp, divtmp, preHeader, "multmp");
    preHeader->insertBeforeTerminal(multmp1);

    auto localEnd = new BinaryInst(Type::getInt32Ty(), Instruction::Add, start, multmp1, preHeader, "localEnd");
    preHeader->insertBeforeTerminal(localEnd);

    auto indPhi = loop->getIndexPhi();

    for(auto [val, pred] : indPhi->getPredecessors())
    {
        if(pred == preHeader)
        {
            indPhi->removeIncomingByBB(preHeader);
            indPhi->setInComingValue(localStart, preHeader);
            break;
        }
    }

    auto indCmp = loop->getIndexCondInstr();
    if(indCmp->getOprands().at(0)->getValue() == end)
        indCmp->replaceOperandWith(0, localEnd);
    else
        indCmp->replaceOperandWith(1, localEnd);

    auto exit = *loop->getExitBlocks().begin();

    auto __thread_join = Module::getInstance().getFunction("__thread_join");
    auto join = new CallInst(__thread_join, { tid }, exit);
    exit->insertAfterPhi(join);
}

void LoopParallel::run(Function* func)
{
    PrintMsg("LoopParallel at function" + func->getName());

    if(func->isRecursive())
        return;

    for(auto loop : func->getTopLoops())
        runOnLoop(loop);
}

} // namespace ir
    
} // namespace sysy
