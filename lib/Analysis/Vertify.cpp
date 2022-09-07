#include "sysy/Analysis/Vertify.h"
#include "sysy/Support/Debug.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Value.h"
#include "sysy/IR/IRDump.h"
#include <unordered_set>

namespace sysy
{
namespace ir
{

void ModuleVertify::run(Module* module)
{

}

void FunctionVertify::run(Function* func)
{
    std::unordered_set<Value*> def;
    std::unordered_set<BasicBlock*> vis;

    // PrintMsg("Vertify");

    for(auto& arg : func->getArgs())
        def.insert(arg.get());

    BasicBlock::BasicBlockDfs(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        // PrintMsg("In bb " + bb->getName());
        int cnt = 1;
        for(auto instr : bb->getInstructions())
        {
            def.insert(instr);
            // PrintMsg("\tAt instr " + instr->getSignature());
            if(auto binary = dynamic_cast<BinaryInst*>(instr))
            {
                if(!def.count(binary->getLhs()) && !dynamic_cast<GlobalValue*>(binary->getLhs()) 
                    && !dynamic_cast<Constant*>(binary->getLhs()))
                {
                    PrintMsg("Dead block is -->");
                    dumpBasickBlock(bb);
                    ErrorMsg("Undefineud Variable: " + binary->getLhs()->getName() 
                        + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                }
                else if(!def.count(binary->getRhs()) && !dynamic_cast<GlobalValue*>(binary->getRhs())
                        && !dynamic_cast<Constant*>(binary->getRhs()))
                {
                    PrintMsg("Dead block is -->");
                    dumpBasickBlock(bb);
                    ErrorMsg("Undefineud Variable: " + binary->getRhs()->getName() 
                        + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                }
            }
            else if(auto unary = dynamic_cast<UnaryInst*>(instr))
            {
                if(!def.count(unary->getValue()) && !dynamic_cast<GlobalValue*>(unary->getValue())
                    && !dynamic_cast<Constant*>(unary->getValue()))
                {
                    PrintMsg("Dead block is -->");
                    dumpBasickBlock(bb);
                    ErrorMsg("Undefineud Variable: " + unary->getValue()->getName() 
                        + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                }
            }
            else if(auto load = dynamic_cast<LoadInst*>(instr))
            {
                if(!def.count(load->getPtr()) && !dynamic_cast<GlobalValue*>(load->getPtr())
                    && !dynamic_cast<Constant*>(load->getPtr()))
                {
                    PrintMsg("Dead block is -->");
                    dumpBasickBlock(bb);
                    ErrorMsg("Undefineud Variable: " + load->getPtr()->getName() 
                        + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                }
            }
            else if(auto store = dynamic_cast<StoreInst*>(instr))
            {
                if(!def.count(store->getPtr()) && !dynamic_cast<GlobalValue*>(store->getPtr())
                    && !dynamic_cast<Constant*>(store->getPtr()))
                {
                    PrintMsg("Dead block is -->");
                    dumpBasickBlock(bb);
                    ErrorMsg("Undefineud Variable: " + store->getPtr()->getName() 
                        + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                }
                else if(!def.count(store->getValue()) && !dynamic_cast<GlobalValue*>(store->getValue())
                            && !dynamic_cast<Constant*>(store->getValue()))
                {
                    PrintMsg("Dead block is -->");
                    dumpBasickBlock(bb);
                    ErrorMsg("Undefineud Variable: " + store->getValue()->getName() 
                        + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                }
            }
            else if(auto gep = dynamic_cast<GEPInst*>(instr))
            {
                for(auto idx : gep->getIndex())
                    if(!def.count(idx) && !dynamic_cast<GlobalValue*>(idx) && !dynamic_cast<Constant*>(idx))
                    {
                        PrintMsg("Dead block is -->");
                        dumpBasickBlock(bb);
                        ErrorMsg("Undefineud Variable: " + idx->getName()
                            + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                    }
            }
            else if(auto call = dynamic_cast<CallInst*>(instr))
            {
                for(auto arg : call->getArgs())
                    if(!def.count(arg) && !dynamic_cast<GlobalValue*>(arg) && !dynamic_cast<Constant*>(arg))
                    {
                        PrintMsg("Dead block is -->");
                        dumpBasickBlock(bb);
                        ErrorMsg("Undefineud Variable: " + arg->getName() 
                            + " at BasickBlock " + bb->getName() + ", at line " + std::to_string(cnt));
                    }
            }
            cnt++;
        }
        return false;
    });
};

} // namespace ir
    
} // namespace sysy
