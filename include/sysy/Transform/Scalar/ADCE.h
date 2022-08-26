#ifndef SYSY_TRANSFORM_ADCE_H
#define SYSY_TRANSFORM_ADCE_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"
namespace sysy
{
namespace ir
{

class ADCE : public FunctionPass
{
private:
    void removeUselessStore(BasicBlock* bb);
    bool isUsefulInstr(Instruction* instr);
    void findAliveCode(Instruction* instr);
public:
    ADCE();
    void run(Function* func) override;
    std::string getName() override { return "ADCE"; }
};

} // namespace ir
    
} // namespace sysy


#endif

