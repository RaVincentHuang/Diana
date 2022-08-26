#include "sysy/Transform/IPO/IPDCE.h"
#include "sysy/Transform/Scalar/DCE.h"
#include "sysy/Analysis/IPAnalysis.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/Instructions.h"
namespace sysy
{
namespace ir
{

IPDCE::IPDCE()
{
    getRelated().push_back(new IPAnalysis);
}

void IPDCE::eraseAllUnuseReturn(Module* mod)
{
    auto dce = new DCE;

    bool fixed;
    do
    {
        fixed = true;
        for(auto func : mod->__function)
        {
            if(func->isDeclare() || func->getReturnType()->isVoidTy() || func->getName() == "main")
                continue;
            bool retUse = false;
            for(auto call : func->getCallSite())
            {
                if(!call->getUses().empty())
                    retUse = true;
            }

            if(!retUse)
            {
                bool changedFuction = false;
                for(auto bb : func->getBasicBlockList())
                    for(auto instr : bb->getInstructions())
                        if(auto ret = dynamic_cast<RetInst*>(instr))
                        {
                            if(auto retVal = dynamic_cast<Instruction*>(ret->getRet()))
                            {
                                ret->UnUseAll();
                                if(func->getReturnType()->isFloatTy())
                                    ret->addOperand(ConstantFP::getZero());
                                else
                                    ret->addOperand(ConstantInt::get(32, 0));
                                changedFuction = true;
                            }
                        }
                if(changedFuction)
                {
                    dce->run(func);
                    fixed = false;
                }
            }
        }
    } while(!fixed);
}

void IPDCE::run(Module* module)
{
    PrintMsg("Run IPDCE");
    eraseAllUnuseReturn(module);
}

} // namespace ir
    
} // namespace sysy
