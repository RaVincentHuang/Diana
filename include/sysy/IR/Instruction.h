#ifndef SYSY_IR_INSTRUCTION_H
#define SYSY_IR_INSTRUCTION_H

#include "sysy/IR/User.h"

namespace sysy
{
namespace ir
{

class BasicBlock;

class Instruction : public User
{
public:
    enum InstrKind
    {
        // Binary
        Add = 0,
        Sub,
        Mul,
        SDiv,
        SRem,
        ICmpEQ,
        ICmpNE,
        ICmpSLT,
        ICmpSGT,
        ICmpSLE,
        ICmpSGE,
        AShr,
        LShr,
        Shl,
        FAdd,
        FSub,
        FMul,
        FDiv,
        FRem,
        FCmpUEQ,
        FCmpUNE,
        FCmpULT,
        FCmpUGT,
        FCmpULE,
        FCmpUGE,
        // Unary
        Neg,
        Not,
        FNeg,
        FPToSI,
        SIToFP,
        BitCast,
        //terminator
        CondBr,
        Br,
        Call,
        Ret,
        //mem op
        Alloca,
        Load,
        Store,
        InboundGEP,
        Zext,
        Phi,
        MemPhi,
        LoadDep,
        MemCopy,
        // vector op
        InsertEle,
        ExtractEle,
        
        Move,
        Dead
    };
private:
    InstrKind kind;
    BasicBlock* father;
public:
    Instruction(Type* _type, InstrKind _kind, BasicBlock* _father, std::vector<Value*> _oprands, std::string _name = "") 
        : User(_type, std::move(_oprands), _name), kind(_kind), father(_father) {}
    Instruction(Type* _type, InstrKind _kind, BasicBlock* _father, std::string _name = "")
        : User(_type, _name), kind(_kind), father(_father) {}

    InstrKind getKind() { return kind; }

    bool isInteger() { return (kind >= Add && kind <= Shl) || kind == Neg || kind == Not || kind == SIToFP; }
    bool isCmp() { return (kind >= ICmpEQ && kind <= ICmpSGE) || (kind >= FCmpUEQ && kind <= FCmpUGE) || kind == Not; }
    
    bool isTerminator() { return kind >= CondBr && kind <= Ret; }
    bool isCommutative() { return kind == Add || kind == Mul || kind == ICmpEQ || kind == ICmpNE 
                            || kind == FAdd || kind == FMul || kind == FCmpUEQ || kind == FCmpUNE; }
    static bool isReverse(Instruction* x, Instruction* y);

    BasicBlock* getBasicBlock() { return father; }
    BasicBlock* getBasicBlock() const { return father; }
    void reSetBasicBlock(BasicBlock* bb) { father = bb; }

    int getRank();

    virtual std::string getSignature() = 0;
};

} // namespace ir
    
} // namespace sysy



#endif