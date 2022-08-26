#include "sysy/Transform/Loop/LSR.h"
#include "sysy/Transform/Utils/LCSSA.h"
#include "sysy/Analysis/SCEV.h"
#include "sysy/Analysis/LoopInfo.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/IR/IRDump.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
namespace sysy
{
namespace ir
{

LSR::LSR()
{
    getRelated().push_back(new LoopInfo);
    getRelated().push_back(new LCSSA);
    getRelated().push_back(new ScalarEvolution);
    getFinish().push_back(new ScalarEvolutionClean);
    // getFinish().push_back(new FunctionVertify);
}

void LSR::identifyAllInductiveMultiplication(Loop* loop)
{
    Multiplies.clear();
    for(auto bb : loop->getLoopBasicBlocks())
        for(auto instr : bb->getInstructions())
            if(auto multi = dynamic_cast<BinaryInst*>(instr); 
                multi && multi->getKind() == Instruction::Mul)
                if(loop->hasSCEV(multi))
                    Multiplies.push_back(multi);
}

void LSR::eraseSingleMul(Loop* loop, BinaryInst* mul)
{
    // PrintMsg(mul->getSignature());

    auto& scev = loop->getSCEV(mul);

    // step1. initial val
    auto initial = scev.at(0);
    std::vector<BinaryInst*> initialVal;
    scev.getItemChains(0, initialVal);
    for(auto init : initialVal)
    {
        // PrintMsg(init->getSignature());
        loop->getPreHeader()->insertBeforeTerminal(init);
    }
        
    // step2. step val
    auto step = scev.at(1);
    std::vector<BinaryInst*> stepVal;
    scev.getItemChains(1, stepVal);
    for(auto step : stepVal)
    {
        // PrintMsg(step->getSignature());
        loop->getPreHeader()->insertBeforeTerminal(step);
    }

    // dumpBasickBlock(loop->getPreHeader());

    // step3. queue the phi instr
    auto phi = new PhiInst(initial->getType(), loop->getHeader(), mul->getName() + ".lsrphi");
    loop->getHeader()->insertFront(phi);

    // step4. next val
    auto next = new BinaryInst(step->getType(), Instruction::Add, phi, step, loop->getLatchBlock(), mul->getName() + ".lsrnext");
    loop->getLatchBlock()->insertBeforeTerminal(next);
    
    // step5. set the incoming val of phi
    phi->setInComingValue(initial, loop->getPreHeader());
    phi->setInComingValue(next, loop->getLatchBlock());

    // step6. replace the mul
    mul->replaceAllUseWith(phi);
}

void LSR::runOnLoop(Loop* loop)
{
    if(!loop)
        return;

    for(auto subLoop : loop->getSubLoops())
        runOnLoop(subLoop);
    
    identifyAllInductiveMultiplication(loop);

    for(auto mul : Multiplies)
        eraseSingleMul(loop, mul);
    
}

void LSR::run(Function* func)
{
    PrintMsg("LSR at function " + func->getName());
    for(auto loop : func->getTopLoops())
        runOnLoop(loop);
    
    // dumpFunction(func);
}

} // namespace ir
    
} // namespace sysy
