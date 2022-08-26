#ifndef SYSY_TRANSFORM_OPT_H
#define SYSY_TRANSFORM_OPT_H
#include "sysy/IR/PassManager.h"
#include <string>
namespace sysy
{

namespace ir
{

void initialIRTransform(std::string opt, ModulePassManager* pm);

class ScalarPass : public ModulePass
{
private:
    bool isAfterLoopOpt;
public:
    ScalarPass(bool after);
    void run(Module* module) override;
    std::string getName() override { return "Scalar"; } 
};

class LinearScalarPass : public ModulePass
{
public:
    void run(Module* module) override;
    std::string getName() override { return "Scalar"; } 
};

class LoopVariablePass : public ModulePass
{
public:
    LoopVariablePass();
    void run(Module* module) override;
    std::string getName() override { return "Loop"; } 
};

class LoopEarlyPass : public ModulePass
{
public:
    LoopEarlyPass();
    void run(Module* module) override;
    std::string getName() override { return "Loop"; } 
};

} // namespace ir


} // namespace sysy


#endif