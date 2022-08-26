//===-------------- SimplifyCFG.cpp - Simplify the CFG -------------------===//
// We can simplify the CFG in the following cases.
// 1. CondBr. If the condition of a conditional branch instruction is known, 
// or the two branches are consistent, it can be replaced with an unconditional branch.
// 2. OnlyBr. If a block has only one unconditional branch, we can merge 
// it into the successor block and modify the phi function.
// 3. Merge the bb. If one block has a unique successor and the other 
// has a unique predecessor, the two basic blocks can be merged.
// 4. Upper the Branch. B1──B2─B4  ->  B1──B4   ──B2──B4  Do this operation if B2 is empty.
//                        ──└──B3       └──B3     └───B3
// 5. Remove Useless phi function. If phi has only pred, change it to its 
// only incoming value. 
//===---------------------------------------------------------------------===//


#include "sysy/Transform/Utils/SimplifyCFG.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/IRDump.h"
#include <unordered_set>

namespace sysy
{
namespace ir
{

SimplifyCFG::SimplifyCFG(bool after) : isAfterLoopOpt(after)
{
    // getFinish().push_back(new FunctionVertify);
}

void SimplifyCFG::deleteEdge(BasicBlock* from, BasicBlock* to)
{
    // dumpBasickBlock(from);
    // dumpBasickBlock(to);
    from->getSuccessor().erase(
        std::find(from->getSuccessor().begin(), from->getSuccessor().end(), to)
    );

    to->getPredecessor().erase(
        std::find(to->getPredecessor().begin(), to->getPredecessor().end(), from)
    );

    for(auto instr : to->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            phi->removeIncomingByBB(from);
        }
        else
            break;
    }
}

void SimplifyCFG::redirectEdge(BasicBlock* preFrom, BasicBlock* nowFrom, BasicBlock* to)
{
    preFrom->getSuccessor().erase(
        std::find(preFrom->getSuccessor().begin(), preFrom->getSuccessor().end(), to)
    );
    nowFrom->getSuccessor().push_back(to);

    to->getPredecessor().erase(
        std::find(to->getPredecessor().begin(), to->getPredecessor().end(), preFrom)
    );
    to->getPredecessor().push_back(nowFrom);

    for(auto instr : to->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            for(int i = 0; i < phi->size(); i++)
                if(phi->getPredecessors(i).second == preFrom)
                    phi->setIncomingBB(i, nowFrom);
        }
        else
            break;
    }
}

void SimplifyCFG::addEdge(BasicBlock* from, BasicBlock* to, BasicBlock* origion)
{
    from->getSuccessor().push_back(to);
    to->getPredecessor().push_back(from);

    for(auto instr : to->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            Value* income = UndefValue::Create();
            for(auto [val, bb] : phi->getPredecessors())
                if(bb == origion)
                {
                    income = val;
                    break;
                }
            phi->setInComingValue(income, from);
        }
        else
            break;
    }
}

bool SimplifyCFG::uncondTheCondBr(BasicBlock* bb)
{
    // PrintMsg("uncondTheCondBr");
    auto lastInstr = *(--bb->getInstructions().end());
    if(auto br = dynamic_cast<BrInst*>(lastInstr))
    {
        if(auto cond = br->getCond())
        {
            if(br->getTrueBlock() == br->getFalseBlock())
            {
                // PrintMsg("uncondTheCondBr equal");
                auto eraseSingle = [&](std::vector<BasicBlock*>& BBS, BasicBlock* single) -> void {
                    int cnt = 0;
                    for(auto bb : BBS)
                        if(bb == single)
                            cnt ++;
                    if(cnt == 1)
                        return;
                    
                    BBS.erase(
                        std::find(BBS.begin(), BBS.end(), single)
                    );
                };
                auto target = br->getTrueBlock();
                br->setUnconditional(target);
                eraseSingle(bb->getSuccessor(), target);
                eraseSingle(target->getPredecessor(), bb);
                return false;
            }
            else if(auto c = dynamic_cast<ConstantInt*>(cond))
            {
                if(c->isZero())
                {
                    // dumpBasickBlock(bb);
                    // PrintMsg("uncondTheCondBr zero");
                    deleteEdge(bb, br->getTrueBlock());
                    br->setUnconditional(br->getFalseBlock());
                    return false;
                }
                else if(c->isOne())
                {
                    // PrintMsg("uncondTheCondBr one");
                    deleteEdge(bb, br->getFalseBlock());
                    br->setUnconditional(br->getTrueBlock());
                    return false;
                } 
            }
        }
    }
    return true;
}

bool SimplifyCFG::deleteEmptyBB(BasicBlock* bb)
{
    // PrintMsg("deleteEmptyBB");
    if(bb == bb->getParent()->getEntryBlock())
        return true;
    // dumpBasickBlock(bb);
    // PrintMsg((!isAfterLoopOpt && (bb->isPreHeader())));

    if(!isAfterLoopOpt && bb->isPreHeader())
        return true;

    if(bb->getInstructions().size() == 1)
        if(auto br = dynamic_cast<BrInst*>(*bb->getInstructions().begin());
            br && !br->getCond())
        {
            // PrintMsg("Empty bb");
            auto succ = br->getTrueBlock();
            for(auto instr : succ->getInstructions())
            {
                if(auto phi = dynamic_cast<PhiInst*>(instr))
                {
                    std::set<BasicBlock*> succPred(succ->getPredecessor().begin(), 
                                                    succ->getPredecessor().end());
                    std::set<BasicBlock*> pred(bb->getPredecessor().begin(),
                                                bb->getPredecessor().end());
                    std::set<BasicBlock*> ans;
                    std::set_intersection(pred.begin(), pred.end(), 
                                            succPred.begin(), succPred.end(),
                                            std::inserter(ans, ans.begin()));
                    
                    if(!ans.empty())
                        return true;
                    break;
                }
                else
                    break;
            }
            // PrintMsg("Empty bb");
            // dumpBasickBlock(bb);
            // dumpBasickBlock(succ);
            // dumpBasickBlock(bb);
            std::vector<BasicBlock*> predToReplace;
            for(auto pred : bb->getPredecessor())
                predToReplace.push_back(pred);
            for(auto pred : predToReplace)
            {
                auto lastInstr = *(--pred->getInstructions().end());
                if(auto br = dynamic_cast<BrInst*>(lastInstr))
                    br->replaceTarget(bb, succ);
                addEdge(pred, succ, bb);
                deleteEdge(pred, bb);
            }
            // dumpBasickBlock(bb);
            // dumpBasickBlock(succ);
            deleteEdge(bb, succ);
            return false;
        }


    return true;
}

bool SimplifyCFG::mergeBB(BasicBlock* bb)
{
    // PrintMsg("mergeBB");
    if(bb->getSuccessor().size() == 1)
    {
        auto lastInstr = *(--bb->getInstructions().end());
        if(auto br = dynamic_cast<BrInst*>(lastInstr); br && !br->getCond())
        {
            auto succ = br->getTrueBlock();
            if(succ->getPredecessor().size() != 1)
                return true;
            PrintMsg("mergeBB");
            // dumpBasickBlock(bb);
            // dumpBasickBlock(succ);
            bb->getInstructions().pop_back();
            std::vector<Instruction*> move(succ->getInstructions().begin(), 
                                            succ->getInstructions().end());
            // dumpBasickBlock(succ);
            for(auto instr : move)
            {
                succ->getInstructions().remove(instr);
                bb->insertBack(instr);
                instr->reSetBasicBlock(bb);
            }
            deleteEdge(bb, succ);

            std::vector<BasicBlock*> succToRedirect;
            for(auto succOfSucc : succ->getSuccessor())
                succToRedirect.push_back(succOfSucc);

            for(auto succOfSucc : succToRedirect)
                redirectEdge(succ, bb, succOfSucc);
            // dumpBasickBlock(bb);
            return false;
        }
    }
    return true;
}

bool SimplifyCFG::upperTheBranch(BasicBlock* bb)
{
    // PrintMsg("upperTheBranch");
    auto lastInstr = *(--bb->getInstructions().end());
    if(auto br = dynamic_cast<BrInst*>(lastInstr); br && !br->getCond())
    {
        auto succ = br->getTrueBlock();
        if(succ->getInstructions().size() == 1)
        {
            auto succLastInstr = *succ->getInstructions().begin();
            if(auto succBr = dynamic_cast<BrInst*>(succLastInstr); succBr && succBr->getCond())
            {
                // PrintMsg("upperTheBranch");
                // dumpBasickBlock(bb);
                // dumpBasickBlock(succ);
                // PrintMsg(succBr->getSignature());
                br->setConditional(succBr->getCond(), succBr->getTrueBlock(), succBr->getFalseBlock());
                std::vector<BasicBlock*> succToBeAdd;
                for(auto succOfsucc : succ->getSuccessor())
                    succToBeAdd.push_back(succOfsucc);
                for(auto succOfsucc : succToBeAdd)
                    addEdge(bb, succOfsucc, succ);
                deleteEdge(bb, succ);
                // dumpBasickBlock(bb);
                return false;
            }
        }
    }
    return true;
}

bool SimplifyCFG::simplifyPhi(BasicBlock* bb)
{
    // PrintMsg("simplifyPhi");
    if(bb->getPredecessor().size() == 1)
    {
        std::vector<PhiInst*> phiToRemove;
        for(auto instr : bb->getInstructions())
            if(auto phi = dynamic_cast<PhiInst*>(instr))
            {
                phi->replaceAllUseWith(phi->getPredecessors(0).first);
                phi->UnUseAll();
                phiToRemove.push_back(phi);
            }
            else
                break;
        for(auto phi : phiToRemove)
            phi->getBasicBlock()->getInstructions().remove(phi);
        if(!phiToRemove.empty())
        {
            // PrintMsg("simplifyPhi");
            // dumpBasickBlock(bb);
            return false;
        }
    }
    return true;
}

void SimplifyCFG::removeDeadBB(Function* func)
{
    std::unordered_set<BasicBlock*> vis;
    BasicBlock::BasicBlockDfs(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        return false;
    });

    std::vector<BasicBlock*> deadBB;
    for(auto bb : func->getBasicBlockList())
        if(!vis.count(bb))
            deadBB.push_back(bb);
    
    for(auto bb : deadBB)
    {
        std::vector<BasicBlock*> succToRemove;
        for(auto succ : bb->getSuccessor())
            succToRemove.push_back(succ);
        for(auto succ : bb->getSuccessor())
            deleteEdge(bb, succ);
        for(auto instr : bb->getInstructions())
            instr->UnUseAll();
        func->getBasicBlockList().remove(bb);
    }
}

void SimplifyCFG::run(Function* func)
{
    PrintMsg("SimplifyCFG at function " + func->getName());
    dumpFunction(func);
    bool fixed;
    change = false;
    do
    {
        fixed = true;

        std::unordered_set<BasicBlock*> vis;
        std::vector<BasicBlock*> PostOrderWorkList;
        BasicBlock::BasicBlockDfsPost(func->getEntryBlock(), [&vis](BasicBlock* bb) -> bool {
            if(vis.count(bb))
                return true;
            vis.insert(bb);
            return false;
        }, [&](BasicBlock* bb) -> void {
            // PrintMsg("push");
            // dumpBasickBlock(bb);
            PostOrderWorkList.push_back(bb);
        });

        for(auto bb : PostOrderWorkList)
        {
            // PrintMsg("In bb");
            // dumpBasickBlock(bb);
            fixed &= uncondTheCondBr(bb);
            if(auto live = deleteEmptyBB(bb); !live)
            {
                fixed &= live;
                // PrintMsg("Out bb");
                // dumpBasickBlock(bb);
                continue;
            }
            fixed &= mergeBB(bb);
            fixed &= upperTheBranch(bb);
            fixed &= simplifyPhi(bb);
            // PrintMsg("Out bb");
            // dumpBasickBlock(bb);
        }

        change |= !fixed;

    }while(!fixed);

    removeDeadBB(func);

    dumpFunction(func);
}

} // namespace ir
    
} // namespace sysy
