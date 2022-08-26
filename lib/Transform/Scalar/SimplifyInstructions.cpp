#include "sysy/Transform/Scalar/SimplifyInstructions.h"

namespace sysy
{
namespace ir
{
Value* simplifyInstruction(Instruction* instr)
{
    if(auto bin = dynamic_cast<BinaryInst*>(instr))
        return simplifyInstruction(bin);
    else if(auto unary = dynamic_cast<UnaryInst*>(instr))
        return simplifyInstruction(unary);
    // else if(auto alloca = dynamic_cast<AllocaInst*>(instr))
    //     return simplifyInstruction(alloca);
    // else if(auto gep = dynamic_cast<GEPInst*>(instr))
    //     return simplifyInstruction(gep);
    else if(auto phi = dynamic_cast<PhiInst*>(instr))
        return simplifyInstruction(phi);
    // else if(auto load = dynamic_cast<LoadInst*>(instr))
    //     return simplifyInstruction(load);
    // else if(auto call = dynamic_cast<CallInst*>(instr))
    //     return simplifyInstruction(call);
    else
        return instr;
}

using Kind = Instruction::InstrKind;

Value* simplifyInstruction(BinaryInst* bin)
{
    if(auto c = constantFold(bin))
        return c;
    auto lhs = bin->getLhs();
    auto rhs = bin->getRhs();
    switch (bin->getKind())
    {
    case Kind::Add:
        if(auto clhs = dynamic_cast<ConstantInt*>(lhs))
        {
            bin->UnUseAll();
            bin->addOperand(rhs);
            bin->addOperand(lhs);
            lhs = bin->getLhs();
            rhs = bin->getRhs();
        }
        // X + 0 = 0
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isZero())
            return lhs;
        // X + (0 - X) = 0; (0 - X) + X = 0
        if(auto slhs = dynamic_cast<BinaryInst*>(lhs); slhs && slhs->getKind() == Kind::Sub)
            if(auto clhsOfLhs = dynamic_cast<ConstantInt*>(slhs->getLhs()); clhsOfLhs && clhsOfLhs->isZero() && slhs->getRhs() == rhs)
            return ConstantInt::get(32, 0);
        else if(auto srhs = dynamic_cast<BinaryInst*>(rhs); srhs && srhs->getKind() == Kind::Sub)
            if(auto clhsOfRhs = dynamic_cast<ConstantInt*>(srhs->getLhs()); clhsOfRhs && clhsOfRhs->isZero() && srhs->getRhs() == lhs)
            return ConstantInt::get(32, 0);
        // (X1 - Y1) + (Y2 - X2) = 0
        if(auto slhs = dynamic_cast<BinaryInst*>(lhs); slhs && slhs->getKind() == Kind::Sub)
            if(auto srhs = dynamic_cast<BinaryInst*>(rhs); srhs && srhs->getKind() == Kind::Sub)
                if(slhs->getLhs() == srhs->getRhs() && slhs->getRhs() == srhs->getLhs())
                    return ConstantInt::get(32, 0);
        return bin;
    case Kind::Sub:
        // X - 0 = X
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isZero())
            return lhs;
        // 0 - (0 - X) =  X
        if(auto clhs = dynamic_cast<ConstantInt*>(lhs); clhs && clhs->isZero())
            if(auto srhs = dynamic_cast<BinaryInst*>(rhs); srhs && srhs->getKind() == Kind::Sub)
                if(auto clhsOfRhs = dynamic_cast<ConstantInt*>(srhs->getLhs()); clhsOfRhs && clhsOfRhs->isZero())
                    return srhs->getRhs();
        // X - X = 0
        if(lhs == rhs)
            return ConstantInt::get(32, 0);
        return bin;
    case Kind::Mul:
        if(auto clhs = dynamic_cast<ConstantInt*>(lhs))
        {
            bin->UnUseAll();
            bin->addOperand(rhs);
            bin->addOperand(lhs);
            lhs = bin->getLhs();
            rhs = bin->getRhs();
        }
        // X * 0 = 0
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isZero())
            return ConstantInt::get(32, 0);
        // X * 1 = X
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isOne())
            return lhs;
        return bin;
    case Kind::SDiv:
        // X / 1 = X
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isOne())
            return lhs;
        return bin;
    // case Kind::SRem:
    case Kind::ICmpEQ:
        if(lhs == rhs)
            return ConstantInt::get(1, 1);
        if(auto clhs = dynamic_cast<ConstantInt*>(lhs))
        {
            bin->UnUseAll();
            bin->addOperand(rhs);
            bin->addOperand(lhs);
            lhs = bin->getLhs();
            rhs = bin->getRhs();
        }
        // (i1 == 1) -> i1
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isOne())
            if(auto blhs = dynamic_cast<BinaryInst*>(lhs); blhs && blhs->isCmp())
                return blhs;
        return bin;
    case Kind::ICmpNE:
        if(lhs == rhs)
            return ConstantInt::get(1, 0);
        if(auto clhs = dynamic_cast<ConstantInt*>(lhs))
        {
            bin->UnUseAll();
            bin->addOperand(rhs);
            bin->addOperand(lhs);
            lhs = bin->getLhs();
            rhs = bin->getRhs();
        }
        // (i1 != 0) -> i1
        if(auto crhs = dynamic_cast<ConstantInt*>(rhs); crhs && crhs->isZero())
            if(auto blhs = dynamic_cast<BinaryInst*>(lhs); blhs && blhs->isCmp())
                return blhs;
        return bin;
    // TODO Standrad all instr to </<=
    // case Kind::ICmpSLT:
    // case Kind::ICmpSGT:
    // case Kind::ICmpSLE:
    // case Kind::ICmpSGE:
    // case Kind::AShr:
    // case Kind::LShr:
    // case Kind::Shl:
    // case Kind::FAdd:
    // case Kind::FSub:
    // case Kind::FMul:
    // case Kind::FDiv:
    // case Kind::FRem:
    // case Kind::FCmpUEQ:
    // case Kind::FCmpUNE:
    // case Kind::FCmpULT:
    // case Kind::FCmpUGT:
    // case Kind::FCmpULE:
    // case Kind::FCmpUGE:
    default:
        return bin;
    }
}
Value* simplifyInstruction(UnaryInst* unary)
{
    if(auto c = constantFold(unary))
        return c;
    auto val = unary->getValue();
    switch (unary->getKind())
    {
    case Kind::Neg:
        return new BinaryInst(Type::getInt32Ty(), Instruction::Sub, ConstantInt::get(32, 0), val, unary->getBasicBlock(), "subtmp");
    // case Kind::Not:
    case Kind::FNeg:
        return new BinaryInst(Type::getFloatTy(), Instruction::FSub, ConstantFP::get(0), val, unary->getBasicBlock(), "fsubtmp");
    // case Kind::FPToSI:
    // case Kind::SIToFP:
    default:
        return unary;
    }
}

Value* constantFold(BinaryInst* bin)
{
    if(bin->isInteger())
    {
        if(auto clhs = dynamic_cast<ConstantInt*>(bin->getLhs()))
            if(auto crhs = dynamic_cast<ConstantInt*>(bin->getRhs()))
            {
                int x = clhs->getValue(), y = crhs->getValue();
                switch (bin->getKind())
                {
                case Kind::Add:
                    return ConstantInt::get(32, x + y);
                case Kind::Sub:
                    return ConstantInt::get(32, x - y);
                case Kind::Mul:
                    return ConstantInt::get(32, x * y);
                case Kind::SDiv:
                    return ConstantInt::get(32, x / y);
                case Kind::SRem:
                    return ConstantInt::get(32, x % y);
                case Kind::ICmpEQ:
                    return ConstantInt::get(1, x == y);
                case Kind::ICmpNE:
                    return ConstantInt::get(1, x != y);
                case Kind::ICmpSLT:
                    return ConstantInt::get(1, x < y);
                case Kind::ICmpSGT:
                    return ConstantInt::get(1, x > y);
                case Kind::ICmpSLE:
                    return ConstantInt::get(1, x <= y);
                case Kind::ICmpSGE:
                    return ConstantInt::get(1, x >= y);
                default:
                    return nullptr;
                }
            }
        return nullptr;
    }
    else
    {
        if(auto clhs = dynamic_cast<ConstantFP*>(bin->getLhs()))
            if(auto crhs = dynamic_cast<ConstantFP*>(bin->getRhs()))
            {
                float x = clhs->getValue(), y = crhs->getValue();
                switch (bin->getKind())
                {
                case Kind::FAdd:
                    return ConstantFP::get(x + y);
                case Kind::FSub:
                    return ConstantFP::get(x - y);
                case Kind::FMul:
                    return ConstantFP::get(x * y);
                case Kind::FDiv:
                    return ConstantFP::get(x / y);
                case Kind::FRem:
                    return ConstantFP::get((int)x % (int)y);
                case Kind::FCmpUEQ:
                    return ConstantInt::get(1, x == y);
                case Kind::FCmpUNE:
                    return ConstantInt::get(1, x != y);
                case Kind::FCmpULT:
                    return ConstantInt::get(1, x < y);
                case Kind::FCmpUGT:
                    return ConstantInt::get(1, x > y);
                case Kind::FCmpULE:
                    return ConstantInt::get(1, x <= y);
                case Kind::FCmpUGE:
                    return ConstantInt::get(1, x >= y);
                default:
                    return nullptr;
                }
            }
        return nullptr;
    }
}

Value* constantFold(UnaryInst* unary)
{
    if(unary->isInteger())
    {
        if(auto c = dynamic_cast<ConstantInt*>(unary->getValue()))
        {
            switch (unary->getKind())
            {
            case Kind::Neg:
                return ConstantInt::get(32, - c->getValue());
            case Kind::Not:
                return ConstantInt::get(1, !c->getValue());
            case Kind::SIToFP:
                return ConstantFP::get(c->getValue());
            default:
                return nullptr;
            }
        }
        return nullptr;
    }
    else
    {
        if(auto c = dynamic_cast<ConstantFP*>(unary->getValue()))
        {
            if(unary->getKind() == Kind::FNeg)
                return ConstantFP::get(- c->getValue());
            else if(unary->getKind() == Kind::FPToSI)
                return ConstantInt::get(32, c->getValue());
            return nullptr;
        }
        return nullptr;
    }
}

// Value* simplifyInstruction(AllocaInst* alloca)
// {

// }
// Value* simplifyInstruction(GEPInst* gep)
// {

// }
Value* simplifyInstruction(PhiInst* phi)
{
    bool same = true;
    auto first = phi->getPredecessors(0).first;
    for(auto [value, bb] : phi->getPredecessors())
        if(value != first)
        {
            same = false;
            break;
        } 
    if(same)
        return first;
    return phi;
}
// Value* simplifyInstruction(LoadInst* load)
// {

// }
// Value* simplifyInstruction(CallInst* call)
// {

// }
} // namespace ir

} // namespace sysy
