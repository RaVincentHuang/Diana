#ifndef SYSY_TRANSFORM_FUNCTIONINLINE_H
#define SYSY_TRANSFORM_FUNCTIONINLINE_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"
namespace sysy
{
namespace ir
{

class FunctionInline : public ModulePass
{
private:
    void inlineAtCall(CallInst* call);
public:
    FunctionInline();
    void run(Module* moudle) override;
    std::string getName() override { return "FunctionInline"; }
};

} // namespace ir
    
} // namespace sysy


#endif