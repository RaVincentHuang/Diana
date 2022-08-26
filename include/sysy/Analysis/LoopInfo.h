#ifndef SYSY_ANALYSIS_LOOPINFO_H
#define SYSY_ANALYSIS_LOOPINFO_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Loop.h"
namespace sysy
{

namespace ir
{

class LoopInfo : public FunctionPass
{
private:
    void discoverAndMapSubloop(Loop* loop, Function* func, std::stack<BasicBlock*>& BackedgeTo);
    void computeExitingBlocks(Function* func);
    void computeExitBlocks(Function* func);
    void computePreHeader(Function* func);
    void computeLatch(Function* func);
    void computeLoopIndVar(Function* func);
public:
    LoopInfo();
    std::string getName() { return "LoopInfo"; }
    static int getLoopDepth(BasicBlock* bb);
    void run(Function* func) override;
};

} // namespace ir


} // namespace sysy

#endif