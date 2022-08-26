#ifndef SYSY_IR_BUILDER_H
#define SYSY_IR_BUILDER_H

#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"


namespace sysy
{
namespace ir
{

class IRBuilder
{
private:
    BasicBlock* curBB;
    BasicBlock::iterator insertPoint;
public:
    IRBuilder(BasicBlock* BB = nullptr, BasicBlock::iterator _insertPoint = {}) : curBB(BB), insertPoint(_insertPoint) {}

    void SetInsertPoint(BasicBlock* BB, BasicBlock::iterator _insertPoint)
    { curBB = BB, insertPoint = _insertPoint; }

    void SetInsertPoint(BasicBlock* BB) { curBB = BB, insertPoint = BB->end(); }

    BasicBlock* GetInsertBlock() { return curBB; }

    void insertInstr(Instruction* instr);

    Value* CreateInBoundsGEP(Type* _type, Value* ptr, const std::vector<Value*>& index, const std::string& name = "");

    Value* CreateSIToFP(Value* val, Type* destTy, const std::string& name = "");

    Value* CreateFPToSI(Value* val, Type* destTy, const std::string& name = "");

    StoreInstruction* CreateStore(Value* val, Value* ptr);

    CallInstruction* CreateCall(Function* callee, const std::vector<Value*> args = {}, const std::string& name = "");

    RetInstruction* CreateRetVoid();

    RetInstruction* CreateRet(Value* ret);

    LoadInstruction* CreateLoad(Type* type, Value* ptr, const std::string& name = "");

    BrInstruction* CreateCondBr(Value* cond, BasicBlock* trueBB, BasicBlock* falseBB);

    BrInstruction* CreateBr(BasicBlock* dest);

    AllocaInstruction* CreateAlloca(Type* type, const std::string& name = "");

    Value* CreateAdd(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateSub(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateMul(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateSDiv(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateSRem(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateICmpEQ(Value* lhs, Value* rhs, const std::string& name = ""); 

    Value* CreateICmpNE(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateICmpSLT(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateICmpSGT(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateICmpSLE(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateICmpSGE(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateFAdd(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFSub(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFMul(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFDiv(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFRem(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFCmpUNE(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFCmpULT(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFCmpUGT(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFCmpULE(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFCmpUGE(Value* lhs, Value* rhs, const std::string& name = "");
    
    Value* CreateFCmpUEQ(Value* lhs, Value* rhs, const std::string& name = "");

    Value* CreateNeg(Value* val, const std::string& name);

    Value* CreateNot(Value* val, const std::string& name);

    Value* CreateFNeg(Value* val, const std::string& name);

    Value* CreateSIToFP(Value* val, const std::string& name);

    Value* CreateFPToSI(Value* val, const std::string& name);

    Value* CreateBitCast(Value* val, Type* destTy, const std::string& name);

    Value* CreateMemCpyInline(Value* dst, unsigned dstAlign, Value* src, unsigned srcAlign, Value* size);
};

} // namespace ir
    
} // namespace sysy


#endif