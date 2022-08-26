#include "sysy/Transform/Utils/MemorySSA.h"
#include "sysy/Analysis/AliasAnalysis.h"
#include "sysy/Analysis/DomInfo.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/IRDump.h"
#include "sysy/Support/debug.h"
#include <queue>
#include <algorithm>
namespace sysy
{
namespace ir
{

MemorySSA::MemorySSA()
{
    getRelated().push_back(new DomInfo);
}

static std::unordered_map<Instruction*, LoadInst*> globalValueDefUse;
static std::unordered_map<LoadInst*, Instruction*> globalValueUseDef;
static std::unordered_map<GlobalValue*, GVInfo> globalValueInfo;

void MemorySSA::analysisScalarGV(GlobalValue* gv)
{
    auto& gvInfo = globalValueInfo[gv];

    auto less = [](Instruction* x, Instruction* y) -> bool {
        return x->getRank() < y->getRank();
    };

    for(auto [bb, storeSet] : gvInfo.get())
    {
        bb->rankReorder();
        auto loadIter = gvInfo.getLoad(bb).rbegin();
        for(auto storeIter = storeSet.rbegin(); storeIter != storeSet.rend(); storeIter++)
        {
            while(loadIter != gvInfo.getLoad(bb).rend() && less(*storeIter, *loadIter))
            {
                globalValueUseDef[*loadIter] = *storeIter;
                loadIter++;
            }
        }
    }

    for(auto bb : gvInfo.getUseBlocks())
    {
        bb->rankReorder();
        auto loadSet = gvInfo.getLoad(bb);
        auto storeIter = gvInfo.getStore(bb).rbegin();
        for(auto loadIter = loadSet.rbegin(); loadIter != loadSet.rend(); loadIter++)
        {
            while(storeIter != gvInfo.getStore(bb).rend() && less(*loadIter, *storeIter))
            {
                globalValueDefUse[*storeIter] = *loadIter;
                storeIter++;
            }
        }
    }
}

void MemorySSA::computeScalarGVInfo(Function* func)
{
    for(auto bb : func->getBasicBlockList())
    {
        bb->rankReorder();
        for(auto instr : bb->getInstructions())
        {
            if(auto load = dynamic_cast<LoadInst*>(instr))
            {
                auto ptr = load->getPtr();
                if(auto gv = dynamic_cast<GlobalValue*>(ptr); gv && !gv->getValueType()->isArrayTy())
                {
                    if(!globalValueInfo.count(gv))
                    {
                        // PrintMsg(gv->getSignature());
                        GVInfo gvInfo;
                        globalValueInfo[gv] = gvInfo;
                    }
                    auto& gvInfo = globalValueInfo[gv];
                    gvInfo.getUseBlocks().insert(bb);
                    gvInfo.getBlock().insert(bb);
                    gvInfo.getLoad(bb).insert(load);
                }
            }
        }
    }

    for(auto& [gv, gvInfo] : globalValueInfo)
    {
        for(auto bb : func->getBasicBlockList())
            for(auto instr : bb->getInstructions())
            {
                if(auto store = dynamic_cast<StoreInst*>(instr); store 
                    && gv == store->getPtr())
                {
                    store->hasAlias = true;
                    gvInfo.getDefBlocks().insert(bb);
                    gvInfo.getBlock().insert(bb);
                    gvInfo.getStore(bb).insert(store);
                }
                else if(auto call = dynamic_cast<CallInst*>(instr))
                {
                    auto callHasGV = [](GlobalValue* gv, CallInst* call) -> bool {
                        auto callee = call->getCallee();
                        return callee->getStoreGV().count(gv);
                    };
                    if(call->getCallee()->hasSideEffect() && callHasGV(gv, call))
                    {
                        call->hasAlias = true;
                        gvInfo.getDefBlocks().insert(bb);
                        gvInfo.getBlock().insert(bb);
                        gvInfo.getStore(bb).insert(call);
                    }
                }
            }
    }
}

void MemorySSA::queueStoreMemPhi(GlobalValue* gv, BasicBlock* father)
{
    auto phi = new MemPhiInst(Type::getVoidTy(), father, gv->getName() + ".memphi");
    father->insertFront(phi);
    globalValueInfo[gv].setStorePhi(father, phi);
}

void MemorySSA::queueLoadMemPhi(GlobalValue* gv, BasicBlock* father)
{
    auto phi = new MemPhiInst(Type::getVoidTy(), father, gv->getName() + ".memphi");
    father->insertFront(phi);
    globalValueInfo[gv].setLoadPhi(father, phi);
}


std::unordered_map<BasicBlock*, std::unordered_map<GlobalValue*, Value*>> IncomingGVStoreInBB;
void MemorySSA::renameLoadOfGVInBB(GlobalValue* gv, BasicBlock* bb, BasicBlock* pred)
{
    Value* val = (pred) ? IncomingGVStoreInBB[pred][gv] : UndefValue::Create();
    if(auto phi = globalValueInfo[gv].getStorePhi(bb))
        val = phi;
    for(auto load : globalValueInfo[gv].getLoad(bb))
    {
        if(!globalValueUseDef.count(load))
        {
            load->setStore(val);
        }
        else
        {
            auto store = globalValueUseDef[load];
            load->setStore(store);
        }
    }

    for(auto store : globalValueInfo[gv].getStore(bb))
        val = store;

    IncomingGVStoreInBB[bb][gv] = val;
}

std::unordered_map<BasicBlock*, std::unordered_map<GlobalValue*, Value*>> IncomingGVLoadInBB;
void MemorySSA::renameStoreOfGVInBB(GlobalValue* gv, BasicBlock* bb, BasicBlock* pred)
{
    Value* val = (pred) ? IncomingGVLoadInBB[pred][gv] : UndefValue::Create();
    if(auto phi = globalValueInfo[gv].getLoadPhi(bb))
        val = phi;
    for(auto instr : globalValueInfo[gv].getStore(bb))
    {
        if(!globalValueDefUse.count(instr))
        {
            if(auto load = dynamic_cast<LoadInst*>(val))
            {
                auto iter = std::find(instr->getBasicBlock()->begin(), instr->getBasicBlock()->end(), instr);
                auto dead = new DeadInst(bb, load);
                instr->getBasicBlock()->insertInstr(iter, dead);
            }
        }
        else
        {
            auto iter = std::find(instr->getBasicBlock()->begin(), instr->getBasicBlock()->end(), instr);
            auto load = globalValueDefUse[instr];
            auto dead = new DeadInst(bb, load);
            instr->getBasicBlock()->insertInstr(iter, dead);
        }
    }

    for(auto load : globalValueInfo[gv].getLoad(bb))
        val = load;

    IncomingGVLoadInBB[bb][gv] = val;
}

static std::unordered_map<Instruction*, LoadInst*> arrayDefUse;
static std::unordered_map<LoadInst*, Instruction*> arrayUseDef;
static std::unordered_map<Value*, ArrayInfo> info;

void MemorySSA::analysisArray(Value* array)
{
    auto& arrInfo = info[array];

    auto less = [](Instruction* x, Instruction* y) -> bool {
        return x->getRank() < y->getRank();
    };

    for(auto [bb, storeSet] : arrInfo.get())
    {
        bb->rankReorder();
        auto loadIter = arrInfo.getLoad(bb).rbegin();
        for(auto storeIter = storeSet.rbegin(); storeIter != storeSet.rend(); storeIter++)
        {
            while(loadIter != arrInfo.getLoad(bb).rend() && less(*storeIter, *loadIter))
            {
                arrayUseDef[*loadIter] = *storeIter;
                loadIter++;
            }
        }
    }

    for(auto bb : arrInfo.getUseBlocks())
    {
        bb->rankReorder();
        auto loadSet = arrInfo.getLoad(bb);
        auto storeIter = arrInfo.getStore(bb).rbegin();
        for(auto loadIter = loadSet.rbegin(); loadIter != loadSet.rend(); loadIter++)
        {
            while(storeIter != arrInfo.getStore(bb).rend() && less(*loadIter, *storeIter))
            {
                arrayDefUse[*storeIter] = *loadIter;
                storeIter++;
            }
        }
    }
}

void MemorySSA::computeArrayInfo(Function* func)
{
    for(auto bb : func->getBasicBlockList())
    {
        bb->rankReorder();
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
                if(!info.count(array))
                {
                    ArrayInfo arrInfo;
                    info[array] = arrInfo;
                }
                auto& arrInfo = info[array];
                arrInfo.getUseBlocks().insert(bb);
                arrInfo.getBlock().insert(bb);
                arrInfo.getLoad(bb).insert(load);
            }
        }
    }

    for(auto& [array, arrInfo] : info)
    {
        for(auto bb : func->getBasicBlockList())
            for(auto instr : bb->getInstructions())
            {
                if(auto store = dynamic_cast<StoreInst*>(instr))
                {
                    // FIXME there is mix use of the alloca of arg and arg
                    if(store->getValue()->getType()->isPointerTy())
                        continue;
                    if(alias(array, getOriginArray(store->getPtr())))
                    {
                        store->hasAlias = true;
                        arrInfo.getDefBlocks().insert(bb);
                        arrInfo.getBlock().insert(bb);
                        arrInfo.getStore(bb).insert(store);
                    }
                }
                else if(auto call = dynamic_cast<CallInst*>(instr))
                {
                    if(call->getCallee()->hasSideEffect() && aliasAtCall(array, call))
                    {
                        call->hasAlias = true;
                        arrInfo.getDefBlocks().insert(bb);
                        arrInfo.getBlock().insert(bb);
                        arrInfo.getStore(bb).insert(call);
                    }
                }
            }
    }
}

void MemorySSA::queueStoreMemPhi(Value* array, BasicBlock* father)
{
    auto phi = new MemPhiInst(Type::getVoidTy(), father, array->getName() + ".memphi");
    father->insertFront(phi);
    info[array].setStorePhi(father, phi);
}

void MemorySSA::queueLoadMemPhi(Value* array, BasicBlock* father)
{
    auto phi = new MemPhiInst(Type::getVoidTy(), father, array->getName() + ".memphi");
    father->insertFront(phi);
    info[array].setLoadPhi(father, phi);
}

std::unordered_map<BasicBlock*, std::unordered_map<Value*, Value*>> IncomingStoreInBB;
void MemorySSA::renameLoadOfArrayInBB(Value* array, BasicBlock* bb, BasicBlock* pred)
{
    // PrintMsg("variable " + array->getName());
    // dumpBasickBlock(bb);
    Value* val = (pred) ? IncomingStoreInBB[pred][array] : UndefValue::Create();
    if(auto phi = info[array].getStorePhi(bb))
        val = phi;
    for(auto load : info[array].getLoad(bb))
    {
        // PrintMsg("\tload" + load->getSignature() + " ");
        if(!arrayUseDef.count(load))
        {
            load->setStore(val);

        }
        else
        {
            auto store = arrayUseDef[load];
            load->setStore(store);
        }
    }

    for(auto store : info[array].getStore(bb))
        val = store;

    // dumpBasickBlock(bb);
    IncomingStoreInBB[bb][array] = val;
}

std::unordered_map<LoadInst*, LoadInst*> LoadCoverLoad;

std::unordered_map<BasicBlock*, std::unordered_map<Value*, Value*>> IncomingLoadInBB;
void MemorySSA::renameStoreOfArrayInBB(Value* array, BasicBlock* bb, BasicBlock* pred)
{
    // dumpBasickBlock(bb);
    Value* val = (pred) ? IncomingLoadInBB[pred][array] : UndefValue::Create();
    if(auto phi = info[array].getLoadPhi(bb))
        val = phi;

    auto tmp = val;
    for(auto load : info[array].getLoad(bb))
    {
        if(auto earlierLoad = dynamic_cast<LoadInst*>(tmp))
            LoadCoverLoad[load] = earlierLoad;
        tmp = load;
    }

    for(auto instr : info[array].getStore(bb))
    {
        if(!arrayDefUse.count(instr))
        {
            if(auto load = dynamic_cast<LoadInst*>(val))
            {
                auto iter = std::find(instr->getBasicBlock()->begin(), instr->getBasicBlock()->end(), instr);
                auto dead = new DeadInst(bb, load);
                instr->getBasicBlock()->insertInstr(iter, dead);
            }
        }
        else
        {
            auto iter = std::find(instr->getBasicBlock()->begin(), instr->getBasicBlock()->end(), instr);
            auto load = arrayDefUse[instr];
            auto dead = new DeadInst(bb, load);
            instr->getBasicBlock()->insertInstr(iter, dead);

            while(LoadCoverLoad.count(load))
            {
                // PrintMsg(load->getSignature());
                load = LoadCoverLoad[load];
                auto dead = new DeadInst(bb, load);
                instr->getBasicBlock()->insertInstr(iter, dead);
            }
        }
    }

    for(auto load : info[array].getLoad(bb))
        val = load;

    IncomingLoadInBB[bb][array] = val;
}

void MemorySSA::DefUseCompute(Function* func)
{
    PrintMsg("Insert the phi instructions(MemSSA-def-use)");
    for(auto& [array, arrInfo] : info)
    {
        std::set<BasicBlock*> vis;
        std::queue<BasicBlock*> WorkList;
        for(auto bb : arrInfo.getDefBlocks())
            WorkList.push(bb);
        while(!WorkList.empty())
        {
            auto bb = WorkList.front();
            WorkList.pop();
            for(auto tar : bb->getDomFrontier())
                if(!vis.count(tar))
                {
                    vis.insert(tar);
                    queueStoreMemPhi(array, tar);
                    if(!info[array].getDefBlocks().count(tar))
                        WorkList.push(tar);
                }
        }
    }
    // dumpFunction(func);

    // Rename the Variable at A Sparse Method
    PrintMsg("Rename the variable(MemSSA-def-use)");
    std::unordered_set<BasicBlock*> vis;
    auto entry = func->getEntryBlock();
    for(auto& [array, arrInfo] : info)
        IncomingStoreInBB[entry][array] = UndefValue::Create();
    LoadCoverLoad.clear();
    std::stack<std::pair<BasicBlock*, BasicBlock*>> WorkStk;
    WorkStk.push(std::make_pair(nullptr, entry));
    while (!WorkStk.empty())
    {
        auto pred = WorkStk.top().first;
        auto bb = WorkStk.top().second;
        WorkStk.pop();
        for(auto& [array, arrInfo] : info)
            if(auto phi = info[array].getStorePhi(bb))
                phi->setInComingValue(IncomingStoreInBB[pred][array], pred);

        if(!vis.count(bb))
        {
            vis.insert(bb);
            for(auto& [array, arrInfo] : info)
                renameLoadOfArrayInBB(array, bb, pred);
            for(auto riter = bb->getSuccessor().rbegin(); riter != bb->getSuccessor().rend(); riter++)
                WorkStk.push(std::make_pair(bb, *riter));
        }
        // dumpBasickBlock(bb);
    }

    PrintMsg("Insert the phi instructions(MemSSA-def-use)GV");
    for(auto& [gv, gvInfo] : globalValueInfo)
    {
        std::set<BasicBlock*> vis;
        std::queue<BasicBlock*> WorkList;
        for(auto bb : gvInfo.getDefBlocks())
            WorkList.push(bb);
        while(!WorkList.empty())
        {
            auto bb = WorkList.front();
            WorkList.pop();
            for(auto tar : bb->getDomFrontier())
                if(!vis.count(tar))
                {
                    vis.insert(tar);
                    queueStoreMemPhi(gv, tar);
                    if(!globalValueInfo[gv].getDefBlocks().count(tar))
                        WorkList.push(tar);
                }
        }
    }
    // dumpFunction(func);

    // Rename the Variable at A Sparse Method
    PrintMsg("Rename the variable(MemSSA-def-use)GV");
    vis.clear();
    entry = func->getEntryBlock();
    for(auto& [gv, gvInfo] : globalValueInfo)
        IncomingGVStoreInBB[entry][gv] = UndefValue::Create();

    WorkStk.push(std::make_pair(nullptr, entry));
    while (!WorkStk.empty())
    {
        auto pred = WorkStk.top().first;
        auto bb = WorkStk.top().second;
        WorkStk.pop();
        for(auto& [gv, gvInfo] : globalValueInfo)
            if(auto phi = globalValueInfo[gv].getStorePhi(bb))
                phi->setInComingValue(IncomingGVStoreInBB[pred][gv], pred);

        if(!vis.count(bb))
        {
            vis.insert(bb);
            for(auto& [gv, gvInfo] : globalValueInfo)
                renameLoadOfGVInBB(gv, bb, pred);
            for(auto riter = bb->getSuccessor().rbegin(); riter != bb->getSuccessor().rend(); riter++)
                WorkStk.push(std::make_pair(bb, *riter));
        }
        // dumpBasickBlock(bb);
    }
}

void MemorySSA::UseDefCompute(Function* func)
{
    PrintMsg("Insert the phi instructions(MemSSA-use-def)");
    for(auto& [array, arrInfo] : info)
    {
        std::set<BasicBlock*> vis;
        std::queue<BasicBlock*> WorkList;
        for(auto bb : arrInfo.getUseBlocks())
            WorkList.push(bb);
        while(!WorkList.empty())
        {
            auto bb = WorkList.front();
            WorkList.pop();
            for(auto tar : bb->getDomFrontier())
                if(!vis.count(tar))
                {
                    vis.insert(tar);
                    queueLoadMemPhi(array, tar);
                    if(!info[array].getDefBlocks().count(tar))
                        WorkList.push(tar);
                }
        }
    }
    // dumpFunction(func);

    // Rename the Variable at a Sparse Method
    PrintMsg("Rename the variable(MemSSA-use-def)");
    std::unordered_set<BasicBlock*> vis;
    auto entry = func->getEntryBlock();
    for(auto& [array, arrInfo] : info)
        IncomingLoadInBB[entry][array] = UndefValue::Create();
    std::stack<std::pair<BasicBlock*, BasicBlock*>> WorkStk;
    WorkStk.push(std::make_pair(nullptr, entry));
    while (!WorkStk.empty())
    {
        auto pred = WorkStk.top().first;
        auto bb = WorkStk.top().second;
        WorkStk.pop();
        for(auto& [array, arrInfo] : info)
            if(auto phi = info[array].getLoadPhi(bb))
                phi->setInComingValue(IncomingLoadInBB[pred][array], pred);

        if(!vis.count(bb))
        {
            vis.insert(bb);
            for(auto& [array, arrInfo] : info)
                renameStoreOfArrayInBB(array, bb, pred);
            for(auto riter = bb->getSuccessor().rbegin(); riter != bb->getSuccessor().rend(); riter++)
                WorkStk.push(std::make_pair(bb, *riter));
        }
        // dumpBasickBlock(bb);
    }


    PrintMsg("Insert the phi instructions(MemSSA-use-def)GV");
    for(auto& [gv, gvInfo] : globalValueInfo)
    {
        std::set<BasicBlock*> vis;
        std::queue<BasicBlock*> WorkList;
        for(auto bb : gvInfo.getUseBlocks())
            WorkList.push(bb);
        while(!WorkList.empty())
        {
            auto bb = WorkList.front();
            WorkList.pop();
            for(auto tar : bb->getDomFrontier())
                if(!vis.count(tar))
                {
                    vis.insert(tar);
                    queueLoadMemPhi(gv, tar);
                    if(!globalValueInfo[gv].getDefBlocks().count(tar))
                        WorkList.push(tar);
                }
        }
    }
    // dumpFunction(func);

    // Rename the Variable at a Sparse Method
    PrintMsg("Rename the variable(MemSSA-use-def)GV");
    vis.clear();
    entry = func->getEntryBlock();
    for(auto& [gv, gvInfo] : globalValueInfo)
        IncomingGVLoadInBB[entry][gv] = UndefValue::Create();

    WorkStk.push(std::make_pair(nullptr, entry));
    while (!WorkStk.empty())
    {
        auto pred = WorkStk.top().first;
        auto bb = WorkStk.top().second;
        WorkStk.pop();
        for(auto& [gv, gvInfo] : globalValueInfo)
            if(auto phi = globalValueInfo[gv].getLoadPhi(bb))
                phi->setInComingValue(IncomingGVLoadInBB[pred][gv], pred);

        if(!vis.count(bb))
        {
            vis.insert(bb);
            for(auto& [gv, gvInfo] : globalValueInfo)
                renameStoreOfGVInBB(gv, bb, pred);
            for(auto riter = bb->getSuccessor().rbegin(); riter != bb->getSuccessor().rend(); riter++)
                WorkStk.push(std::make_pair(bb, *riter));
        }
        // dumpBasickBlock(bb);
    }
}

void MemorySSA::run(Function* func)
{
    PrintMsg("Memory SSA at " + func->getName());
    arrayDefUse.clear();
    arrayUseDef.clear();
    info.clear();

    globalValueDefUse.clear();
    globalValueUseDef.clear();
    globalValueInfo.clear();

    computeArrayInfo(func);

    for(auto& [array, arrInfo] : info)
        analysisArray(array);

    computeScalarGVInfo(func);
    for(auto& [gv, gvInfo] : globalValueInfo)
        analysisScalarGV(gv);

    DefUseCompute(func);
    UseDefCompute(func);
    // dumpFunction(func);
}

void ClearMemorySSA::run(Function* func)
{
    PrintMsg("clear Memory SSA at " + func->getName());
    for(auto& bb : func->getBasicBlockList())
    {
        std::vector<Instruction*> deadCode;
        for(auto instr : bb->getInstructions())
        {
            if(auto memPhi = dynamic_cast<MemPhiInst*>(instr))
                deadCode.push_back(memPhi);
            else if(auto dead = dynamic_cast<DeadInst*>(instr))
                deadCode.push_back(dead);
            else if(auto load = dynamic_cast<LoadInst*>(instr))
                load->removeStore();
            else if(auto store = dynamic_cast<StoreInst*>(instr))
                store->hasAlias = false;
            else if(auto call = dynamic_cast<CallInst*>(instr))
                call->hasAlias = false;
        }
        for(auto& instr : deadCode)
        {
            instr->removeAllUseWith();
            instr->UnUseAll();
            bb->getInstructions().remove(instr);
        }
    }
    // dumpFunction(func);
}


} // namespace ir
    
} // namespace sysy