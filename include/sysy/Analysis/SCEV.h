#ifndef SYSY_ANALYSIS_SCEV_H
#define SYSY_ANALYSIS_SCEV_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Loop.h"
namespace sysy
{
namespace ir
{

class ScalarEvolution : public FunctionPass
{
private:
    bool fixed;
    void linkInstructionToSCEV(Loop* loop, BinaryInst* instr);

    void runOnLoop(Loop* loop);
    void findAndRegisterBIV(Loop* loop);
    void registerBIV(Loop* loop, PhiInst* phi);
    void BIV2SCEV(Loop* loop, PhiInst* phi, std::set<Value*>& adds, std::set<Value*>& subs);
public:
    void run(Function* func) override;
    std::string getName() override { return "SCEV"; }
};

class ScalarEvolutionClean : public FunctionPass
{
private:
    void runOnLoop(Loop* loop);
public:
    void run(Function* func) override;
    std::string getName() override { return "SCEV-Clean"; }    
};

}    
} // namespace sysy


#endif