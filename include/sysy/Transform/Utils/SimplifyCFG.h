#ifndef SYSY_TRANSFORM_SIMPLIFYCFG_H
#define SYSY_TRANSFORM_SIMPLIFYCFG_H

#include "sysy/IR/PassManager.h"

namespace sysy
{

namespace ir
{

class SimplifyCFG : public FunctionPass
{
private:
    bool isAfterLoopOpt;
    bool change;
    bool uncondTheCondBr(BasicBlock* bb);
    bool deleteEmptyBB(BasicBlock* bb);
    bool mergeBB(BasicBlock* bb);
    bool upperTheBranch(BasicBlock* bb);
    bool simplifyPhi(BasicBlock* bb);
public:
    SimplifyCFG(bool after);
    bool changed() { return change; }
    void run(Function* func) override;
    std::string getName() override { return "SimplifyCFG"; }
    static void deleteEdge(BasicBlock* from, BasicBlock* to);
    static void addEdge(BasicBlock* from, BasicBlock* to, BasicBlock* origion);
    static void redirectEdge(BasicBlock* preFrom, BasicBlock* nowFrom, BasicBlock* to);
    static void removeDeadBB(Function* func);
};

} // namespace ir


} // namespace sysy


#endif