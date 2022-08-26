#ifndef SYSY_ANALYSIS_POSTDOMINFO_H
#define SYSY_ANALYSIS_POSTDOMINFO_H
#include "sysy/IR/PassManager.h"
namespace sysy
{

namespace ir
{

class PostDomInfo : public FunctionPassManager
{
public:
    PostDomInfo();
    std::string getName() override { return "PostDomInfo"; }
};

class IpdomGen : public FunctionPass
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

class PostDomTreeGen : public FunctionPass
{
private:
    void computePostDomLevel(BasicBlock* bb, int level);
    void computeAllPostDom(BasicBlock* bb);
public:
    void run(Function* func) override;
    std::string getName() override { return "pdomTree"; }
};

class PostDomFrontierGen : public FunctionPass
{
private:
    void computeRDF(BasicBlock* bb);
public:
    void run(Function* func) override;
    std::string getName() override { return "pdomFrontier"; }
};


} // namespace ir


} // namespace sysy


#endif