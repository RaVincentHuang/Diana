#include "sysy/IR/Instructions.h"
#include "sysy/IR/Function.h"
#include "sysy/Support/debug.h"
#include <algorithm>

namespace sysy
{
namespace ir
{
int Instruction::getRank() { return father->getRank(this); }

bool Instruction::isReverse(Instruction* x, Instruction* y)
{
    if(x->getKind() == ICmpSGE && y->getKind() == ICmpSLE
        || x->getKind() == ICmpSLE && y->getKind() == ICmpSGE)
        return true;
    else if(x->getKind() == ICmpSGT && y->getKind() == ICmpSLT
        || x->getKind() == ICmpSLT && y->getKind() == ICmpSGT)
        return true;
    else if(x->getKind() == FCmpUGE && y->getKind() == FCmpULE
        || x->getKind() == FCmpULE && y->getKind() == FCmpUGE)
        return true;
    else if(x->getKind() == FCmpUGT && y->getKind() == FCmpULT
        || x->getKind() == FCmpULT && y->getKind() == FCmpUGT)
        return true;
    else
        return false;
}

CallInstruction::CallInstruction(Function* _callee, std::vector<Value*> _args, BasicBlock* _father, std::string _name)
    : TerminatorInstruction(_callee->getReturnType(), Call, _father, _name) 
{
    addOperand(_callee);
    for(auto arg : _args)
    {
        addOperand(arg);
    }
}

Function* CallInstruction::getCallee() 
{ 
    return dynamic_cast<Function*>(operands.at(0)->getValue()); 
}

CallInstruction::argRange CallInstruction::getArgs() 
{ 
    return make_range(operands.begin() + 1, operands.end()); 
}

Value* CallInstruction::getArgs(unsigned index) 
{ 
    return operands.at(index + 1)->getValue(); 
}

bool CallInstruction::isPureCall()
{
    auto func = getCallee();
    if(func->hasSideEffect()) 
        return false;
    for(auto arg : getArgs())
    {
        if(arg->getType()->isPointerTy())
            return false;
        if(auto gep = dynamic_cast<GEPInst*>(arg))
            return false;
    }
    return true;
}

BrInstruction::BrInstruction(Value* _cond, BasicBlock* _trueBlock, BasicBlock* _falseBlock, BasicBlock* _father)
    : TerminatorInstruction(Type::getVoidTy(), Br, _father), 
        unconditional(false) 
{
    addOperand(_cond);
    addOperand(_trueBlock);
    addOperand(_falseBlock);
}

BrInstruction::BrInstruction( BasicBlock* _trueBlock, BasicBlock* _father) // TODO change to constant i1 true
        : TerminatorInstruction(Type::getVoidTy(), CondBr, _father), 
            unconditional(true) 
{
    addOperand(ConstantInt::get(1, 1));
    addOperand(_trueBlock);
}

BasicBlock* BrInstruction::getTrueBlock() 
{ 
    return dynamic_cast<BasicBlock*>(operands.at(1)->getValue()); 
}

BasicBlock* BrInstruction::getFalseBlock()
{ 
    return dynamic_cast<BasicBlock*>(operands.at(2)->getValue()); 
}

void BrInstruction::replaceTarget(BasicBlock* from, BasicBlock* to)
{
    if(getTrueBlock() == from)
        replaceOperandWith(1, to);
    if(!unconditional && getFalseBlock() == from)
        replaceOperandWith(2, to);
}

void BrInstruction::setUnconditional(BasicBlock* to)
{
    replaceOperandWith(1, to);
    if(!unconditional)
    {
        unconditional = true;
        auto falseEdge = operands.at(2);
        falseEdge->getValue()->removeUse(falseEdge);
        operands.pop_back();

        replaceOperandWith(0, ConstantInt::get(1, 1));
    }
}

void BrInstruction::setConditional(Value* cond, BasicBlock* trueBB, BasicBlock* falseBB)
{
    replaceOperandWith(0, cond);
    replaceOperandWith(1, trueBB);
    if(unconditional)
    {
        unconditional = false;
        addOperand(falseBB);
    }
    else
        replaceOperandWith(2, falseBB);

}

GEPInstruction::GEPInstruction(Type* _type, Value* _ptr, std::vector<Value*> _idxList, BasicBlock* _father, std::string _name)
    : MemoryInstruction(_type, InboundGEP, _father, _name) 
{
    addOperand(_ptr);
    for(auto item : _idxList)
        addOperand(item);
}

bool GEPInstruction::hasAllZeroIndices()
{
    return std::all_of(getIndex().begin(), getIndex().end(), [](Value* val) -> bool {
        if(auto cons = dynamic_cast<Constant*>(val))
            return cons->isZero();
        return false;
    });
}

GEPInstruction::indexRange GEPInstruction::getIndex() 
{ 
    return make_range(operands.begin() + 1, operands.end()); 
}

PhiInstruction::PhiInstruction(Type* _type, std::vector<ValueBlockPair> _predecessors, BasicBlock* _father)
        : MemoryInstruction(_type, Phi, _father) 
{
    for(auto [val, bb] : _predecessors)
    {
        addOperand(val);
        addOperand(bb);
    }
}

PhiInstruction::PhiInstruction(Type* _type, BasicBlock* _father, std::string name)
    : MemoryInstruction(_type, Phi, _father, name) {}

void PhiInstruction::setInComingValue(Value* val, BasicBlock* bb)
{
    addOperand(val);
    addOperand(bb);
}

PhiInstruction::indexRange PhiInstruction::getPredecessors() 
{ 
    return make_range(operands.begin(), operands.end()); 
}

PhiInstruction::ValueBlockPair PhiInstruction::PhiIterator::operator*() 
{
    Value* val = (*_M_current)->getValue();
    BasicBlock* bb = dynamic_cast<BasicBlock*>((*(_M_current + 1))->getValue());
    auto res = std::make_pair(val, bb); 
    return res;
}

PhiInstruction::ValueBlockPair PhiInstruction::getPredecessors(unsigned i)
{
    unsigned idx_val = i * 2, idx_bb = i * 2 + 1;
    Value* val = getOprands().at(idx_val)->getValue();
    BasicBlock* bb = dynamic_cast<BasicBlock*>(getOprands().at(idx_bb)->getValue());
    return std::make_pair(val, bb);
}

void PhiInstruction::setIncomingBB(unsigned index, BasicBlock* bb)
{
    unsigned idx_bb = index * 2 + 1;
    replaceOperandWith(idx_bb, bb);
}

void PhiInstruction::removeIncomingByBB(BasicBlock* bb)
{
    int indexToRemove = -1;
    for(int i = 0; i < size(); i++)
    {
        auto predUse = getPredecessors(i);
        if(predUse.second == bb)
            indexToRemove = i;
    }

    if(indexToRemove >= 0)
    {
        auto use1 = operands.at(indexToRemove * 2);
        use1->getValue()->getUses().remove(use1);
        auto use2 = operands.at(indexToRemove * 2 + 1);
        use2->getValue()->getUses().remove(use2);

        operands.erase(operands.begin() + indexToRemove * 2);
        operands.erase(operands.begin() + indexToRemove * 2);

        for(auto i = 0; i < operands.size(); i++)
            operands.at(i)->reSetPos(i);
    }

}

MemPhiInstruction::MemPhiInstruction(Type* _type, BasicBlock* _father, std::string name)
    : MemoryInstruction(_type, MemPhi, _father, name) {}
MemPhiInstruction::MemPhiInstruction(Type* _type, std::vector<ValueBlockPair> _predecessors, BasicBlock* _father)
    : MemoryInstruction(_type, Phi, _father, name) 
{
    for(auto [val, bb] : _predecessors)
    {
        addOperand(val);
        addOperand(bb);
    }
}

void MemPhiInstruction::setInComingValue(Value* val, BasicBlock* bb)
{
    addOperand(val);
    addOperand(bb);
}

void MemPhiInstruction::setIncomingBB(unsigned index, BasicBlock* bb)
{
    unsigned idx_bb = index * 2 + 1;
    replaceOperandWith(idx_bb, bb);
}

MemPhiInstruction::indexRange MemPhiInstruction::getPredecessors()
{
    return make_range(operands.begin(), operands.end()); 
}

MemPhiInstruction::ValueBlockPair MemPhiInstruction::getPredecessors(unsigned i)
{
    unsigned idx_val = i * 2, idx_bb = i * 2 + 1;
    Value* val = getOprands().at(idx_val)->getValue();
    BasicBlock* bb = dynamic_cast<BasicBlock*>(getOprands().at(idx_bb)->getValue());
    return std::make_pair(val, bb);
}

MemPhiInstruction::ValueBlockPair MemPhiInstruction::MemPhiIterator::operator*()
{
    Value* val = (*_M_current)->getValue();
    BasicBlock* bb = dynamic_cast<BasicBlock*>((*(_M_current + 1))->getValue());
    auto res = std::make_pair(val, bb); 
    return res;
}

Value* LoadInst::getStore()
{
    if(operands.size() < 2 || !operands.at(1))
        return nullptr;
    return operands.at(1)->getValue();
}

void LoadInst::setStore(Value* store)
{
    if(operands.size() > 1)
        replaceOperandWith(1, store);
    else
        addOperand(store);
}

void LoadInst::removeStore()
{
    if(operands.size() < 2)
        return;
    if(auto& use = operands.at(1))
        use->getValue()->removeUse(use);
    operands.pop_back();
}

MoveInstruction::MoveInstruction(PhiInstruction* phi, Value* IncomingVal, BasicBlock* bb)
    : Instruction(Type::getVoidTy(), Move, bb)
{
    addOperand(phi);
    addOperand(IncomingVal);
}

DeadInstruction::DeadInstruction(BasicBlock* bb, LoadInst* load) : Instruction(Type::getVoidTy(), Dead, bb) 
{
    addOperand(load);
}

} // namespace ir
    
} // namespace sysy