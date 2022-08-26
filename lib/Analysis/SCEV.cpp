//===-------------------- SCEV.cpp - Scala Evolution ----------------------===//
// 1. G + {a, +, b} -> {G + a, +, b}
// 2. G * {a, +, b} -> {G * a, +, G * b}
// 3. {a, +, b} + {c, +, d} -> {a + c, +, b + d}
// 4. {a, +, b} * {c, +, d} -> {a * c, +, a * d + b * c + b * d, +, 2 * b * d}
// 5. {a, +, b, +, c} * {d, +, e} ->
// {a * d, +, b * e + b * d + a * e, + , 2 * b * e + c * d + 2 * c * e, + , 3 * c * e}
//===----------------------------------------------------------------------===//


#include "sysy/Analysis/SCEV.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Loop.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/IRDump.h"

namespace sysy
{
namespace ir
{

// TODO We can Only recongnize the add BIV.
void ScalarEvolution::registerBIV(Loop* loop, PhiInst* phi)
{
        std::stack<BinaryInst*> workStk;
        for(auto use : phi->getUses())
        {
            auto user = use->getUser();
            if(auto instr = dynamic_cast<BinaryInst*>(user); 
                instr && instr->getBasicBlock() && (instr->getKind() == Instruction::Add 
                        || instr->getKind() == Instruction::Sub))
                workStk.push(instr);
        }

        std::set<Value*> addInSCEV;
        std::set<Value*> subInSCEV;

        while(!workStk.empty())
        {
            auto binary = workStk.top();
            workStk.pop();

            if(binary->getKind() == Instruction::Add)
            {
                if(loop->isLoopInvariant(binary->getLhs()))
                    addInSCEV.insert(binary->getLhs());
                else if(loop->isLoopInvariant(binary->getRhs()))
                    addInSCEV.insert(binary->getRhs());
                else
                    return;
            }
            else if(binary->getKind() == Instruction::Sub)
            {
                if(loop->isLoopInvariant(binary->getRhs()))
                    subInSCEV.insert(binary->getRhs());
                else
                    return;
            }
            for(auto use : binary->getUses())
            {
                auto user = use->getUser();
                if(user == phi)
                {
                    BIV2SCEV(loop, phi, addInSCEV, subInSCEV);
                    return;
                }   
                else if(auto instr = dynamic_cast<BinaryInst*>(user); 
                    instr && (instr->getKind() == Instruction::Add 
                        || instr->getKind() == Instruction::Sub))
                    workStk.push(instr);
            }
        }
}

void ScalarEvolution::BIV2SCEV(Loop* loop, PhiInst* phi, std::set<Value*>& adds, std::set<Value*>& subs)
{
    Value* initial = nullptr;
    for(auto [val, pred] : phi->getPredecessors())
        if(loop->isLoopInvariant(val))
            initial = val;
    
    if(!initial)
        return;
    if(!adds.empty())
    {
        auto step = *adds.begin();
        adds.erase(step);
        std::set<BinaryInst*> binaryStore;
        for(auto sub : subs)
        {
            auto binary = new BinaryInst(step->getType(), Instruction::Sub, step, sub, nullptr, "scevsubtmp");
            binaryStore.insert(binary);
            step = binary;
        }
        for(auto add : adds)
        {
            auto binary = new BinaryInst(step->getType(), Instruction::Add, step, add, nullptr, "scevaddtmp");
            binaryStore.insert(binary);
            step = binary;
        }
        SCEV scev(initial, step, std::move(binaryStore));
        loop->registerSCEV(phi, scev);
        return;
    }
    auto sub = *subs.begin();
    subs.erase(sub);
    std::set<BinaryInst*> binaryStore;
    auto step = new BinaryInst(Type::getInt32Ty(), Instruction::Sub, ConstantInt::getZero(Type::getInt32Ty()), sub, nullptr, "scevsubtmp");
    binaryStore.insert(step);
    for(auto sub : subs)
    {
        step = new BinaryInst(Type::getInt32Ty(), Instruction::Sub, ConstantInt::getZero(Type::getInt32Ty()), sub, nullptr, "scevsubtmp");
        binaryStore.insert(step);
    }
    SCEV scev(initial, step, std::move(binaryStore));
    loop->registerSCEV(phi, scev);
}

void ScalarEvolution::findAndRegisterBIV(Loop* loop)
{
    auto header = loop->getHeader();
    std::vector<PhiInst*> phiList;
    for(auto instr : header->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
            phiList.push_back(phi);
        else
            break;
    }

    for(auto phi : phiList)
    {
        std::stack<Instruction*> workStk;
        for(auto use : phi->getUses())
        {
            auto user = use->getUser();
            if(auto instr = dynamic_cast<Instruction*>(user))
                workStk.push(instr);
        }

        while(!workStk.empty())
        {
            auto instr = workStk.top();
            workStk.pop();
            if(instr == phi)
            {
                registerBIV(loop, phi);
                break;
            }

            if(auto phi = dynamic_cast<PhiInst*>(instr))
                break;

            if(loop->isLoopInvariant(instr))
                continue;
            
            for(auto use : instr->getUses())
            {
                auto user = use->getUser();
                if(auto instr = dynamic_cast<Instruction*>(user))
                    workStk.push(instr);
            }
        }
    }
}

using Kind = Instruction::InstrKind;

void ScalarEvolution::linkInstructionToSCEV(Loop* loop, BinaryInst* binary)
{
    auto lhs = binary->getLhs();
    auto rhs = binary->getRhs();

    if(auto lhsInstr = dynamic_cast<Instruction*>(lhs); 
        lhsInstr && loop->hasSCEV(lhsInstr))
    {
        if(loop->isLoopInvariant(rhs))
        {
            if(binary->getKind() == Kind::Add)
            {
                auto scev = rhs + loop->getSCEV(lhsInstr);
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            }
            else if(binary->getKind() == Kind::Mul)
            {
                auto scev = rhs * loop->getSCEV(lhsInstr);
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            }
            else if(binary->getKind() == Kind::Sub)
            {
                auto scev = loop->getSCEV(lhsInstr) - rhs;
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            }
        }
        else if(auto rhsInstr = dynamic_cast<Instruction*>(rhs);
                rhsInstr && loop->hasSCEV(rhsInstr))
        {
            if(binary->getKind() == Kind::Add)
            {
                auto scev = loop->getSCEV(lhsInstr) + loop->getSCEV(rhsInstr);
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            } // TODO solve the Higher SCEV like {0, +, 1} * {0, +, 1} = {0, +, 1, +, 2}
            // else if(binary->getKind() == Kind::Mul)
            // {
            //     auto scev = loop->getSCEV(lhsInstr) * loop->getSCEV(rhsInstr);
            //     loop->registerSCEV(binary, scev);
            //     fixed = false;
            //     return;
            // }
            else if(binary->getKind() == Kind::Sub)
            {
                auto scev = loop->getSCEV(lhsInstr) - loop->getSCEV(rhsInstr);
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            }
        }
    }
    else if(auto rhsInstr = dynamic_cast<Instruction*>(rhs);
                rhsInstr && loop->hasSCEV(rhsInstr))
    {
        if(loop->isLoopInvariant(lhs))
        {
            if(binary->getKind() == Kind::Add)
            {
                auto scev = lhs + loop->getSCEV(rhsInstr);
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            }
            else if(binary->getKind() == Kind::Mul)
            {
                auto scev = lhs * loop->getSCEV(rhsInstr);
                loop->registerSCEV(binary, scev);
                fixed = false;
                return;
            }
        }
    }

}

void ScalarEvolution::runOnLoop(Loop* loop)
{
    if(!loop)
        return;
    
    for(auto subLoop : loop->getSubLoops())
        runOnLoop(subLoop);

    loop->cleanSCEV();
    // Step1. Find the BIV.
    findAndRegisterBIV(loop);

    // Step2. For every instrution calculate the SCEV
    do
    {
        fixed = true;

        for(auto bb : loop->getLoopBasicBlocks())
        {
            for(auto instr : bb->getInstructions())
            {
                if(loop->hasSCEV(instr))
                    continue;
                if(auto binary = dynamic_cast<BinaryInst*>(instr); binary
                    && (binary->getKind() == Kind::Add || binary->getKind() == Kind::Sub
                        || binary->getKind() == Kind::Mul))
                    linkInstructionToSCEV(loop, binary);
            }
        }

    } while(!fixed);

    // for(auto [instr, scev] : loop->getSCEV())
    // {
    //     PrintMsg(instr->getSignature() + " SCEV: " + scev.getSignature());
    // }

    if(loop->getIndexPhi() && loop->hasSCEV(loop->getIndexPhi()))
    {
        auto phi = loop->getIndexPhi();
        auto& scev = loop->getSCEV(phi);

        if(auto cEnd = dynamic_cast<ConstantInt*>(loop->getIndexEnd()))
            if(auto cInitial = dynamic_cast<ConstantInt*>(scev.at(0)))
                if(auto cStep = dynamic_cast<ConstantInt*>(scev.at(1)))
                {
                    int c;
                    if(loop->getICmpKind() == Kind::ICmpEQ 
                        || loop->getICmpKind() == Kind::ICmpSGE
                        || loop->getICmpKind() == Kind::ICmpSLE)
                        c = (cEnd->getValue() + 1 - cInitial->getValue()) / cStep->getValue();
                    else
                        c = (cEnd->getValue() - cInitial->getValue()) / cStep->getValue();
                    loop->setTripCount(c);
                }
    }
}

void ScalarEvolution::run(Function* func)
{
    PrintMsg("ScalarEvolution in Function " + func->getName());
    for(auto loop : func->getTopLoops())
        runOnLoop(loop);
}

void ScalarEvolutionClean::runOnLoop(Loop* loop)
{
    for(auto subLoop : loop->getSubLoops())
        runOnLoop(subLoop);
    
    for(auto& [instr, scev] : loop->getSCEV())
        scev.clear();
}

void ScalarEvolutionClean::run(Function* func)
{
    PrintMsg("ScalarEvolution-clean in Function " + func->getName());
    for(auto loop : func->getTopLoops())
        runOnLoop(loop);
}

} // namespace ir
    
} // namespace sysy
