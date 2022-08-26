#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/Value.h"
#include "sysy/IR/IRDump.h"
#include "sysy/Analysis/DomInfo.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/Transform/Utils/Memory2Reg.h"
#include "sysy/Transform/Scalar/DCE.h"
#include "sysy/Support/debug.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <unordered_map>

namespace sysy
{
namespace ir
{

void SSASet::run(Module* mod)
{
    // PrintMsg("Runing Mem2Reg");
    auto funcPM = new FunctionPassManager;
    funcPM->addPass(new DCE);
    funcPM->addPass(new Memory2Reg);
    for(auto func : mod->__function)
        if(!func->isDeclare())
            funcPM->run(func);
}

Memory2Reg::Memory2Reg() : FunctionPass()
{
    getRelated().push_back(new DomInfo);
    // getFinish().push_back(new FunctionVertify);
}

StoreInst* AllocaInfo::getOnlyStore()
{
    for(auto s : store)
        if(!s.second.empty())
            return *s.second.begin();
}

bool Memory2Reg::isAllocaPromotable(AllocaInst* alloca)
{
    // PrintMsg("<-alloca->" + alloca->getName());
    if(alloca->getAllocatedType()->isPointerTy() || alloca->getAllocatedType()->isArrayTy())
        return false;
    for(auto ptr : alloca->getUses())
    {
        auto user = ptr.get()->getUser();
        // PrintMsg("\t" + user->getSignature());
        if(auto store = dynamic_cast<StoreInst*>(user))
        {
            if(store->getValue() == alloca || 
                store->getValue()->getType() != alloca->getAllocatedType())
                return false;
        }
        else if(auto gep = dynamic_cast<GEPInst*>(user))
            // if(!gep->hasAllZeroIndices()) // We don't use the gep.
            return false;
    }
    return true;
}

static std::unordered_map<StoreInst*, std::set<LoadInst*>> allocaDefUse;
static std::unordered_map<LoadInst*, StoreInst*> allocaUseDef;
static std::unordered_map<AllocaInst*, AllocaInfo> info;
static std::unordered_map<PhiInst*, AllocaInst*> phiCheck;

void Memory2Reg::analysisAlloca(AllocaInst* alloca)
{
    AllocaInfo allocaInfo;
    for(auto use : alloca->getUses())
    {
        auto user = use.get()->getUser();
        if(auto store = dynamic_cast<StoreInst*>(user))
        {
            allocaInfo.getStore(store->getBasicBlock()).insert(store);
            allocaInfo.getBlock().insert(store->getBasicBlock());
            allocaInfo.getDefBlocks().insert(store->getBasicBlock());
        }
        else if(auto load = dynamic_cast<LoadInst*>(user))
        {
            allocaInfo.getLoad(load->getBasicBlock()).insert(load);
            allocaInfo.getBlock().insert(load->getBasicBlock());
            allocaInfo.getUseBlocks().insert(load->getBasicBlock());
        }
    }
    for(auto [bb, storeSet] : allocaInfo.get())
    {
        bb->rankReorder();
        auto loadIter = allocaInfo.getLoad(bb).rbegin();
        for(auto storeIter = storeSet.rbegin(); storeIter != storeSet.rend(); storeIter++)
        {
            while(loadIter != allocaInfo.getLoad(bb).rend() && less(*storeIter, *loadIter))
            {
                allocaDefUse[*storeIter].insert(*loadIter);
                allocaUseDef[*loadIter] = *storeIter;
                loadIter++;
            }
        }
    }

    for(auto bb : allocaInfo.getUseBlocks())
        for(auto load : allocaInfo.getLoad(bb))
            if(!allocaUseDef.count(load))
                allocaInfo.unLockLoad.insert(load);

    info[alloca] = std::move(allocaInfo);
}

static std::vector<AllocaInst*> allocaToRemove;
bool Memory2Reg::allocaSimplify(AllocaInst* alloca)
{
    if(alloca->getUses().empty())
    {
        alloca->removeAllUseWith();
        return false;
    }
    analysisAlloca(alloca);
    auto& allocaInfo = info[alloca];
    if(allocaInfo.storeSize() == 1)
    {
        auto store = allocaInfo.getOnlyStore();
        std::vector<LoadInst*> loadToRemove;
        for(auto load : allocaDefUse[store])
        {
            // PrintMsg("load " + load->getSignature());
            allocaUseDef.erase(load);
            allocaInfo.getLoad(load->getBasicBlock()).erase(load);
            load->replaceAllUseWith(store->getValue());
            loadToRemove.push_back(load);
            load->getBasicBlock()->getInstructions().remove(load);
            load->UnUseAll();
        }
        for(auto load : loadToRemove)
            allocaDefUse[store].erase(load);

        loadToRemove.clear();
        for(auto load : allocaInfo.unLockLoad)
        {
            if(load->getBasicBlock()->getDoms().count(store->getBasicBlock()))
            {
                allocaInfo.getLoad(load->getBasicBlock()).erase(load);
                load->replaceAllUseWith(store->getValue());
                load->getBasicBlock()->getInstructions().remove(load);
                loadToRemove.push_back(load);
                load->UnUseAll();
            }
        }
        for(auto load : loadToRemove)
            allocaInfo.unLockLoad.erase(load);


        if(!allocaInfo.unLockLoad.empty())
            return true;
        allocaDefUse.erase(store);
        allocaInfo.getStore(store->getBasicBlock()).erase(store);
        store->getBasicBlock()->getInstructions().remove(store);
        store->UnUseAll();
        allocaToRemove.push_back(alloca);
        return false;
    }
    if(allocaInfo.getBlock().size() == 1)
    {
        // PrintMsg("/alloca/" + alloca->getName());
        auto bb = *allocaInfo.getBlock().begin();
        std::vector<StoreInst*> storeToRemove;
        for(auto store : allocaInfo.getStore(bb))
        {
            std::vector<LoadInst*> loadToRemove;
            for(auto load : allocaDefUse[store])
            {
                // PrintMsg("load " + load->getSignature());
                allocaUseDef.erase(load);
                allocaInfo.getLoad(load->getBasicBlock()).erase(load);
                load->replaceAllUseWith(store->getValue());
                loadToRemove.push_back(load);
                load->getBasicBlock()->getInstructions().remove(load);
                load->UnUseAll();
            }
            for(auto load : loadToRemove)
                allocaDefUse[store].erase(load);
            allocaDefUse.erase(store);
            store->getBasicBlock()->getInstructions().remove(store);
            storeToRemove.push_back(store);
            store->UnUseAll();
        }
        for(auto store : storeToRemove)
            allocaInfo.getStore(store->getBasicBlock()).erase(store);

        if(!allocaInfo.unLockLoad.empty())
            return true;
        return false;
    }
    return true;
}

void Memory2Reg::computeLiveIn(AllocaInst* alloca, std::set<BasicBlock*>& liveInBasicBlock)
{
    auto& allocaInfo = info[alloca];
    for(auto bb : allocaInfo.getUseBlocks())
    {
        if(!allocaInfo.getDefBlocks().count(bb))
        {
            liveInBasicBlock.insert(bb);
            continue;
        }
        auto load = *allocaInfo.getLoad(bb).begin();
        if(!allocaUseDef.count(load))
            liveInBasicBlock.insert(bb);
    }
    std::vector<BasicBlock*> tmp;
    std::unordered_set<BasicBlock*> vis;
    for(auto bb : liveInBasicBlock)
        BasicBlock::BasicBlockDfsInv(bb, [&](BasicBlock* bb) -> bool {
            if(vis.count(bb) || (allocaInfo.getDefBlocks().count(bb) && !liveInBasicBlock.count(bb) ))
                return true;
            vis.insert(bb);
            tmp.push_back(bb);
            return false;
        });
    liveInBasicBlock.insert(tmp.begin(), tmp.end());
}

void Memory2Reg::queuePhi(AllocaInst* alloca, BasicBlock* father)
{
    auto phi = new PhiInst(alloca->getAllocatedType(), father, alloca->getName() + ".phi");
    father->insertFront(phi);
    info[alloca].setPhi(father, phi);
    phiCheck[phi] = alloca;
}

std::unordered_map<BasicBlock*, std::unordered_map<AllocaInst*, Value*>> IncomingValsInBB;
void Memory2Reg::renameAllocaInBB(AllocaInst* alloca, BasicBlock* bb, BasicBlock* pred)
{
    // PrintMsg("variable " + alloca->getName());
    Value* val = (pred) ? IncomingValsInBB[pred][alloca] : UndefValue::Create();
    // PrintMsg("Inval is " + val->getSignature());
    if(auto phi = info[alloca].getPhi(bb))
        val = phi;
    std::vector<LoadInst*> deadLoad;
    // dumpBasickBlock(bb);
    for(auto load : info[alloca].getLoad(bb))
    {
        // PrintMsg("\tload" + load->getSignature() + " ");
        if(!allocaUseDef.count(load))
        {
            load->replaceAllUseWith(val);
            for(auto [otherAlloca, otherVal] : IncomingValsInBB[bb])
                if(otherAlloca != alloca && otherVal == load)
                    IncomingValsInBB[bb][otherAlloca] = val;
        }
        else
        {
            auto store = allocaUseDef[load];
            load->replaceAllUseWith(store->getValue());
            for(auto [otherAlloca, otherVal] : IncomingValsInBB[bb])
                if(otherAlloca != alloca && otherVal == load)
                    IncomingValsInBB[bb][otherAlloca] = store->getValue();
            allocaDefUse[store].erase(load);
        }
        allocaUseDef.erase(load);
        bb->getInstructions().remove(load);
        load->UnUseAll();
        deadLoad.push_back(load);
    }
    for(auto load : deadLoad)
        info[alloca].getLoad(bb).erase(load);

    for(auto store : info[alloca].getStore(bb))
    {
        val = store->getValue();
        if(allocaDefUse[store].empty())
        {
            bb->getInstructions().remove(store);
            store->UnUseAll();
        }
    }
    // dumpBasickBlock(bb);
    IncomingValsInBB[bb][alloca] = val;
    // PrintMsg("Outval is " + val->getSignature());
}

void Memory2Reg::run(Function* func)
{
    // PrintMsg("mem2reg func: " + func->getName() + "\n");
    allocaToRemove.clear();
    allocaDefUse.clear();
    allocaUseDef.clear();
    info.clear();
    phiCheck.clear();
    IncomingValsInBB.clear();
    // PrintMsg("Judge the alloca(Mem2reg)");
    std::vector<AllocaInst*> Allocas;
    BasicBlock* entry = func->getEntryBlock();

    // Get the rank of the all instructions in list.
    std::unordered_set<BasicBlock*> hasRank;
    BasicBlock::BasicBlockDfs(entry, [&](BasicBlock* bb) -> bool {
        if(hasRank.count(bb))
            return true;
        hasRank.insert(bb);
        bb->rankReorder();
        return false;
    });

    for(auto instr : entry->getInstructions())
        if(auto alloca = dynamic_cast<AllocaInst*>(instr))
            if(isAllocaPromotable(alloca) && allocaSimplify(alloca))
                Allocas.push_back(alloca);
    for(auto alloca : allocaToRemove)
    {
        alloca->getBasicBlock()->getInstructions().remove(alloca);
        alloca->UnUseAll();
    }
    allocaToRemove.clear();

    // dumpFunction(func);
    
    // Insert the phi instructions.
    // PrintMsg("Insert the phi instructions(Mem2Reg)");
    for(auto alloca : Allocas)
    {
        std::set<BasicBlock*> LiveInBlocks;
        computeLiveIn(alloca, LiveInBlocks);
        std::set<BasicBlock*> vis;
        std::queue<BasicBlock*> WorkList;
        for(auto bb : info[alloca].getDefBlocks())
            WorkList.push(bb);
        while(!WorkList.empty())
        {
            auto bb = WorkList.front();
            WorkList.pop();
            for(auto tar : bb->getDomFrontier())
                if(!vis.count(tar))
                {
                    vis.insert(tar);
                    if(LiveInBlocks.count(tar))
                    {
                        queuePhi(alloca, tar);
                    }
                    if(!info[alloca].getDefBlocks().count(tar))
                        WorkList.push(tar);
                }
        }
    }
    // dumpFunction(func);

    // Rename the Variable at A Sparse Method
    // PrintMsg("Rename the variable(Mem2Reg)");
    std::unordered_set<BasicBlock*> vis;
    for(auto alloca : Allocas)
        IncomingValsInBB[entry][alloca] = UndefValue::Create();
    std::stack<std::pair<BasicBlock*, BasicBlock*>> WorkStk;
    WorkStk.push(std::make_pair(nullptr, entry));
    while (!WorkStk.empty())
    {
        auto pred = WorkStk.top().first;
        auto bb = WorkStk.top().second;
        WorkStk.pop();
        for(auto alloca : Allocas)
            if(auto phi = info[alloca].getPhi(bb))
                phi->setInComingValue(IncomingValsInBB[pred][alloca], pred);

        if(!vis.count(bb))
        {
            vis.insert(bb);
            for(auto alloca : Allocas)
                renameAllocaInBB(alloca, bb, pred);
            // for(auto alloca : Allocas)
            //     PrintMsg("alloca " + alloca->getName() + " outVal is" + IncomingValsInBB[bb][alloca]->getSignature());
            for(auto riter = bb->getSuccessor().rbegin(); riter != bb->getSuccessor().rend(); riter++)
                WorkStk.push(std::make_pair(bb, *riter));
        }
        // dumpBasickBlock(bb);
    }

    for(auto alloca : Allocas)
    {
        alloca->getBasicBlock()->getInstructions().remove(alloca);
        alloca->UnUseAll();
    }    
}

bool less(LoadInst* load, StoreInst* store)
{
    return load->getBasicBlock()->getRank(load) < store->getBasicBlock()->getRank(store);
}

bool less(StoreInst* store, LoadInst* load)
{
    return store->getBasicBlock()->getRank(store) < load->getBasicBlock()->getRank(load);
}

} // namespace ir
} // namespace sysy


