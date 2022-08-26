#ifndef SYSY_TRANSFORM_ELIMINATEPHI_H
#define SYSY_TRANSFORM_ELIMINATEPHI_H
#include "sysy/IR/PassManager.h"
namespace sysy
{
namespace ir
{

class UnsetSSA : public ModulePass
{
public:
    void run(Module* func) override;
    std::string getName() override { return "UnsetSSA"; }
};

class EliminatePhi : public FunctionPass
{
private:
    bool isNeedToCut(BasicBlock* pre, BasicBlock* bb);
    void cutKeyEdge(BasicBlock* from, BasicBlock* to, Function* func);
public:
    EliminatePhi();
    void run(Function* func) override;
    std::string getName() override { return "EliminatePhi"; }
};

} // namespace ir
    
} // namespace sysy


#endif