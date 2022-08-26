#ifndef SYSY_TRANSFORM_GVN_H
#define SYSY_TRANSFORM_GVN_H
#include "sysy/IR/Instructions.h"
#include "sysy/IR/PassManager.h"
namespace sysy
{
namespace ir
{

class LocalGVN : public ModulePass
{
public:
    LocalGVN();
    std::string getName() override { return "GVN"; }
    void run(Module* func) override;
};

// need ssa
class GVN : public FunctionPass
{
private:
    void doPerInstr(Instruction* instr);
    Value* checkoutTable(Value* val);
    Value* getValueNumber(Instruction* instr);

    Value* getValueNumber(BinaryInst* binary);
    Value* getValueNumber(UnaryInst* unary);
    Value* getValueNumber(PhiInst* phi);
    Value* getValueNumber(GEPInst* gep);
    Value* getValueNumber(CallInst* call);
    Value* getValueNumber(LoadInst* load);
public:
    GVN();
    std::string getName() override { return "GVN"; }
    void run(Function* func) override;
};

} // namespace ir
    
} // namespace sysy


#endif