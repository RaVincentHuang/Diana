#include "sysy/Transform/Scalar/PRE.h"
#include "sysy/Transform/Utils/MemorySSA.h"
#include "sysy/Transform/Utils/SplitCriticalEdges.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/Analysis/DomInfo.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include <unordered_map>
#include <set>
#include <queue>

namespace sysy
{

namespace ir
{

PRE::PRE()
{
    getRelated().push_back(new SplitCriticalEdge);
    getRelated().push_back(new DomInfo);
    getRelated().push_back(new MemorySSA);
    getFinish().push_back(new ClearMemorySSA);
    // getFinish().push_back(new FunctionVertify);
}
static int cnt = 0;
Occurrence::Occurrence(std::string _name) 
    : name(_name), _class(cnt++) {}

RealOccurrence::RealOccurrence(Instruction* instr, std::string _name)
    : Occurrence(_name), expr(instr) {}

Instruction* RealOccurrence::getExpr()
{
    return expr;
}

std::string RealOccurrence::getSignature()
{
    return expr->getSignature() + " [h:" + getName() + "]";
}

PhiOccurrence::PhiOccurrence(BasicBlock* bb, std::string _name)
    : Occurrence(_name), father(bb) {}

void PhiOccurrence::setIncoming(Occurrence* occur, BasicBlock* pred)
{
    incomingOccurrence.push_back(std::make_pair(occur, pred));
}

std::vector<std::pair<Occurrence*, BasicBlock*>>& PhiOccurrence::getIncoming()
{
    return incomingOccurrence;
}

Occurrence* PhiOccurrence::getIncoming(size_t index)
{
    return incomingOccurrence.at(index).first;
}

BasicBlock* PhiOccurrence::getBB(size_t index)
{
    return incomingOccurrence.at(index).second;
}

std::string PhiOccurrence::getSignature()
{
    std::string str = "h:" + std::to_string(cnt++);
    str += " = phi"; 
    for(auto [curr, pred] : incomingOccurrence)
        str += " [" + curr->getName() + ", " + pred->getName() + "] ";
    return str;
}

CollectOccurrences::CollectOccurrences(Instruction* key)
    : keyInstr(key) {}

bool CollectOccurrences::isSameExpr(Instruction* expr)
{
    if(expr->getKind() != keyInstr->getKind())
        return false;
    if(auto binary = dynamic_cast<BinaryInst*>(expr); binary && binary->isCommutative())
        if(auto keyBinary = dynamic_cast<BinaryInst*>(keyInstr))
        {
            if(PRE::sameOriginal(binary->getLhs(), keyBinary->getRhs())
                && PRE::sameOriginal(binary->getRhs(), keyBinary->getLhs()))
                return true;
        }
    if(auto dead =  dynamic_cast<DeadInst*>(expr))
        return false;
    if(keyInstr->getOprands().size() != expr->getOprands().size())
        return false;
    auto len = keyInstr->getOprands().size();
    for(auto i = 0; i < len; i++)
    {
        if(!PRE::sameOriginal(keyInstr->getOprands().at(i)->getValue(), 
                                    expr->getOprands().at(i)->getValue()))
            return false;
    }
    return true;
}

void CollectOccurrences::addExpr(Instruction* instr)
{
    auto bb = instr->getBasicBlock();
    realOccurrencesBB.insert(bb);
    instructions[bb].push_back(instr);
}

void CollectOccurrences::addPhi(PhiOccurrence* phi, BasicBlock* bb)
{
    phiBB.insert(bb);
    phis[bb] = phi;
}

std::vector<Instruction*>& CollectOccurrences::getExprFromBB(BasicBlock* bb)
{
    return instructions[bb];
}

static std::vector<CollectOccurrences> collectOccurrencesList;

bool PRE::sameOriginal(Value* x, Value* y)
{
    if(x == y)
        return true;
    if(auto instrX = dynamic_cast<Instruction*>(x))
        if(auto instrY = dynamic_cast<Instruction*>(y))
        {
            std::unordered_set<Instruction*> xSet, ySet;
            std::stack<PhiInst*> stk;
            xSet.insert(instrX); ySet.insert(instrY);
            if(auto phiX = dynamic_cast<PhiInst*>(instrX))
            {
                stk.push(phiX);
                while(!stk.empty())
                {
                    auto phi = stk.top();
                    stk.pop();
                    for(auto [incomingVal, pred] : phi->getPredecessors())
                    {
                        if(incomingVal == y)
                            return true;
                        if(auto instr = dynamic_cast<Instruction*>(incomingVal))
                        {
                            xSet.insert(instr);
                            if(auto predPhi = dynamic_cast<PhiInst*>(instr))
                                stk.push(predPhi);
                        }
                    }
                }
            }
            if(auto phiY = dynamic_cast<PhiInst*>(instrY))
            {
                stk.push(phiY);
                while(!stk.empty())
                {
                    auto phi = stk.top();
                    stk.pop();
                    for(auto [incomingVal, pred] : phi->getPredecessors())
                    {
                        if(incomingVal == x)
                            return true;
                        if(auto instr = dynamic_cast<Instruction*>(incomingVal))
                        {
                            ySet.insert(instr);
                            if(auto predPhi = dynamic_cast<PhiInst*>(instr))
                                stk.push(predPhi);
                        }
                    }
                }
            }

            return xSet.count(instrY) || ySet.count(instrX);
        }
    return false;
}

void PRE::buildCollectOccurrences(Function* func)
{
    for(auto bb : func->getBasicBlockList())
        for(auto instr : bb->getInstructions())
        {
            bool alreadyExist = false;
            for(auto& collect : collectOccurrencesList)
                if(collect.isSameExpr(instr))
                {
                    alreadyExist = true;
                    collect.addExpr(instr);
                }
            if(!alreadyExist)
                collectOccurrencesList.push_back(CollectOccurrences(instr));
        }
}

static std::unordered_set<PhiOccurrence*> downSafe;

void PRE::insertPhiInstruction(Function* func)
{
    std::set<BasicBlock*> hasPhiBB;
    for(auto bb : func->getBasicBlockList())
    {
        auto beginInstr = *bb->getInstructions().begin();
        if(auto phi = dynamic_cast<PhiInst*>(beginInstr))
            hasPhiBB.insert(bb);
    }

    int cnt = 0;
    for(auto& collect : collectOccurrencesList)
    {
        std::set<BasicBlock*> BBNeedToInsert;
        std::unordered_set<BasicBlock*> vis;
        std::queue<BasicBlock*> WorkList;
        for(auto bb : collect.getRealOccurrencesBB())
            WorkList.push(bb);
        while(!WorkList.empty())
        {
            auto bb = WorkList.front();
            WorkList.pop();
            for(auto tar : bb->getDomFrontier())
                if(!vis.count(tar))
                {
                    vis.insert(tar);
                    BBNeedToInsert.insert(tar);
                    if(!collect.getRealOccurrencesBB().count(tar))
                        WorkList.push(tar);
                }
        }

        std::set_union(BBNeedToInsert.begin(), BBNeedToInsert.end(), 
                        hasPhiBB.begin(), hasPhiBB.end(),
                        std::inserter(BBNeedToInsert, BBNeedToInsert.begin()));
        
        for(auto bb : BBNeedToInsert)
        {
            std::string name = "expr_" + std::to_string(cnt) + "." 
                                + std::to_string(collect.occurrenceName.size() + collect.phiSize());
            auto phi = new PhiOccurrence(bb, name);
            collect.allClasses.insert(phi);
            downSafe.insert(phi);
            collect.addPhi(phi, bb);
        }
        cnt++;
    }
}

static std::unordered_map<PhiOccurrence*, std::unordered_set<int>> hasRealUse;
static std::unordered_map<PhiOccurrence*, BasicBlock*> crossReal;

using InstrCpy = std::pair<Instruction*, std::vector<Value*>>;
// std::unordered_set<InstrCpy> forRename2; // FIXME
static int varCnt;
void PRE::rename1(BasicBlock* entry, CollectOccurrences& collect)
{
    // forRename2.clear(); // FIXME
    std::unordered_map<BasicBlock*, Occurrence*> IncomingOccurrence;
    IncomingOccurrence[entry] = UndefOccurrence::get();
    std::unordered_set<BasicBlock*> vis;
    std::stack<std::pair<BasicBlock*, BasicBlock*>> WorkStk;
    WorkStk.push(std::make_pair(nullptr, entry));
    while (!WorkStk.empty())
    {
        auto pred = WorkStk.top().first;
        auto bb = WorkStk.top().second;
        WorkStk.pop();
        if(collect.getPhiBB().count(bb))
        {
            auto phi = collect.getPhi(bb);
            bool hasReal = false;
            if(auto real = dynamic_cast<RealOccurrence*>(IncomingOccurrence[pred]))
                hasReal = true;
            else if(auto prePhi = dynamic_cast<PhiOccurrence*>(IncomingOccurrence[pred]); prePhi
                && crossReal.count(prePhi) && pred->getDoms().count(crossReal[prePhi]))
                hasReal = true;
            if(hasReal)
            {
                if(!hasRealUse.count(phi))
                    hasRealUse[phi] = {};
                hasRealUse[phi].insert(phi->size());
            }

            phi->setIncoming(IncomingOccurrence[pred], pred);
        }
        if(!vis.count(bb))
        {
            vis.insert(bb);
            
            auto val = (pred) ? IncomingOccurrence[pred] : UndefOccurrence::get();
            if(collect.getRealOccurrencesBB().count(bb))
                for(auto instr : collect.getExprFromBB(bb))
                {
                    auto isSame = [](Instruction* x, Instruction* y) -> bool {
                        if(auto binary = dynamic_cast<BinaryInst*>(x); binary && binary->isCommutative())
                            if(auto keyBinary = dynamic_cast<BinaryInst*>(y))
                            {
                                if(binary->getLhs() == keyBinary->getRhs()
                                    && binary->getRhs() == keyBinary->getLhs())
                                    return true;
                            }
                        auto len = y->getOprands().size();
                        for(auto i = 0; i < len; i++)
                        {
                            if(y->getOprands().at(i)->getValue() 
                                != x->getOprands().at(i)->getValue())
                                return false;
                        }
                        return true;
                    };

                    if(auto real = dynamic_cast<RealOccurrence*>(val))
                    {
                        if(isSame(real->getExpr(), instr))
                            collect.occurrenceName[instr] = real;
                        else
                        {
                            std::string str = "expr_" + std::to_string(varCnt) + "."
                                                + std::to_string(collect.occurrenceName.size() + collect.phiSize());
                            auto name = new RealOccurrence(instr, str);
                            collect.allClasses.insert(name);
                            collect.occurrenceName[instr] = name;
                            val = name;
                        }
                    }
                    else if(auto phi = dynamic_cast<PhiOccurrence*>(val))
                    {
                        bool allDominate = true;
                        for(auto operand : instr->getOprands())
                            if(auto variable = dynamic_cast<Instruction*>(operand->getValue()); variable
                                && !phi->getDefineBB()->getDoms().count(variable->getBasicBlock()))
                            {
                                allDominate = false;
                                break;
                            }
                        if(allDominate)
                        {
                            collect.occurrenceName[instr] = phi;
                            // forRename2.insert(std::make_pair(instr, std::vector<Value*>()));
                            // FIXME  
                            crossReal[phi] = bb;
                        }
                        else
                        {
                            downSafe.erase(phi);
                            std::string str = "expr_" + std::to_string(varCnt) + "."
                                                + std::to_string(collect.occurrenceName.size() + collect.phiSize());
                            auto name = new RealOccurrence(instr, str);
                            collect.allClasses.insert(name);
                            collect.occurrenceName[instr] = name;
                            val = name;
                        }
                    }
                }
            if(bb->getSuccessor().empty())
                if(auto phi = dynamic_cast<PhiOccurrence*>(val))
                    downSafe.erase(phi);
            IncomingOccurrence[pred] = val;
            for(auto riter = bb->getSuccessor().rbegin(); riter != bb->getSuccessor().rend(); riter++)
                WorkStk.push(std::make_pair(bb, *riter));
        }
    }
}

void PRE::rename2(BasicBlock* entry, CollectOccurrences& collect)
{
    std::unordered_map<PhiOccurrence*, std::unordered_set<int>> processed;
    for(auto bb : collect.getPhiBB())
        processed[collect.getPhi(bb)] = {};
    // while(!forRename2.empty()) // FIXME 
    // {
    //     auto Z = *forRename2.begin();
    //     forRename2.erase(Z);
    //     auto tmp = collect.occurrenceName[Z.first];
    //     if(auto f = dynamic_cast<PhiOccurrence*>(tmp))
    //     {
    //         for(int index = 0; index < f->size(); index++)
    //         {
    //             if(!processed[f].count(index))
    //             {
    //                 processed[f].insert(index);

    //                 auto phiOperandFromRes = [&f](InstrCpy Z, int idx) -> void {
    //                     auto b = f->getDefineBB();
    //                     if(Z.second.empty())
    //                         for(auto use : Z.first->getOprands())
    //                         {
    //                             auto operand = use->getValue();
    //                             if(auto phi = dynamic_cast<PhiInst*>(operand); phi && phi->getBasicBlock() == b)
    //                                 operand = phi->getPredecessors(idx).first;
    //                             Z.second.push_back(operand);
    //                         }
    //                     else
    //                     {
    //                         for(auto iter = Z.second.begin(); iter != Z.second.end(); iter++)
    //                         {
    //                             auto operand = *iter;
    //                             if(auto phi = dynamic_cast<PhiInst*>(operand); phi && phi->getBasicBlock() == b)
    //                                 operand = phi->getPredecessors(idx).first;
    //                             *iter = operand; 
    //                         }
    //                     }
    //                 };
    //                 phiOperandFromRes(Z, index);
    //                 auto X = f->getIncoming(index);
    //                 if(auto real = dynamic_cast<RealOccurrence*>(X))
    //                 {
    //                     auto isSame = [](Instruction* x, InstrCpy y) -> bool {
    //                         if(auto binary = dynamic_cast<BinaryInst*>(x); binary && binary->isCommutative())
    //                             if(auto keyBinary = dynamic_cast<BinaryInst*>(y.first))
    //                             {
    //                                 if(binary->getLhs() == y.second.at(1)
    //                                     && binary->getRhs() == y.second.at(0))
    //                                     return true;
    //                             }
    //                         auto len = y.first->getOprands().size();
    //                         for(auto i = 0; i < len; i++)
    //                             if(x->getOprands().at(i)->getValue() != y.second.at(i))
    //                                 return false;
    //                         return true;
    //                     };
    //                     if(isSame(real->getExpr(), Z))
    //                         continue;
    //                     else
    //                         f->changeIncoming(index, UndefOccurrence::get());
    //                 }
    //                 else if(auto phi = dynamic_cast<PhiOccurrence*>(X))
    //                 {
    //                     auto allDom = [](InstrCpy instr, BasicBlock* bb) -> bool {
    //                         for(auto iter = instr.second.begin(); iter != instr.second.end(); iter++)
    //                             if(auto variable = dynamic_cast<Instruction*>(*iter);
    //                                 variable && !bb->getDoms().count(variable->getBasicBlock()))
    //                                 return false;
    //                         return true;
    //                     };
    //                     if(allDom(Z, phi->getDefineBB()))
    //                         forRename2.insert(Z);
    //                     else
    //                         f->changeIncoming(index, UndefOccurrence::get());
    //                 }
    //             }

    //         }
    //     }
    //     else
    //         ErrorMsg("There is no phi in rename2");
    // }
}

void PRE::renaming(BasicBlock* entry)
{
    for(auto& collect : collectOccurrencesList)
    {
        rename1(entry, collect);
        rename2(entry, collect);
    }
    varCnt++;
}

void PRE::resetDownSafe(PhiOccurrence* phi, int index)
{
    if(auto occ = dynamic_cast<PhiOccurrence*>(phi->getIncoming(index)); 
        hasRealUse[phi].count(index) || !occ)
        return;
    else if(!downSafe.count(occ))
        return;
    else
    {
        downSafe.erase(occ);
        for(int i = 0; i < occ->size(); i++)
            resetDownSafe(occ, i);
    }
}

void PRE::downSafety(CollectOccurrences& collect)
{
    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        if(!downSafe.count(phi))
            for(int i = 0; i < phi->size(); i++)
                resetDownSafe(phi, i);
    }
}

static std::unordered_set<PhiOccurrence*> canBeAvail;
static std::unordered_set<PhiOccurrence*> later;

void PRE::resetCanBeAvail(PhiOccurrence* occ, CollectOccurrences& collect)
{
    canBeAvail.erase(occ);
    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        for(int i = 0; i < phi->size(); i++)
            if(phi->getIncoming(i) == occ && hasRealUse[phi].count(i))
                if(!downSafe.count(phi) && canBeAvail.count(phi))
                {
                    resetCanBeAvail(phi, collect);
                    break;
                }
    }

}
void PRE::computeCanBeAvail(CollectOccurrences& collect)
{
    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        canBeAvail.insert(phi);
    }

    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        auto hasUndef = [](PhiOccurrence* occ) -> bool {
            for(auto [operand, pred] : occ->getIncoming())
                if(auto undef = dynamic_cast<UndefOccurrence*>(operand))
                    return true;
            return false;
        };
        if(!downSafe.count(phi) && canBeAvail.count(phi) && hasUndef(phi))
            resetCanBeAvail(phi, collect);
    }
}
void PRE::resetLater(PhiOccurrence* occ, CollectOccurrences& collect)
{
    later.erase(occ);
    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        for(int i = 0; i < phi->size(); i++)
            if(phi->getIncoming(i) == occ && later.count(phi))
            {
                resetLater(phi, collect);
                break;
            }
    }
}
void PRE::computeLater(CollectOccurrences& collect)
{
    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        if(canBeAvail.count(phi))
            later.insert(phi);
    }

    for(auto bb : collect.getPhiBB())
    {
        auto phi = collect.getPhi(bb);
        auto judge = [](PhiOccurrence* occ) -> bool {
            for(int i = 0; i < occ->size(); i++)
                if(auto undef = dynamic_cast<UndefOccurrence*>(occ->getIncoming(i));
                    !undef && hasRealUse[occ].count(i))
                    return true;
            return false;
        };
        if(later.count(phi) && judge(phi))
            resetLater(phi, collect);
    }

}

void PRE::finalize1(CollectOccurrences& collect)
{

    for(auto x : collect.allClasses)
        return;
}

void PRE::finalize2(CollectOccurrences& collect)
{

}

void PRE::finalize()
{

}

void PRE::willBeAvail(CollectOccurrences& collect)
{
    computeCanBeAvail(collect);
    computeLater(collect);
}

void PRE::run(Function* func)
{
    collectOccurrencesList.clear();
    buildCollectOccurrences(func);
    insertPhiInstruction(func);
    renaming(func->getEntryBlock());
    for(auto& collect : collectOccurrencesList)
        downSafety(collect);
    for(auto& collect : collectOccurrencesList)
        willBeAvail(collect);
    
} 

} // namespace ir


} // namespace sysy

