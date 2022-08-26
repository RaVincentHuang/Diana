#ifndef SYSY_TRANSFORM_DCE_H
#define SYSY_TRANSFORM_DCE_H

#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"

namespace sysy
{
namespace ir
{
class LoaclDCE : public ModulePass
{
public:
    std::string getName() override { return "DCE"; }
    void run(Module* module) override;
};

class DCE : public FunctionPass
{
private:
    void removeUselessStore(BasicBlock* bb);
    bool isUsefulInstr(Instruction* instr);
    void findAliveCode(Instruction* instr);
public:
    void run(Function* func) override;
    std::string getName() override { return "DCE"; }
};
} // namespace ir
    
} // namespace sysy


#endif