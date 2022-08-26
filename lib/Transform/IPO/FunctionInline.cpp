#include "sysy/Transform/IPO/FunctionInline.h"
#include "sysy/Transform/Utils/SimplifyCFG.h"
#include "sysy/Analysis/IPAnalysis.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/IRCopy.h"
#include "sysy/IR/IRDump.h"
#include <stack>
#include <unordered_set>
namespace sysy
{
namespace ir
{

FunctionInline::FunctionInline()
{
    getRelated().push_back(new IPAnalysis);
}

void FunctionInline::inlineAtCall(CallInst* call)
{
    auto callee = call->getCallee();
    auto nowBB = call->getBasicBlock();
    auto caller = nowBB->getParent();
    PrintMsg("Inline the func " + callee->getName());
    dumpFunction(callee);

    auto retBB = BasicBlock::Create(callee->getName() + ".ret", caller);

    nowBB->rankReorder();
    std::vector<Instruction*> instrToNewBB;
    for(auto instr : nowBB->getInstructions())
        if(instr->getRank() > call->getRank())
            instrToNewBB.push_back(instr);
    
    for(auto instr : instrToNewBB)
    {
        nowBB->getInstructions().remove(instr);
        retBB->insertBack(instr);
        instr->reSetBasicBlock(retBB);
    }
    
    for(auto succ : nowBB->getSuccessor())
        SimplifyCFG::redirectEdge(nowBB, retBB, succ);

    IRCopier copier;
    auto funcCpy = copier.getFunctionCopy(callee);

    nowBB->getInstructions().pop_back();
    nowBB->insertBack(new BrInst(funcCpy->getEntryBlock(), nowBB));
    nowBB->getSuccessor().push_back(funcCpy->getEntryBlock());
    funcCpy->getEntryBlock()->getPredecessor().push_back(nowBB);

    std::unordered_set<BasicBlock*> vis;

    std::vector<std::pair<RetInst*, BasicBlock*>> retInstrs;

    BasicBlock::BasicBlockDfs(funcCpy->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        bb->setParent(caller);
        if(bb->getSuccessor().empty())
        {
            auto lastInstr = *(--bb->getInstructions().end());
            if(auto ret = dynamic_cast<RetInst*>(lastInstr))
            {
                retInstrs.push_back(std::make_pair(ret, bb));
                bb->getInstructions().pop_back();
                auto br = new BrInst(retBB, bb);
                bb->insertBack(br);
            }
        }
        return false;
    });

    for(auto [ret, bb] : retInstrs)
    {
        bb->getSuccessor().push_back(retBB);
        retBB->getPredecessor().push_back(bb);
    }

    if(!callee->getReturnType()->isVoidTy())
    {
        auto phi = new PhiInst(call->getType(), retBB, callee->getName() + ".phi");
        retBB->insertFront(phi);
        call->replaceAllUseWith(phi);
        for(auto [ret, bb] : retInstrs)
        {
            phi->setInComingValue(ret->getRet(), bb);
            ret->UnUseAll();
        }
    }

    for(int i = 0; i < funcCpy->getArgs().size(); i++)
    {
        auto realArg = call->getArgs(i);
        auto formalArg = funcCpy->getArgs().at(i).get();
        if(!formalArg->getType()->isPointerTy())
            formalArg->replaceAllUseWith(realArg);
        else
        {
            std::vector<StoreInst*> storeToRemove;
            for(auto use : formalArg->getUses())
            {
                if(auto store = dynamic_cast<StoreInst*>(use->getUser()))
                {
                    auto alloca = store->getPtr();
                    // PrintMsg(alloca->getSignature());
                    if(auto allocaInst = dynamic_cast<AllocaInst*>(alloca))
                        allocaInst->getBasicBlock()->getInstructions().remove(allocaInst);
                    std::vector<LoadInst*> loadToReplace;
                    for(auto useOfalloca : alloca->getUses())
                        if(auto load = dynamic_cast<LoadInst*>(useOfalloca->getUser()))
                            loadToReplace.push_back(load);
                    for(auto load : loadToReplace)
                    {
                        load->replaceAllUseWith(realArg);
                        load->UnUseAll();
                        load->getBasicBlock()->getInstructions().remove(load);
                    }
                    storeToRemove.push_back(store);
                }
            }
            for(auto store : storeToRemove)
            {
                store->UnUseAll();
                store->getBasicBlock()->getInstructions().remove(store);
            }
        }
    }


    call->UnUseAll();

    std::vector<AllocaInst*> allocaToRemove;
    for(auto instr : funcCpy->getEntryBlock()->getInstructions())
    {
        if(auto alloca = dynamic_cast<AllocaInst*>(instr))
        {
            allocaToRemove.push_back(alloca);
            caller->getEntryBlock()->insertFront(alloca);
            alloca->reSetBasicBlock(caller->getEntryBlock());
        }
    }

    for(auto alloca : allocaToRemove)
        funcCpy->getEntryBlock()->getInstructions().remove(alloca);


    caller->getBasicBlockList().splice(caller->getBasicBlockList().end(), funcCpy->getBasicBlockList());
}

void FunctionInline::run(Module* module)
{
    PrintMsg("Function inline");
    std::stack<Function*> functionToremove;
    for(auto func : module->__function)
    {
        if(func->getCallee().empty() && !func->isDeclare() && func->getName() != "main")
            functionToremove.push(func);
    }

    while(!functionToremove.empty())
    {
        auto func = functionToremove.top();
        functionToremove.pop();
        for(auto call : func->getCallSite())
            inlineAtCall(call);
        
        for(auto caller : func->getCaller())
            caller->getCallee().erase(func);
        
        func->getCallSite().clear();
        func->getCaller().clear();
        module->__function.remove(func);
        // TODO delete the function.

        for(auto func : module->__function)
        {
            if(func->getCallee().empty() && !func->isDeclare() && func->getName() != "main")
                functionToremove.push(func);
        }
    }
}

} // namespace ir
    
} // namespace sysy
