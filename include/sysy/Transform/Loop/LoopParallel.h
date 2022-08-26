#ifndef SYSY_TRANSFORM_LOOPPARALLEL_H
#define SYSY_TRANSFORM_LOOPPARALLEL_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Loop.h"
namespace sysy
{
namespace ir
{

class LoopParallel : public FunctionPass
{
private:
    bool isParallelizable(Loop* loop);
    void runOnLoop(Loop* loop);
public:
    LoopParallel();
    void run(Function* func) override;
    std::string getName() override { return "LoopParallel"; }
};

} // namespace ir
    
} // namespace sysy


#endif