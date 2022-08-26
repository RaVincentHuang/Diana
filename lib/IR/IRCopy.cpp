#include "sysy/IR/IRCopy.h"
#include "sysy/IR/Value.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/IRDump.h"
#include <vector>
#include <unordered_set>

namespace sysy
{
namespace ir
{

Function* IRCopier::getFunctionCopy(Function* func)
{
    ValueCopy.clear();
    for(auto gv : Module::getInstance().__globalValue)
        ValueCopy[gv] = gv;
    auto funcCpy = Function::Create(dynamic_cast<FunctionType*>(func->getType()));

    for(int i = 0; i < funcCpy->getArgs().size(); i++)
    {
        funcCpy->getArgs().at(i)->setName("copy." + func->getArgs().at(i)->getName());
        ValueCopy[func->getArgs().at(i).get()] = funcCpy->getArgs().at(i).get();
    }

    for(auto bb : func->getBasicBlockList())
        ValueCopy[bb] = BasicBlock::Create(func->getName() + ".copy." + bb->getName(), funcCpy, bb->isPreHeader());

    for(auto bb : func->getBasicBlockList())
    {
        auto bbCpy = dynamic_cast<BasicBlock*>(ValueCopy[bb]);
        for(auto pred : bb->getPredecessor())
            bbCpy->getPredecessor().push_back(dynamic_cast<BasicBlock*>(ValueCopy[pred]));
        
        for(auto succ : bb->getSuccessor())
            bbCpy->getSuccessor().push_back(dynamic_cast<BasicBlock*>(ValueCopy[succ]));
    }

    std::vector<PhiInst*> queuePhi;
    std::unordered_set<BasicBlock*> vis;
    BasicBlock::BasicBlockDfs(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);

        auto bbCpy = dynamic_cast<BasicBlock*>(ValueCopy[bb]);

        for(auto instr : bb->getInstructions())
        {
            auto instrCpy = getCopyOfInstruction(instr, [&](Value* val) -> Value* {
                if(auto c = dynamic_cast<Constant*>(val))
                    return c;
                return ValueCopy[val];
            });
            ValueCopy[instr] = instrCpy;
            bbCpy->insertBack(instrCpy);
            instrCpy->reSetBasicBlock(bbCpy);
            if(auto phi = dynamic_cast<PhiInst*>(instr))
                queuePhi.push_back(phi);
        }

        return false;
    });

    for(auto phi : queuePhi)
    {
        auto getValue = [&](Value* val) -> Value* {
                if(auto c = dynamic_cast<Constant*>(val))
                    return c;
                else if(auto undef = dynamic_cast<UndefValue*>(val))
                    return undef;
                return ValueCopy[val];
        };
        auto phiCpy = dynamic_cast<PhiInst*>(ValueCopy[phi]);
        for(auto [val, pred] : phi->getPredecessors())
        {
            phiCpy->setInComingValue(getValue(val), dynamic_cast<BasicBlock*>(ValueCopy[pred]));
        }
    }

    return funcCpy;
}

Loop* IRCopier::getLoopCopy(Loop* loop, std::unordered_map<Value*, Value*>& LoopValueCpy, std::unordered_map<PhiInst*, Value*>& CurrentIncomingValue)
{
    auto headerCpy = BasicBlock::Create(loop->getHeader()->getName() + ".unrollcpy", loop->getParent());

    auto latchCpy = (loop->getHeader() == loop->getLatchBlock()) ? headerCpy : BasicBlock::Create(loop->getLatchBlock()->getName() + ".unrollcpy", loop->getParent());
    
    auto loopCpy = new Loop(headerCpy);

    loopCpy->setLatchBlock(latchCpy);

    auto getValue = [&](Value* val) -> Value* {
        if(auto phi = dynamic_cast<PhiInst*>(val); phi && CurrentIncomingValue.count(phi))
            return CurrentIncomingValue.at(phi);
        if(auto instr = dynamic_cast<Instruction*>(val); instr && LoopValueCpy.count(instr))
            return LoopValueCpy[instr];
        if(auto bb = dynamic_cast<BasicBlock*>(val); bb && LoopValueCpy.count(bb))
            return LoopValueCpy[bb];
        else
            return val;
    };

    std::unordered_map<BasicBlock*, BasicBlock*> loopBBMap;

    std::unordered_map<PhiInst*, PhiInst*> queuePhi;

    for(auto bb : loop->getLoopBasicBlocks())
        if(bb == loop->getHeader())
        {
            loopBBMap[bb] = headerCpy;
            LoopValueCpy[bb] = headerCpy;
        }
        else if(bb == loop->getLatchBlock())
        {
            loopBBMap[bb] = latchCpy;
            LoopValueCpy[bb] = latchCpy;
            loopCpy->getLoopBasicBlocks().push_back(latchCpy);
            loopCpy->BBSet.insert(latchCpy);
        }
        else if(bb != loop->getHeader() && bb != loop->getLatchBlock())
        {
            auto loopBodyCpy = BasicBlock::Create(bb->getName() + ".unrollcpy", loop->getParent());
            loopBBMap[bb] = loopBodyCpy;
            LoopValueCpy[bb] = loopBodyCpy;
            loopCpy->getLoopBasicBlocks().push_back(loopBodyCpy);
            loopCpy->BBSet.insert(loopBodyCpy);
        }

    for(auto bb : loop->getLoopBasicBlocks())
    {
        if(bb == loop->getHeader())
        {
            for(auto instr : bb->getInstructions())
            {
                if(auto phi = dynamic_cast<PhiInst*>(instr))
                    continue;
                auto instrCpy = getCopyOfInstruction(instr, getValue);
                LoopValueCpy[instr] = instrCpy;
                headerCpy->insertBack(instrCpy);
                instrCpy->reSetBasicBlock(headerCpy);
            }
        }
        else if(bb == loop->getLatchBlock())
        {
            for(auto instr : bb->getInstructions())
            {
                auto instrCpy = getCopyOfInstruction(instr, getValue);
                LoopValueCpy[instr] = instrCpy;
                latchCpy->insertBack(instrCpy);
                instrCpy->reSetBasicBlock(latchCpy);
                if(auto phi = dynamic_cast<PhiInst*>(instr))
                    if(auto phiCpy = dynamic_cast<PhiInst*>(instrCpy))
                        queuePhi[phi] = phiCpy;
            }
        }
        else
        {
            for(auto instr : bb->getInstructions())
            {
                auto instrCpy = getCopyOfInstruction(instr, getValue);
                LoopValueCpy[instr] = instrCpy;
                auto loopBodyCpy = loopBBMap[bb];
                loopBodyCpy->insertBack(instrCpy);
                instrCpy->reSetBasicBlock(loopBodyCpy);
                if(auto phi = dynamic_cast<PhiInst*>(instr))
                    if(auto phiCpy = dynamic_cast<PhiInst*>(instrCpy))
                        queuePhi[phi] = phiCpy;
            }
        }
    }

    for(auto [bb, bbCpy] : loopBBMap)
    {
        // dumpBasickBlock(bbCpy);
        if(bb != loop->getHeader())
            for(auto pred : bb->getPredecessor())
                bbCpy->getPredecessor().push_back(loopBBMap[pred]);
        if(bb != loop->getLatchBlock())
            for(auto succ : bb->getSuccessor())
                bbCpy->getSuccessor().push_back(loopBBMap[succ]);
        // dumpBasickBlock(bbCpy);
    }

    for(auto [phi, phiCpy] : queuePhi)
    {
        for(auto [val, pred] : phi->getPredecessors())
        {
            phiCpy->setInComingValue(getValue(val), dynamic_cast<BasicBlock*>(loopBBMap[pred]));
        }
    }

    std::unordered_map<PhiInst*, Value*> phiAssignLater;
    for(auto instr : loop->getHeader()->getInstructions())
    {
        if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            for(auto [val, pred] : phi->getPredecessors())
                if(pred == loop->getLatchBlock())
                    phiAssignLater[phi] = getValue(val);
        }
        else
            break;
    }

    for(auto [phi, val] : phiAssignLater)
        CurrentIncomingValue[phi] = val;

    return loopCpy;
}

Instruction* IRCopier::getCopyOfInstruction(Instruction* instr, std::function<Value*(Value*)> getValue)
{
    if(auto binary = dynamic_cast<BinaryInst*>(instr))
    {
        auto lhs = getValue(binary->getLhs());
        auto rhs = getValue(binary->getRhs());

        auto name = "cpy." + binary->getName();
        return new BinaryInst(binary->getType(), binary->getKind(), lhs, rhs, nullptr, name);
    }
    else if(auto load = dynamic_cast<LoadInst*>(instr))
    {
        auto ptr = getValue(load->getPtr());

        auto name = "cpy." + load->getName();
        return new LoadInst(load->getType(), ptr, nullptr, name);
    }
    else if(auto store = dynamic_cast<StoreInst*>(instr))
    {
        auto ptr = getValue(store->getPtr());
        auto value = getValue(store->getValue());
        
        return new StoreInst(value, ptr, nullptr);
    }
    else if(auto gep = dynamic_cast<GEPInst*>(instr))
    {
        auto ptr = getValue(gep->getPtr());

        std::vector<Value*> IDX;
        for(auto idx : gep->getIndex())
            IDX.push_back(getValue(idx));
        
        auto name = "cpy." + gep->getName();

        return new GEPInst(gep->getType(), ptr, IDX, nullptr, name);
    }
    else if(auto call = dynamic_cast<CallInst*>(instr))
    {
        auto callee = call->getCallee();

        std::vector<Value*> args;
        for(auto arg : call->getArgs())
            args.push_back(getValue(arg));

        auto name = "copy." + call->getName();
        return new CallInst(callee, args, nullptr, name);
    }
    else if(auto br = dynamic_cast<BrInst*>(instr))
    {
        if(br->getCond())
        {
            auto cond = getValue(br->getCond());
            auto trueBB = dynamic_cast<BasicBlock*>(getValue(br->getTrueBlock()));
            auto falseBB = dynamic_cast<BasicBlock*>(getValue(br->getFalseBlock()));
            return new BrInst(cond, trueBB, falseBB, nullptr);
        }
        else
        {
            auto trueBB = dynamic_cast<BasicBlock*>(getValue(br->getTrueBlock()));

            return new BrInst(trueBB, nullptr);
        }
    }
    else if(auto phi = dynamic_cast<PhiInst*>(instr))
    {
        auto name = "copy." + phi->getName();
        auto ptr = new PhiInst(phi->getType(), nullptr, name);
        return ptr;
    }
    else if(auto alloca = dynamic_cast<AllocaInst*>(instr))
    {
        auto name = "copy." + alloca->getName();

        return new AllocaInst(alloca->getAllocatedType(), nullptr, name);
    }
    else if(auto unary = dynamic_cast<UnaryInst*>(instr))
    {
        auto value = getValue(unary->getValue());

        auto name = "copy." + unary->getName();

        return new UnaryInst(unary->getType(), unary->getKind(), value, nullptr, name);
    }
    else if(auto ret = dynamic_cast<RetInst*>(instr))
    {
        if(ret->isVoidRet())
            return new RetInst(nullptr);
        auto value = getValue(ret->getRet());
        return new RetInst(value, nullptr);
    }
}

} // namespace ir
    
} // namespace sysy
