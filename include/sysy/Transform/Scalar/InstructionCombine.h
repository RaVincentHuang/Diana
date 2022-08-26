#ifndef SYSY_INSTRUCTIONCOMBINE_H
#define SYSY_INSTRUCTIONCOMBINE_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"

namespace sysy
{
namespace ir
{

class InstructionCombine : public FunctionPass
{
private:
    Instruction* visitBinary(BinaryInst* binary);
    Instruction* visitAdd(BinaryInst* add);
    Instruction* visitSub(BinaryInst* sub);
public:
    void run(Function* func) override;
    std::string getName() override { return "IC"; }
};

};
};


#endif