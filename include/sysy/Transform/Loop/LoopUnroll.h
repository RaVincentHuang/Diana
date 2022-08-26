#ifndef SYSY_TRANSFORM_LOOPUNROLL_H
#define SYSY_TRANSFORM_LOOPUNROLL_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Loop.h"
namespace sysy
{
namespace ir
{

class LoopUnrollAll : public FunctionPass
{
private:
    void runOnLoop(Loop* loop);
public:
    LoopUnrollAll();
    void run(Function* func) override;
    std::string getName() override { return "LoopUnrollAll"; }
};


} // namespace ir
    
} // namespace sysy


#endif