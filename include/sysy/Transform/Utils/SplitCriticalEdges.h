#ifndef SYSY_TRANSFORM_SPLITCRITICALEDGE_H
#define SYSY_TRANSFORM_SPLITCRITICALEDGE_H
#include "sysy/IR/PassManager.h"

namespace sysy
{
namespace ir
{

class SplitCriticalEdge : public FunctionPass
{
private:
    bool isNeedToCut(BasicBlock* pre, BasicBlock* bb);
    void cutKeyEdge(BasicBlock* from, BasicBlock* to, Function* func);
public:
    void run(Function* func) override;
    std::string getName() override { return "SplitCriticalEdge"; }
};

} // namespace ir

} // namespace sysy


#endif