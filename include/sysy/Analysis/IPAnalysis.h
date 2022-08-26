#ifndef SYSY_ANALYSIS_IPANALYSIS_H
#define SYSY_ANALYSIS_IPANALYSIS_H
#include "sysy/IR/PassManager.h"
namespace sysy
{
namespace ir
{

class IPAnalysis : public ModulePass
{
public:
    void run(Module* module) override;
    std::string getName() override { return "IPGVAnalysis"; }
};

} // namespace ir
} // namespace sysy


#endif