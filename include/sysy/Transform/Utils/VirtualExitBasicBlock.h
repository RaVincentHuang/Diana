#ifndef SYSY_TRANSFORM_VIRTUALEXITBB_H
#define SYSY_TRANSFORM_VIRTUALEXITBB_H
#include "sysy/IR/PassManager.h"
namespace sysy
{
namespace ir
{

class ConstructExitBasicBlock : public FunctionPass
{
public:
    void run(Function* func) override;
    std::string getName() override { return "ConstructExitBasicBlock"; }
};

class CleanExitBasicBlock : public FunctionPass
{
public:
    void run(Function* func) override;
    std::string getName() override { return "CleanExitBasicBlock"; }
};

} // namespace ir
    
} // namespace sysy



#endif
