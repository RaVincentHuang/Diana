#ifndef SYSY_ANALYSIS_VERTIFY_H
#define SYSY_ANALYSIS_VERTIFY_H

#include "sysy/IR/PassManager.h"

namespace sysy
{
namespace ir
{

class ModuleVertify : public ModulePass
{
public:
    void run(Module* module) override;
    std::string getName() override { return "Vertify"; }
};

class FunctionVertify : public FunctionPass
{
public:
    void run(Function* func) override;
    std::string getName() override { return "Vertify"; }
};

// class BasicBlockVertify : public BasicBlockPass
// {
// public:
//     void run(BasicBlock* module) override;
//     std::string getName() { return "Vertify"; }
// };

} // namespace ir
} // namespace sysy


#endif