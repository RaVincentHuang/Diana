#ifndef SYSY_ANALYSIS_DOMINFO_H
#define SYSY_ANALYSIS_DOMINFO_H

#include "sysy/IR/PassManager.h"
#include <map>

namespace sysy
{
namespace ir
{

class DomInfo : public FunctionPassManager
{
public:
    DomInfo();
    std::string getName() override { return "DomInfo"; }
};

class IdomGen : public FunctionPass
{
private:
    void getDfnum(Function* func);
    void compress(BasicBlock* v);
    BasicBlock* eval(BasicBlock* v);
    void link(BasicBlock* pre, BasicBlock* now);
public:

    void run(Function* func) override;
    std::string getName() override;
};

class DomTreeGen : public FunctionPass
{
private:
    void computeDomLevel(BasicBlock* bb, int level);
    void computeAllDom(BasicBlock* bb);
public:
    void run(Function* func) override;
    std::string getName() override { return "domTree"; }
};

class DomFrontierGen : public FunctionPass
{
private:
    void computeDF(BasicBlock* bb);
public:
    void run(Function* func) override;
    std::string getName() override { return "domFrontier"; }
};



} // namespace ir
    
} // namespace sysy


#endif