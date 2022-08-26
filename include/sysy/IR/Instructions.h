#ifndef SYSY_IR_INSTRUCTIONS_H
#define SYSY_IR_INSTRUCTIONS_H

#include "sysy/IR/User.h"
#include "sysy/IR/Instruction.h"
#include "sysy/IR/Constant.h"
#include "sysy/Support/iterator_range.h"
#include <memory>

namespace sysy
{
namespace ir
{

class Function;

class BinaryInstrcution : public Instruction
{
public:
    BinaryInstrcution(Type* _type, InstrKind _kind, Value* _lhs, Value* _rhs, 
                        BasicBlock* _father, std::string _name = "")
        : Instruction(_type, _kind, _father, { _lhs, _rhs }, _name) {}

    Value* getLhs() { return operands.at(0)->getValue(); }
    Value* getRhs() { return operands.at(1)->getValue(); }

    std::string getSignature() override;
};
using BinaryInst = BinaryInstrcution;

class UnaryInstruction : public Instruction
{
private:
    // Value* val;
public:
    UnaryInstruction(Type* _type, InstrKind _kind, Value* _val, 
                        BasicBlock* _father, std::string _name = "")
        : Instruction(_type, _kind, _father, {_val }, _name) {}

    Value* getValue() { return operands.at(0)->getValue(); }
    
    std::string getSignature() override;
};
using UnaryInst = UnaryInstruction;

class TerminatorInstruction : public Instruction
{
public:
    TerminatorInstruction(Type* _type, InstrKind _kind, BasicBlock* _father, std::vector<Value*> _oprands, std::string _name = "") 
        : Instruction(_type, _kind, _father, std::move(_oprands), _name) {}
    TerminatorInstruction(Type* _type, InstrKind _kind, BasicBlock* _father, std::string _name = "")
        : Instruction(_type, _kind, _father, _name) {}
};

class CallInstruction : public TerminatorInstruction
{
public:
    class ArgIterator : public OprandList::iterator
    {
    public:
        ArgIterator(OprandList::iterator ord) : OprandList::iterator(ord) {}
        Value* operator*() { return (*_M_current)->getValue(); }
    };
    using argRange = iterator_range<ArgIterator>;

    CallInstruction(Function* _callee, std::vector<Value*> _args, BasicBlock* _father, std::string _name = ""); 

    Function* getCallee();

    argRange getArgs();

    Value* getArgs(unsigned index);

    size_t size() { return operands.size() - 1; }

    bool isPureCall();

    bool hasAlias = false;

    std::string getSignature() override;
};

using CallInst = CallInstruction;

class BrInstruction : public TerminatorInstruction
{
private:
    bool unconditional;
public:
    BrInstruction(Value* _cond, BasicBlock* _trueBlock, BasicBlock* _falseBlock, BasicBlock* _father);
    
    BrInstruction( BasicBlock* _trueBlock, BasicBlock* _father);

    Value* getCond() { if(!unconditional) return operands.at(0)->getValue(); else return nullptr; }

    void setUnconditional(BasicBlock* to);

    void setConditional(Value* cond, BasicBlock* trueBB, BasicBlock* falseBB);
    
    BasicBlock* getTrueBlock();

    BasicBlock* getFalseBlock();

    void replaceTarget(BasicBlock* from, BasicBlock* to);
    
    std::string getSignature() override;
};
using BrInst = BrInstruction;

class RetInstruction : public TerminatorInstruction
{
private:
    // Value* retValue;
public:
    RetInstruction(Value* _retValue, BasicBlock* _father) 
        : TerminatorInstruction(Type::getVoidTy(), Ret, _father, { _retValue }) {}
    RetInstruction(BasicBlock* _father) 
        : TerminatorInstruction(Type::getVoidTy(), Ret, _father) {}
    
    Value* getRet() 
    {
        if(operands.empty())
            return nullptr;
        return operands.at(0)->getValue(); 
    }

    bool isVoidRet() { return !getRet() || getRet()->getType()->isVoidTy(); }
    
    std::string getSignature() override;
};
using RetInst = RetInstruction;

class MemoryInstruction : public Instruction
{
public:
    MemoryInstruction(Type* _type, InstrKind _kind, BasicBlock* _father, std::vector<Value*> _oprands, std::string _name = "") 
        : Instruction(_type, _kind, _father, std::move(_oprands), _name) {}
    MemoryInstruction(Type* _type, InstrKind _kind, BasicBlock* _father, std::string _name = "")
        : Instruction(_type, _kind, _father, _name) {}
};

class StoreInstruction : public MemoryInstruction
{
private:
    // Value* val,* ptr;
public:
    StoreInstruction(Value* _val, Value* _ptr, BasicBlock* _father)
        : MemoryInstruction(Type::getVoidTy(), Store, _father, {_val, _ptr}, "") {}

    Value* getValue() { return operands.at(0)->getValue(); }
    Value* getPtr() { return operands.at(1)->getValue(); }

    bool hasAlias = false;

    std::string getSignature() override;
};
using StoreInst = StoreInstruction;

class LoadInstruction : public MemoryInstruction
{
private:
    // Value* ptr;
public:
    LoadInstruction(Type* _type, Value* _ptr, BasicBlock* _father, std::string _name = "")
        : MemoryInstruction(_type, Load, _father, {_ptr}, _name) {}
    
    Value* getPtr() { return operands.at(0)->getValue(); }
    
    Value* getStore();

    void setStore(Value* store);

    void removeStore();

    std::string getSignature() override;
};
using LoadInst = LoadInstruction;

class GEPInstruction : public MemoryInstruction
{
private:
    // Value* ptr;
    // std::vector<Value*> idxList;
public:
    GEPInstruction(Type* _type, Value* _ptr, std::vector<Value*> _idxList, BasicBlock* _father, std::string _name = "");
    
    Value* getPtr() { return operands.at(0)->getValue(); }
// %1 = gep 
    class IndexIterator : public OprandList::iterator
    {
    public:
        IndexIterator(OprandList::iterator ord) : OprandList::iterator(ord) {}
        Value* operator*() { return (*_M_current)->getValue(); }
    };
    using indexRange = iterator_range<IndexIterator>;

    Value* getIndex(unsigned i) { return operands.at(i + 1)->getValue(); }
    size_t size() { return operands.size() - 1; }

    indexRange getIndex();

    bool hasAllZeroIndices();

    std::string getSignature() override;
};
using GEPInst = GEPInstruction;

class AllocaInstruction : public MemoryInstruction
{
private:
    Type* allocaType;
public:
    AllocaInstruction(Type* _allocaType, BasicBlock* _father, std::string _name = "")
        : MemoryInstruction(Type::getPointerTy(_allocaType), Alloca, _father, _name), allocaType(_allocaType) {}

    Type* getAllocatedType() { return allocaType; }

    Value* getSize() 
    { 
        return ConstantInt::getZero(Type::getInt32Ty()); 
    }

    // for mem2reg.
    // std::list<StoreInst*> store;
    // std::list<LoadInst*> load;
    // std::set<BasicBlock*> block;

    std::string getSignature() override;
};
using AllocaInst = AllocaInstruction;

class PhiInstruction : public MemoryInstruction
{
private:
    using ValueBlockPair = std::pair<Value*, BasicBlock*>;
    // std::vector<ValueBlockPair> predecessors;
public:
    PhiInstruction(Type* _type, BasicBlock* _father, std::string name = "");
    PhiInstruction(Type* _type, std::vector<ValueBlockPair> _predecessors, BasicBlock* _father);

    class PhiIterator : public OprandList::iterator
    {
    public:
        PhiIterator(OprandList::iterator ord) : OprandList::iterator(ord) {}

        ValueBlockPair operator*();

        PhiIterator& operator++() { ++_M_current; ++_M_current; return *this; }
    };
    using indexRange = iterator_range<PhiIterator>;

    size_t size() { return getOprands().size() / 2; }

    void setInComingValue(Value* val, BasicBlock* bb);

    void setIncomingBB(unsigned index, BasicBlock* bb);

    void removeIncomingByBB(BasicBlock* bb);

    indexRange getPredecessors();

    ValueBlockPair getPredecessors(unsigned i);

    std::string getSignature() override;
};
using PhiInst = PhiInstruction;

class MemPhiInstruction : public MemoryInstruction
{
private:
    using ValueBlockPair = std::pair<Value*, BasicBlock*>;
    // std::vector<ValueBlockPair> predecessors;
public:
    MemPhiInstruction(Type* _type, BasicBlock* _father, std::string name = "");
    MemPhiInstruction(Type* _type, std::vector<ValueBlockPair> _predecessors, BasicBlock* _father);

    class MemPhiIterator : public OprandList::iterator
    {
    public:
        MemPhiIterator(OprandList::iterator ord) : OprandList::iterator(ord) {}

        ValueBlockPair operator*();

        MemPhiIterator& operator++() { ++_M_current; ++_M_current; return *this; }
    };
    using indexRange = iterator_range<MemPhiIterator>;

    size_t size() { return getOprands().size() / 2; }

    void setInComingValue(Value* val, BasicBlock* bb);

    void setIncomingBB(unsigned index, BasicBlock* bb);

    indexRange getPredecessors();

    ValueBlockPair getPredecessors(unsigned i);

    std::string getSignature() override;
};
using MemPhiInst = MemPhiInstruction;

class DeadInstruction : public Instruction
{
public:
    DeadInstruction(BasicBlock* bb) : Instruction(Type::getVoidTy(), Dead, bb) {}

    DeadInstruction(BasicBlock* bb, LoadInst* load);

    std::string getSignature() override;

};
using DeadInst = DeadInstruction;

class MoveInstruction : public Instruction
{
public:
    MoveInstruction(PhiInstruction* phi, Value* IncomingVal, BasicBlock* bb);
    Value* getTarget() { return getOprands().at(0)->getValue(); }
    Value* getIncomingVal() { return getOprands().at(1)->getValue(); }
    std::string getSignature() override;
};
using MoveInst = MoveInstruction;

} // namespace ir
    
} // namespace sysy



#endif