#include "sysy/Transform/Scalar/InstructionCombine.h"
#include "sysy/Transform/Scalar/SimplifyInstructions.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/IRDump.h"

namespace sysy
{
namespace ir
{

Instruction* InstructionCombine::visitBinary(BinaryInst* binary)
{
    if(binary->getKind() == Instruction::InstrKind::Add)
        return visitAdd(binary);
    
    if(binary->getKind() == Instruction::InstrKind::Sub)
        return visitSub(binary);

    return binary;
}

Instruction* InstructionCombine::visitAdd(BinaryInst* add)
{
    if(auto clhs = dynamic_cast<ConstantInt*>(add->getLhs()))
    {
        if(auto rhs = dynamic_cast<BinaryInst*>(add->getRhs()))
        {
            if(rhs->getKind() == Instruction::InstrKind::Add)
            {
                // (C1 + (C2 + X)) -> X + (C1 + C2)
                if(auto crhsLhs = dynamic_cast<ConstantInt*>(rhs->getLhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsLhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Add, 
                                            rhs->getRhs(), ConstantInt::get(32, C1 + C2), 
                                            add->getBasicBlock(), "addtmp.byadd");
                }
                // (C1 + (X + C2)) -> X + (C1 + C2)
                else if(auto crhsRhs = dynamic_cast<ConstantInt*>(rhs->getRhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsRhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Add, 
                                            rhs->getLhs(), ConstantInt::get(32, C1 + C2), 
                                            add->getBasicBlock(), "addtmp.byadd");
                }
            }
            else if(rhs->getKind() == Instruction::InstrKind::Sub)
            {
                // (C1 + (C2 - X)) ->  (C1 + C2) - X
                if(auto crhsLhs = dynamic_cast<ConstantInt*>(rhs->getLhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsLhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Sub, 
                                            ConstantInt::get(32, C1 + C2), rhs->getRhs(), 
                                            add->getBasicBlock(), "subtmp.byadd");
                }
                // (C1 + (X - C2)) -> X + (C1 - C2)
                else if(auto crhsRhs = dynamic_cast<ConstantInt*>(rhs->getRhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsRhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Sub, 
                                            rhs->getLhs(), ConstantInt::get(32, C1 - C2), 
                                            add->getBasicBlock(), "subtmp.byadd");
                }
            }
        }
    }
    else if(auto crhs = dynamic_cast<ConstantInt*>(add->getRhs()))
    {
        if(auto lhs = dynamic_cast<BinaryInst*>(add->getLhs()))
        {
            if(lhs->getKind() == Instruction::InstrKind::Add)
            {
                // ((C1 + X) + C2) -> X + (C1 + C2)
                if(auto clhsLhs = dynamic_cast<ConstantInt*>(lhs->getLhs()))
                {
                    int C1 = crhs->getValue();
                    int C2 = clhsLhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Add, 
                                            lhs->getRhs(), ConstantInt::get(32, C1 + C2), 
                                            add->getBasicBlock(), "addtmp.byadd");
                }
                // ((X + C1) + C2)) -> X + (C1 + C2)
                else if(auto clhsRhs = dynamic_cast<ConstantInt*>(lhs->getRhs()))
                {
                    int C1 = crhs->getValue();
                    int C2 = clhsRhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Add, 
                                            lhs->getLhs(), ConstantInt::get(32, C1 + C2), 
                                            add->getBasicBlock(), "addtmp.byadd");
                }
            }
            else if(lhs->getKind() == Instruction::InstrKind::Sub)
            {
                // ((C1 - X) + C2) ->  (C1 + C2) - X
                if(auto clhsLhs = dynamic_cast<ConstantInt*>(lhs->getLhs()))
                {
                    int C1 = clhsLhs->getValue();
                    int C2 = crhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Sub, 
                                            ConstantInt::get(32, C1 + C2), lhs->getRhs(), 
                                            add->getBasicBlock(), "subtmp.byadd");
                }
                // ((X - C1) + C2) -> X + (C2 - C1)
                else if(auto clhsRhs = dynamic_cast<ConstantInt*>(lhs->getRhs()))
                {
                    int C1 = clhsRhs->getValue();
                    int C2 = crhs->getValue();
                    return new BinaryInst(add->getType(), Instruction::InstrKind::Add, 
                                            lhs->getLhs(), ConstantInt::get(32, C2 - C1), 
                                            add->getBasicBlock(), "addtmp.byadd");
                }
            }
        }
    }

    // X + X -> X * 2
    if(add->getLhs() == add->getRhs())
        return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                add->getLhs(), ConstantInt::get(32, 2),
                                add->getBasicBlock(), "multmp.byadd");
    
    if(auto mulLhs = dynamic_cast<BinaryInst*>(add->getLhs()); mulLhs && mulLhs->getKind() == Instruction::InstrKind::Mul)
    {
        auto lhsLhs = mulLhs->getLhs();
        auto lhsRhs = mulLhs->getRhs();
        // X * C + X -> X * (C + 1)
        if(lhsLhs == add->getRhs())
        {   
            if(auto clhsRhs = dynamic_cast<ConstantInt*>(lhsRhs))
            {
                auto C = clhsRhs->getValue();
                return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                lhsLhs, ConstantInt::get(32, C + 1),
                                add->getBasicBlock(), "multmp.byadd");
            }
        }
        // C * X + X -> X * (C + 1)
        if(lhsRhs == add->getRhs())
        {
            if(auto clhsLhs = dynamic_cast<ConstantInt*>(lhsLhs))
            {
                auto C = clhsLhs->getValue();
                return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                lhsRhs, ConstantInt::get(32, C + 1),
                                add->getBasicBlock(), "multmp.byadd");
            }
        }

        // X * C1 + X * C2 -> X * (C1 + C2)
        if(auto mulRhs = dynamic_cast<BinaryInst*>(add->getRhs()); mulRhs && mulRhs->getKind() == Instruction::InstrKind::Mul)
        {
            auto rhsLhs = mulRhs->getLhs();
            auto rhsRhs = mulRhs->getRhs();

            if(lhsLhs == rhsLhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsRhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsRhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                lhsLhs, ConstantInt::get(32, C1 + C2),
                                add->getBasicBlock(), "multmp.byadd");
                    }
            }
            if(lhsLhs == rhsRhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsRhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsLhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                lhsLhs, ConstantInt::get(32, C1 + C2),
                                add->getBasicBlock(), "multmp.byadd");
                    }
            }
            if(lhsRhs == rhsLhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsLhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsRhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                lhsRhs, ConstantInt::get(32, C1 + C2),
                                add->getBasicBlock(), "multmp.byadd");
                    }
            }
            if(lhsRhs == rhsRhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsLhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsLhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                lhsRhs, ConstantInt::get(32, C1 + C2),
                                add->getBasicBlock(), "multmp.byadd");
                    }
            }
        }
    }
    if(auto mulRhs = dynamic_cast<BinaryInst*>(add->getRhs()); mulRhs && mulRhs->getKind() == Instruction::InstrKind::Mul)
    {
        auto rhsLhs = mulRhs->getLhs();
        auto rhsRhs = mulRhs->getRhs();
        // X + X * C -> X * (C + 1)
        if(rhsLhs == add->getLhs())
        {
            if(auto crhsRhs = dynamic_cast<ConstantInt*>(rhsRhs))
            {
                auto C = crhsRhs->getValue();
                return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                rhsLhs, ConstantInt::get(32, C + 1),
                                add->getBasicBlock(), "multmp.byadd");
            }
        }
        // X + C * X -> X * (C + 1)
        if(rhsRhs == add->getLhs())
        {
            if(auto crhsLhs = dynamic_cast<ConstantInt*>(rhsLhs))
            {
                auto C = crhsLhs->getValue();
                return new BinaryInst(add->getType(), Instruction::InstrKind::Mul,
                                rhsRhs, ConstantInt::get(32, C + 1),
                                add->getBasicBlock(), "multmp.byadd");
            }
        }
    }


    return add;
}

Instruction* InstructionCombine::visitSub(BinaryInst* sub)
{
    if(auto clhs = dynamic_cast<ConstantInt*>(sub->getLhs()))
    {
        if(auto rhs = dynamic_cast<BinaryInst*>(sub->getRhs()))
        {
            if(rhs->getKind() == Instruction::InstrKind::Add)
            {
                // (C1 - (C2 + X)) -> (C1 - C2) - X
                if(auto crhsLhs = dynamic_cast<ConstantInt*>(rhs->getLhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsLhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Sub, 
                                            ConstantInt::get(32, C1 - C2), rhs->getRhs(), 
                                            sub->getBasicBlock(), "subtmp.bysub");
                }
                // (C1 - (X + C2)) -> (C1 - C2) - X
                else if(auto crhsRhs = dynamic_cast<ConstantInt*>(rhs->getRhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsRhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Sub, 
                                            ConstantInt::get(32, C1 - C2), rhs->getRhs(), 
                                            sub->getBasicBlock(), "subtmp.bysub");
                }
            }
            else if(rhs->getKind() == Instruction::InstrKind::Sub)
            {
                // (C1 - (C2 - X)) ->  (C1 - C2) + X
                if(auto crhsLhs = dynamic_cast<ConstantInt*>(rhs->getLhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsLhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Add, 
                                            ConstantInt::get(32, C1 - C2), rhs->getRhs(), 
                                            sub->getBasicBlock(), "addtmp.bysub");
                }
                // (C1 - (X - C2)) -> (C1 + C2) - X
                else if(auto crhsRhs = dynamic_cast<ConstantInt*>(rhs->getRhs()))
                {
                    int C1 = clhs->getValue();
                    int C2 = crhsRhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Sub, 
                                            ConstantInt::get(32, C1 + C2), rhs->getLhs(), 
                                            sub->getBasicBlock(), "subtmp.bysub");
                }
            }
        }
    }
    else if(auto crhs = dynamic_cast<ConstantInt*>(sub->getRhs()))
    {
        if(auto lhs = dynamic_cast<BinaryInst*>(sub->getLhs()))
        {
            if(lhs->getKind() == Instruction::InstrKind::Add)
            {
                // ((C1 + X) - C2) -> X + (C1 - C2)
                if(auto clhsLhs = dynamic_cast<ConstantInt*>(lhs->getLhs()))
                {
                    int C1 = clhsLhs->getValue();
                    int C2 = crhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Add, 
                                            lhs->getRhs(), ConstantInt::get(32, C1 - C2), 
                                            sub->getBasicBlock(), "addtmp.bysub");
                }
                // ((X + C1) - C2)) -> X + (C1 - C2)
                else if(auto clhsRhs = dynamic_cast<ConstantInt*>(lhs->getRhs()))
                {
                    int C1 = clhsRhs->getValue();
                    int C2 = crhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Add, 
                                            lhs->getLhs(), ConstantInt::get(32, C1 - C2), 
                                            sub->getBasicBlock(), "addtmp.bysub");
                }
            }
            else if(lhs->getKind() == Instruction::InstrKind::Sub)
            {
                // ((C1 - X) - C2) ->  (C1 - C2) - X
                if(auto clhsLhs = dynamic_cast<ConstantInt*>(lhs->getLhs()))
                {
                    int C1 = clhsLhs->getValue();
                    int C2 = crhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Sub, 
                                            ConstantInt::get(32, C1 - C2), lhs->getRhs(), 
                                            sub->getBasicBlock(), "subtmp.bysub");
                }
                // ((X - C1) - C2) -> X - (C1 + C2)
                else if(auto clhsRhs = dynamic_cast<ConstantInt*>(lhs->getRhs()))
                {
                    int C1 = clhsRhs->getValue();
                    int C2 = crhs->getValue();
                    return new BinaryInst(sub->getType(), Instruction::InstrKind::Sub, 
                                            lhs->getLhs(), ConstantInt::get(32, C2 + C1), 
                                            sub->getBasicBlock(), "subtmp.bysub");
                }
            }
        }
    }
    
    if(auto mulLhs = dynamic_cast<BinaryInst*>(sub->getLhs()); mulLhs && mulLhs->getKind() == Instruction::InstrKind::Mul)
    {
        auto lhsLhs = mulLhs->getLhs();
        auto lhsRhs = mulLhs->getRhs();
        // X * C - X -> X * (C - 1)
        if(lhsLhs == sub->getRhs())
        {   
            if(auto clhsRhs = dynamic_cast<ConstantInt*>(lhsRhs))
            {
                auto C = clhsRhs->getValue();
                return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                lhsLhs, ConstantInt::get(32, C - 1),
                                sub->getBasicBlock(), "multmp.bysub");
            }
        }
        // C * X - X -> X * (C - 1)
        if(lhsRhs == sub->getRhs())
        {
            if(auto clhsLhs = dynamic_cast<ConstantInt*>(lhsLhs))
            {
                auto C = clhsLhs->getValue();
                return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                lhsRhs, ConstantInt::get(32, C - 1),
                                sub->getBasicBlock(), "multmp.bysub");
            }
        }

        // X * C1 - X * C2 -> X * (C1 - C2)
        if(auto mulRhs = dynamic_cast<BinaryInst*>(sub->getRhs()); mulRhs && mulLhs->getKind() == Instruction::InstrKind::Mul)
        {
            auto rhsLhs = mulRhs->getLhs();
            auto rhsRhs = mulRhs->getRhs();

            if(lhsLhs == rhsLhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsRhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsRhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                lhsLhs, ConstantInt::get(32, C1 - C2),
                                sub->getBasicBlock(), "multmp.bysub");
                    }
            }
            if(lhsLhs == rhsRhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsRhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsLhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                lhsLhs, ConstantInt::get(32, C1 - C2),
                                sub->getBasicBlock(), "multmp.bysub");
                    }
            }
            if(lhsRhs == rhsLhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsLhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsRhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                lhsRhs, ConstantInt::get(32, C1 - C2),
                                sub->getBasicBlock(), "multmp.bysub");
                    }
            }
            if(lhsRhs == rhsRhs)
            {
                if(auto c1 = dynamic_cast<ConstantInt*>(lhsLhs))
                    if(auto c2 = dynamic_cast<ConstantInt*>(rhsLhs))
                    {
                        auto C1 = c1->getValue();
                        auto C2 = c2->getValue();

                        return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                lhsRhs, ConstantInt::get(32, C1 - C2),
                                sub->getBasicBlock(), "multmp.bysub");
                    }
            }
        }
    }
    if(auto mulRhs = dynamic_cast<BinaryInst*>(sub->getRhs()); mulRhs && mulRhs->getKind() == Instruction::InstrKind::Mul)
    {
        auto rhsLhs = mulRhs->getLhs();
        auto rhsRhs = mulRhs->getRhs();
        // X - X * C -> X * (1 - C)
        if(rhsLhs == sub->getLhs())
        {
            if(auto crhsRhs = dynamic_cast<ConstantInt*>(rhsRhs))
            {
                auto C = crhsRhs->getValue();
                return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                rhsLhs, ConstantInt::get(32, 1 - C),
                                sub->getBasicBlock(), "multmp.byadd");
            }
        }
        // X - C * X -> X * (C + 1)
        if(rhsRhs == sub->getLhs())
        {
            if(auto crhsLhs = dynamic_cast<ConstantInt*>(rhsLhs))
            {
                auto C = crhsLhs->getValue();
                return new BinaryInst(sub->getType(), Instruction::InstrKind::Mul,
                                rhsRhs, ConstantInt::get(32, 1 - C),
                                sub->getBasicBlock(), "multmp.byadd");
            }
        }
    }


    return sub;
}

void InstructionCombine::run(Function* func)
{
    PrintMsg("InstructionCombine");
    for(auto bb : func->getBasicBlockList())
    {
        for(auto iter = bb->getInstructions().begin(); iter != bb->getInstructions().end(); iter++)
        {
            if(auto binary = dynamic_cast<BinaryInst*>(*iter))
            {
                if(auto bin = simplifyInstruction(binary); bin != binary)
                {
                    binary->replaceAllUseWith(bin);
                    binary->UnUseAll();
                }
                else if(auto simple = visitBinary(binary); simple != binary)
                {
                    // PrintMsg("replace " + binary->getSignature());
                    // PrintMsg("\tto" + simple->getSignature());
                    binary->replaceAllUseWith(simple);
                    binary->UnUseAll();
                    *iter = simple;
                }
            }
        }
    }
}

} // namespace ir

}