#include "sysy/Analysis/IPAnalysis.h"
#include "sysy/Analysis/AliasAnalysis.h"
#include "sysy/IR/Module.h"
#include <algorithm>
#include <unordered_set>
namespace sysy
{
namespace ir
{



void IPAnalysis::run(Module* module)
{
    PrintMsg("IPA");
    for(auto func : module->__function)
    {
        if(func->isDeclare())
            continue;
        func->getCallee().clear();
        func->getCaller().clear();
        func->getLoadGV().clear();
        func->getStoreGV().clear();
        func->getCallSite().clear();
    }

    std::unordered_set<Function*> hasSideEffect;

    for(auto func : module->__function)
    {
        if(func->isDeclare())
            continue;
        for(auto bb : func->getBasicBlockList())
        {
            for(auto instr : bb->getInstructions())
            {
                if(auto load = dynamic_cast<LoadInst*>(instr))
                {
                    auto ptr = load->getPtr();
                    if(auto gep = dynamic_cast<GEPInst*>(ptr))
                    {
                        auto array = getOriginArray(gep);
                        if(auto gv = dynamic_cast<GlobalValue*>(array))
                            func->getLoadGV().insert(gv);
                    }
                    else if(auto gv = dynamic_cast<GlobalValue*>(ptr))
                        func->getLoadGV().insert(gv);
                }
                else if(auto store = dynamic_cast<StoreInst*>(instr))
                {
                    auto ptr = store->getPtr();
                    if(auto gep = dynamic_cast<GEPInst*>(ptr))
                    {
                        auto array = getOriginArray(gep);
                        if(auto gv = dynamic_cast<GlobalValue*>(array))
                            func->getStoreGV().insert(gv);
                        else if(isArgArray(array))
                            hasSideEffect.insert(func);
                    }
                    else if(auto gv = dynamic_cast<GlobalValue*>(ptr))
                        func->getStoreGV().insert(gv);
                }
                else if(auto call = dynamic_cast<CallInst*>(instr))
                {
                    auto callee = call->getCallee();
                    if(callee->isDeclare())
                    {
                        hasSideEffect.insert(func);
                        continue;
                    }
                    func->getCallee().insert(callee);
                    callee->getCaller().insert(func);
                    callee->getCallSite().push_back(call);
                    if(func == callee)
                        func->setRecursive();
                }
            }
        }
    }

    // Function call comm, A fixed point algorithm.
    bool fixed;
    do
    {
        fixed = true;
        for(auto func : module->__function)
        {
            if(func->isDeclare())
                continue;
            auto calleeSet = std::set<Function*>(func->getCallee());
            for(auto callee : calleeSet)
            {
                auto preLoadSize = func->getLoadGV().size();
                auto preStoreSize = func->getStoreGV().size();
                // func = func \/ callee
                std::set_union(func->getLoadGV().begin(), func->getLoadGV().end(),
                                callee->getLoadGV().begin(), callee->getLoadGV().end(),
                                std::inserter(func->getLoadGV(), func->getLoadGV().begin()));
                std::set_union(func->getStoreGV().begin(), func->getStoreGV().end(),
                                callee->getStoreGV().begin(), callee->getStoreGV().end(),
                                std::inserter(func->getStoreGV(), func->getStoreGV().begin()));
                
                auto nowLoadSize = func->getLoadGV().size();
                auto nowStoreSize = func->getStoreGV().size();
                if(preLoadSize != nowLoadSize || preStoreSize != nowStoreSize)
                    fixed = false;
            }
        }
    } while(!fixed);

    // sideEffect, A fixed point algorithm.
    do
    {
        fixed = true;
        for(auto func : module->__function)
        {
            if(func->isDeclare())
                continue;
            auto calleeSet = std::set<Function*>(func->getCallee());
            
            auto preSize = hasSideEffect.size();
            for(auto callee : calleeSet)
            {
                if(hasSideEffect.count(callee))
                    hasSideEffect.insert(func);
            }
            if(hasSideEffect.size() != preSize)
                fixed = false;
        }
    } while(!fixed);

    for(auto func : module->__function)
    {
        if(func->isDeclare())
            continue;
        if(func->getStoreGV().empty() && !hasSideEffect.count(func))
            func->noSideEffect();
    }

}

} // namespace ir
    
} // namespace sysy
