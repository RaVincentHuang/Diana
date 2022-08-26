#ifndef SYSY_TRANSFORM_SIMPLIFYINSTR_H
#define SYSY_TRANSFORM_SIMPLIFYINSTR_H
#include "sysy/IR/Instruction.h"
#include "sysy/IR/Instructions.h"
namespace sysy
{

namespace ir
{

Value* simplifyInstruction(Instruction* instr);

Value* simplifyInstruction(BinaryInst* bin);

Value* simplifyInstruction(UnaryInst* unary);

Value* constantFold(BinaryInst* bin);
Value* constantFold(UnaryInst* unary);
// Value* simplifyInstruction(AllocaInst* alloca);
// Value* simplifyInstruction(GEPInst* gep);
Value* simplifyInstruction(PhiInst* phi);
// Value* simplifyInstruction(LoadInst* load);
// Value* simplifyInstruction(CallInst* call);

} // namespace ir


} // namespace sysy


#endif