#ifndef SYSY_IR_PASSMANAGER_H
#define SYSY_IR_PASSMANAGER_H

#include "sysy/Support/StrUtils.h"
#include <iostream>
#include <vector>
#include <functional>
#include <stack>

namespace sysy
{

template<typename Opt>
class Pass
{
private:
    std::vector<Pass<Opt>*> relatedPasses;
    std::vector<Pass<Opt>*> passFinishBy;
public:
    virtual ~Pass() {}
    virtual void run(Opt opt) = 0;
    virtual std::string getName() = 0;

    using DfsFunc = std::function<void(Pass<Opt>*)>;
    static void relatedDfs(Pass<Opt>* pass, DfsFunc func)
    {
        for(auto pre : pass->getRelated())
            relatedDfs(pre, func);
        func(pass);
    }

    static void finishByDfs(Pass<Opt>* pass, DfsFunc func)
    {
        func(pass);
        for(auto post : pass->getFinish())
            finishByDfs(post, func);
    }

    std::vector<Pass<Opt>*>& getRelated() { return relatedPasses; }
    std::vector<Pass<Opt>*>& getFinish() { return passFinishBy; }
};

template<typename Opt>
class PassManager : public Pass<Opt>
{
protected:
    std::vector<Pass<Opt>*> passes;
public:
    virtual ~PassManager() {}
    void addPass(Pass<Opt>* pass)
    {
        Pass<Opt>::relatedDfs(pass, [this](Pass<Opt>* pass) -> void {
            this->passes.push_back(pass);
        });
        for(auto post : pass->getFinish())
            Pass<Opt>::finishByDfs(post, [this](Pass<Opt>* post) -> void {
                this->passes.push_back(post);
            });
    }

    std::string getName() { return "passManager"; }

    void run(Opt opt)
    {
        for(auto pass : passes) {
            // std::cout << "here\n";
            // std::cout << pass->getName() << "\n";
            pass->run(opt);
        }
    }

    std::string getPassChain()
    {
        std::string str = "";
        for(auto pass : passes)
            if(auto pm = dynamic_cast<PassManager<Opt>*>(pass))
            {
                str += "PassManager: " + getName();
                str += subLine(pm->getPassChain());
            }
            else
                str += "\nPass: " + getName();
        return str;  
    }

    void dumpPass()
    {
        std::cout << getPassChain() << std::endl;
    }
};

namespace ir
{
class Module;
using ModulePass = Pass<Module*>;

class Function;
using FunctionPass = Pass<Function*>;

class BasicBlock;
using BasicBlockPass = Pass<BasicBlock*>;

using ModulePassManager = PassManager<Module*>;

using FunctionPassManager = PassManager<Function*>;

using BasicBlockPassManager = PassManager<BasicBlock*>;

} // namespace ir

namespace mc
{

}


} // namespace sysy


#endif