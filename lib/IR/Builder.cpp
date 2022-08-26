#include "sysy/IR/Module.h"
#include "sysy/IR/Instruction.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/Builder.h"

namespace sysy
{
namespace ir
{

void IRBuilder::insertInstr(Instruction* instr)
{
    if(insertPoint == curBB->end())
        curBB->insertBack(instr), insertPoint = curBB->end();
    else
    {
        insertPoint = curBB->insertInstr(insertPoint, instr);
        insertPoint++;
    }
}

Value* IRBuilder::CreateInBoundsGEP(Type* _type, Value* ptr, const std::vector<Value*>& index, const std::string& name)
{
    Type* defType = _type;
    if(!defType->isPointerTy())
        defType = PointerType::get(defType);
    int cnt = 0;
    // PrintMsg("Type " + defType->getSignature() + " size " + std::to_string(index.size()));
    while(cnt < index.size())
    {
        defType = defType->getInnerType();
        cnt++;
    }
    auto instr = new GEPInstruction(PointerType::get(defType), ptr, index, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateSIToFP(Value* val, Type* destTy, const std::string& name) 
{
    auto instr = new UnaryInstruction(destTy, Instruction::SIToFP, val, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateFPToSI(Value* val, Type* destTy, const std::string& name) 
{
    auto instr = new UnaryInstruction(destTy, Instruction::FPToSI, val, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateBitCast(Value* val, Type* destTy, const std::string& name)
{
    auto instr = new UnaryInstruction(destTy, Instruction::BitCast, val, curBB, name);
    insertInstr(instr);
    return instr;
}

StoreInstruction* IRBuilder::CreateStore(Value* val, Value* ptr) 
{
    auto instr = new StoreInstruction(val, ptr, curBB);
    insertInstr(instr);
    return instr;
}

CallInstruction* IRBuilder::CreateCall(Function* callee, const std::vector<Value*> args, const std::string& name) 
{
    auto instr = new CallInstruction(callee, args, curBB, name);
    insertInstr(instr);
    return instr;
}

RetInstruction* IRBuilder::CreateRetVoid() 
{
    auto instr = new RetInstruction(curBB);
    insertInstr(instr);
    return instr;
}

RetInstruction* IRBuilder::CreateRet(Value* ret) 
{
    auto instr = new RetInstruction(ret, curBB);
    insertInstr(instr);
    return instr;
}

LoadInstruction* IRBuilder::CreateLoad(Type* type, Value* ptr, const std::string& name) 
{
    auto instr = new LoadInstruction(type, ptr, curBB, name);
    insertInstr(instr);
    return instr;
}

BrInstruction* IRBuilder::CreateCondBr(Value* cond, BasicBlock* trueBB, BasicBlock* falseBB) 
{
    auto instr = new BrInstruction(cond, trueBB, falseBB, curBB);
    insertInstr(instr);
    curBB->getSuccessor().push_back(trueBB);
    trueBB->getPredecessor().push_back(curBB);
    curBB->getSuccessor().push_back(falseBB);
    falseBB->getPredecessor().push_back(curBB);
    return instr;
}
BrInstruction* IRBuilder::CreateBr(BasicBlock* dest) 
{
    auto instr = new BrInstruction(dest, curBB);
    insertInstr(instr);
    curBB->getSuccessor().push_back(dest);
    dest->getPredecessor().push_back(curBB);
    return instr;
}

AllocaInstruction* IRBuilder::CreateAlloca(Type* type, const std::string& name)
{
    auto instr = new AllocaInstruction(type, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateAdd(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt32Ty(), Instruction::Add, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateSub(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt32Ty(), Instruction::Sub, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateMul(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt32Ty(), Instruction::Mul, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateSDiv(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt32Ty(), Instruction::SDiv, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateSRem(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt32Ty(), Instruction::SRem, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateICmpEQ(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::ICmpEQ, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
} 

Value* IRBuilder::CreateICmpNE(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::ICmpNE, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateICmpSLT(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::ICmpSLT, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}

Value* IRBuilder::CreateICmpSGT(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::ICmpSGT, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateICmpSLE(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::ICmpSLE, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateICmpSGE(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::ICmpSGE, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFAdd(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getFloatTy(), Instruction::FAdd, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFSub(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getFloatTy(), Instruction::FSub, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFMul(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getFloatTy(), Instruction::FMul, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFDiv(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getFloatTy(), Instruction::FDiv, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFRem(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getFloatTy(), Instruction::FRem, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFCmpUNE(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::FCmpUNE, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFCmpULT(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::FCmpULT, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFCmpUGT(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::FCmpUGT, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFCmpULE(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::FCmpULE, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFCmpUGE(Value* lhs, Value* rhs, const std::string& name) 
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::FCmpUGE, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFCmpUEQ(Value* lhs, Value* rhs, const std::string& name)
{
    auto instr = new BinaryInstrcution(Type::getInt1Ty(), Instruction::FCmpUEQ, lhs, rhs, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateNeg(Value* val, const std::string& name) 
{
    auto instr = new BinaryInst(Type::getInt32Ty(), Instruction::Sub, ConstantInt::get(32, 0), val, curBB, name); 
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateNot(Value* val, const std::string& name) 
{
    // auto instr = new UnaryInstruction(Type::getInt1Ty(), Instruction::Not, val, curBB, name);
    // insertInstr(instr);
    auto instr = CreateICmpEQ(val, ConstantInt::get(val->getType(), 0), name);
    return instr;
}
Value* IRBuilder::CreateFNeg(Value* val, const std::string& name) 
{
    auto instr = new BinaryInst(Type::getFloatTy(), Instruction::FSub, ConstantFP::get(0), val, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateSIToFP(Value* val, const std::string& name) 
{
    auto instr = new UnaryInstruction(Type::getFloatTy(), Instruction::SIToFP, val, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateFPToSI(Value* val, const std::string& name) 
{
    auto instr = new UnaryInstruction(Type::getInt32Ty(), Instruction::SIToFP, val, curBB, name);
    insertInstr(instr);
    return instr;
}
Value* IRBuilder::CreateMemCpyInline(Value* dst, unsigned dstAlign, Value* src, unsigned srcAlign, Value* size) 
{
    int cpySize = 4;
    Type* _type = dst->getType()->getInnerType();
    while(auto type = dynamic_cast<ArrayType*>(_type))
    {
        cpySize *= type->getSize();
        _type = type->getInnerType(); 
    }
    auto dstBit = CreateBitCast(dst, Type::getInt8PtrTy(), "bitdst");
    auto srcBit = CreateBitCast(src, Type::getInt8PtrTy(), "bitsrc");
    return CreateCall(Module::getInstance().getFunction("llvm.memcpy.inline.p0.p0.i32"), 
                        {dstBit, srcBit, ConstantInt::get(32, cpySize), ConstantInt::get(1, 0)});
}

} // namespace ir
    
} // namespace sysy
