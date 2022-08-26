#ifndef SYSY_TRANSFORM_LSR_H
#define SYSY_TRANSFORM_LSR_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Loop.h"
namespace sysy
{

namespace ir
{

class LSR : public FunctionPass
{
private:
    std::vector<BinaryInst*> Multiplies;
    void identifyAllInductiveMultiplication(Loop* loop);
    void eraseSingleMul(Loop* loop, BinaryInst* mul);
    void runOnLoop(Loop* loop);
public:
    LSR();
    void run(Function* func) override;
    std::string getName() override { return "LSR"; }
};

} // namespace ir


} // namespace sysy


#endif