#include "sysy/Transform/Transform.h"
#include "sysy/Transform/Utils/Memory2Reg.h"
#include "sysy/Transform/Utils/EliminatePhi.h"
#include "sysy/Transform/Utils/SimplifyCFG.h"
#include "sysy/Transform/Utils/LCSSA.h"
#include "sysy/Transform/Scalar/GVN.h"
#include "sysy/Transform/Scalar/DCE.h"
#include "sysy/Transform/Scalar/ADCE.h"
#include "sysy/Transform/Scalar/GCM.h"
#include "sysy/Transform/Scalar/InstructionCombine.h"
#include "sysy/Transform/Loop/LSR.h"
#include "sysy/Transform/Loop/LoopUnroll.h"
#include "sysy/Transform/Loop/LoopParallel.h"
#include "sysy/Transform/IPO/FunctionInline.h"
#include "sysy/Transform/IPO/IPDCE.h"
#include "sysy/Analysis/IPAnalysis.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/Function.h"

namespace sysy
{
namespace ir
{

void initialIRTransform(std::string opt, ModulePassManager* pm)
{
    if(opt == "O2")
    {
        pm->addPass(new SSASet);
        pm->addPass(new ScalarPass(false));
        pm->addPass(new LoopEarlyPass);
        pm->addPass(new IPDCE);
        pm->addPass(new ScalarPass(false));
        pm->addPass(new FunctionInline);
        pm->addPass(new ScalarPass(false));
        pm->addPass(new LoopVariablePass);
        pm->addPass(new ScalarPass(true));
        pm->addPass(new UnsetSSA);
    }
    else
    {
        pm->addPass(new SSASet);
        pm->addPass(new LinearScalarPass);
        pm->addPass(new FunctionInline);
        pm->addPass(new LinearScalarPass);
        pm->addPass(new UnsetSSA);
    }
}

ScalarPass::ScalarPass(bool after) : isAfterLoopOpt(after)
{
    getRelated().push_back(new IPAnalysis);
}

void ScalarPass::run(Module* module)
{
    auto pm = new FunctionPassManager;
    pm->addPass(new InstructionCombine);
    pm->addPass(new DCE);
    pm->addPass(new GVN);
    pm->addPass(new DCE);
    pm->addPass(new GCM);
    // pm->addPass(new ADCE);
    auto cfg = new SimplifyCFG(isAfterLoopOpt);
    pm->addPass(cfg);

    for(auto func : module->__function)
        if(!func->isDeclare())
        {
            do
            {
                PrintMsg("Do the Scalar Passes at Function " + func->getName());
                pm->run(func);
            }while(cfg->changed());
        }
}

void LinearScalarPass::run(Module* module)
{
    auto pm = new FunctionPassManager;
    pm->addPass(new InstructionCombine);
    pm->addPass(new DCE);
    auto cfg = new SimplifyCFG(true);
    pm->addPass(cfg);

    for(auto func : module->__function)
        if(!func->isDeclare())
        {
            do
            {
                PrintMsg("Do the Scalar Passes at Function " + func->getName());
                pm->run(func);
            }while(cfg->changed());
        }
}

LoopVariablePass::LoopVariablePass()
{
    getFinish().push_back(new ScalarPass(false));
}



void LoopVariablePass::run(Module* module)
{
    auto pm = new FunctionPassManager;
    pm->addPass(new LSR);
    for(auto func : module->__function)
        if(!func->isDeclare())
            pm->run(func);
}


LoopEarlyPass::LoopEarlyPass()
{
    getFinish().push_back(new ScalarPass(false));
}

void LoopEarlyPass::run(Module* module)
{
    auto pm = new FunctionPassManager;
    pm->addPass(new LoopUnrollAll);
    pm->addPass(new LoopParallel);
    for(auto func : module->__function)
        if(!func->isDeclare())
            pm->run(func);
}

} // namespace ir
    
} // namespace sysy
