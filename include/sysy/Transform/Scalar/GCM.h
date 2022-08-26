#ifndef SYSY_TRANSFORM_GCM_H
#define SYSY_TRANSFORM_GCM_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"
namespace sysy
{
namespace ir
{

class LocalGCM : public ModulePass
{
public:
    // LocalGCM();
    std::string getName() override { return "GCM"; }
    void run(Module* func) override;
};

class GCM : public FunctionPass
{
private:
    void schduleEarly(Instruction* instr);
    void schduleLate(Instruction* instr);
public:
    GCM();
    void run(Function* func) override;
    std::string getName() override { return "GCM"; }
};

} // namespace ir
    
} // namespace sysy


#endif