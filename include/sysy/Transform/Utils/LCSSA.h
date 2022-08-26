#ifndef SYSY_TRANSFORM_LCSSA_H
#define SYSY_TRANSFORM_LCSSA_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Loop.h"
#include <unordered_set>
namespace sysy
{
namespace ir
{

class LCSSA : public FunctionPass
{
private:    // TODO Think if or not use the LoopPass to repalce it later.
    void runOnLoop(Loop* loop);
    void computeDefInUseOut(Loop* loop, std::unordered_set<Instruction*>& DefInUseOut);
    void insertLoopClosedPhi(Loop* loop, Instruction* instr);
    Value* getIncomingValAtBB(BasicBlock* bb, Instruction* instr, std::unordered_map<BasicBlock*, PhiInst*>& BBToPhi, Loop* loop);
public:
    LCSSA();
    void run(Function* func) override;
    std::string getName() override { return "LCSSA"; }
};

} // namespace ir
    
} // namespace sysy


#endif