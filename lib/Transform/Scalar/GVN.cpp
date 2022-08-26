#include "sysy/Transform/Scalar/GVN.h"
#include "sysy/Transform/Scalar/SimplifyInstructions.h"
#include "sysy/Transform/Utils/MemorySSA.h"
#include "sysy/Analysis/Vertify.h"
#include "sysy/Analysis/Analysis.h"
#include "sysy/Analysis/DomInfo.h"
#include "sysy/Analysis/AliasAnalysis.h"
#include "sysy/Analysis/IPAnalysis.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/IRDump.h"
#include <stack>
#include <unordered_set>
#include <unordered_map>

namespace sysy
{
namespace ir
{

LocalGVN::LocalGVN()
{
    getRelated().push_back(new IPAnalysis);
}

void LocalGVN::run(Module* module)
{
    PrintMsg("Runing GVN");
    auto pm = new FunctionPassManager;
    pm->addPass(new GVN);
    for(auto func : module->__function)
        if(!func->isDeclare())
            pm->run(func);
}

GVN::GVN() 
{
    getRelated().push_back(new MemorySSA);
    getRelated().push_back(new DomInfo);
    getFinish().push_back(new ClearMemorySSA);
    // getFinish().push_back(new FunctionVertify);
}

static bool fixed;
static std::unordered_map<Value*, Value*> ValueNumber;
static std::vector<Instruction*> InstrToRemove;

static void replace(Instruction* instr, Value* value)
{
    if(instr == value)
        return;
    fixed = false;
    // PrintMsg("replace " + instr->getSignature() + "\n\tto " + value->getSignature());
    if(auto rootInstr = dynamic_cast<Instruction*>(value))
    {
        auto rootBB = rootInstr->getBasicBlock();
        auto instrBB = instr->getBasicBlock();
        if(rootBB == instrBB && instrBB->getRank(instr) < rootBB->getRank(rootInstr))
        {
            // PrintMsg("swap " + instr->getSignature() + " to " + rootInstr->getSignature());
            auto iter = std::find(instrBB->getInstructions().begin(), 
                                    instrBB->getInstructions().end(), instr);
            auto diter = std::find(rootBB->getInstructions().begin(), 
                                    rootBB->getInstructions().end(), rootInstr);
            *iter = rootInstr;
            rootInstr->reSetBasicBlock(instrBB);
            instrBB->setRank(rootInstr, instrBB->getRank(instr));

            *diter = new DeadInst(instrBB);
            // dumpBasickBlock(instrBB);
            InstrToRemove.push_back(*diter);
            instr->replaceAllUseWith(rootInstr);
            instr->UnUseAll();
            ValueNumber.erase(instr);
            return;
        }
        else if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            ValueNumber.erase(instr);
            instr->replaceAllUseWith(value);
            InstrToRemove.push_back(instr);
            instr->UnUseAll();
            return;
        }
        else if(rootBB != instrBB && !instrBB->getDoms().count(rootBB))
        {
            if(rootBB->getDoms().count(instrBB))
            {
                // PrintMsg("swap " + instr->getSignature() + " to " + rootInstr->getSignature());
                rootInstr->getBasicBlock()->getInstructions().remove(rootInstr);
                auto iter = std::find(instrBB->getInstructions().begin(), 
                                    instrBB->getInstructions().end(), instr);
                *iter = rootInstr;
                rootInstr->reSetBasicBlock(instrBB);
                instrBB->setRank(rootInstr, instrBB->getRank(instr));

                instr->replaceAllUseWith(rootInstr);
                instr->UnUseAll();
                ValueNumber.erase(instr);
                return;
            }
            auto lca = lcaCompute(rootBB, instrBB);
            // PrintMsg("/Insert/ the " + rootInstr->getSignature());
            rootBB->getInstructions().remove(rootInstr);
            lca->insertBeforeTerminal(rootInstr);
            // dumpBasickBlock(rootInstr->getBasicBlock());
            InstrToRemove.push_back(rootInstr);
        }
    }
    // PrintMsg("replace " + instr->getSignature() + " to " + value->getSignature());
    ValueNumber.erase(instr);
    instr->replaceAllUseWith(value);
    InstrToRemove.push_back(instr);
    instr->UnUseAll();
}

void GVN::doPerInstr(Instruction* instr)
{
    auto val = simplifyInstruction(instr);
    if(auto simpleInstr = dynamic_cast<Instruction*>(val); !simpleInstr)
    {
        replace(instr, val);
        return;
    }
    else
    {
        if(auto store = dynamic_cast<StoreInst*>(instr))
            if(!store->getValue()->getType()->isPointerTy())
            {
                ValueNumber[store] = store;
                return;
            }
        else if(auto load = dynamic_cast<LoadInst*>(instr))
        {
            auto ptr = load->getPtr();
            auto array = getOriginArray(ptr);
            bool isConst = false;
            if(auto gep = dynamic_cast<GEPInst*>(ptr))
                if(auto gv = dynamic_cast<GlobalValue*>(array); gv && gv->isConstant())
                {
                    isConst = true;
                    if(gv->getInitial()->isZero())
                    {
                        if(load->getType()->isIntegerTy())
                        {
                            auto zero = ConstantInt::getZero(Type::getInt32Ty());
                            replace(instr, zero);
                            return;
                        }
                        else
                        {
                            auto zero = ConstantFP::getZero();
                            replace(instr, zero);
                            return;
                        }
                    }
                    std::vector<int> indexs;
                    for(auto index : gep->getIndex())
                    {
                        if(auto c = dynamic_cast<ConstantInt*>(index))
                            indexs.push_back(c->getValue());
                        else
                        {
                            isConst = false;
                            break;
                        }
                    }
                    if(isConst)
                    {
                        auto initial = gv->getInitial();
                        for(auto iter = indexs.begin() + 1; iter != indexs.end(); iter++)
                        {
                            if(auto arrc = dynamic_cast<ConstantArray*>(initial))
                                initial = arrc->getValue(*iter);
                            else if(auto fc = dynamic_cast<ConstantFP*>(initial))
                            {
                                if(load->getType()->isIntegerTy())
                                {
                                    auto c = ConstantInt::get(Type::getInt32Ty(), fc->getValue());
                                    replace(instr, c);
                                    return;
                                }
                                else
                                {
                                    replace(instr, fc);
                                    return;
                                }
                            }
                            else if(auto ic = dynamic_cast<ConstantInt*>(initial))
                            {
                                if(load->getType()->isFloatTy())
                                {
                                    auto c = ConstantFP::get(ic->getValue());
                                    replace(instr, c);
                                    return;
                                }
                                else
                                {
                                    replace(instr, ic);
                                    return;
                                }
                            }
                        }
                    }

                }
            auto number = checkoutTable(simpleInstr);
            if(instr != number)
            {
                replace(instr, number);
                return;
            }
            return;
        }
        else if(auto phi = dynamic_cast<PhiInst*>(instr))
        {
            bool same = true;
            auto first = checkoutTable(phi->getPredecessors(0).first);
            for(auto [value, bb] : phi->getPredecessors())
                if(checkoutTable(value) != first)
                {
                    same = false;
                    break;
                }
            if(same)
            {
                replace(instr, first);
                return;
            }
            return;
        }
        bool useByBr = false;
        for(auto use : instr->getUses())
        {
            auto user = use->getUser();
            if(auto br = dynamic_cast<BrInst*>(instr))
            {
                useByBr = true;
                break;
            }
        }
        if(useByBr)
            return;
        
        auto number = checkoutTable(simpleInstr);
        if(instr != number)
        {
            replace(instr, number);
            return;
        }
    }
}

Value* GVN::checkoutTable(Value* val)
{
    if(auto number = ValueNumber.find(val); number != ValueNumber.end())
        return number->second;
    if(auto cVal = dynamic_cast<ConstantInt*>(val))
        for(auto [key, number] : ValueNumber)
            if(auto cKey = dynamic_cast<ConstantInt*>(key); cKey && cKey->getValue() == cVal->getValue())
                return number;
    else if(auto cVal = dynamic_cast<ConstantFP*>(val))
        for(auto [key, number] : ValueNumber)
            if(auto cKey = dynamic_cast<ConstantFP*>(key); cKey && cKey->getValue() == cVal->getValue())
                return number;

    // We must store the new number.
    if(auto instr = dynamic_cast<Instruction*>(val))
        if(auto number = getValueNumber(instr))
        {
            ValueNumber[val] = number;
            return number;
        }
    ValueNumber[val] = val;
    return val;
}

Value* GVN::getValueNumber(Instruction* instr)
{
    if(auto binary = dynamic_cast<BinaryInst*>(instr))
        return getValueNumber(binary);
    else if(auto unary = dynamic_cast<UnaryInst*>(instr))
        return getValueNumber(unary);
    else if(auto phi = dynamic_cast<PhiInst*>(instr))
        return getValueNumber(phi);
    else if(auto gep = dynamic_cast<GEPInst*>(instr))
        return getValueNumber(gep);
    else if(auto call = dynamic_cast<CallInst*>(instr))
        return getValueNumber(call);
    else if(auto load = dynamic_cast<LoadInst*>(instr))
        return getValueNumber(load);
    else
        return nullptr;
}

Value* GVN::getValueNumber(BinaryInst* binary)
{
    auto lhs = checkoutTable(binary->getLhs());
    auto rhs = checkoutTable(binary->getRhs());
    for(auto [key, value] : ValueNumber)
    {
        if(auto binKey = dynamic_cast<BinaryInst*>(key))
        {
            auto keyLhs = checkoutTable(binKey->getLhs());
            auto keyRhs = checkoutTable(binKey->getRhs());
            if(binary->getKind() == binKey->getKind() && ((lhs == keyLhs && rhs == keyRhs) 
                || (binary->isCommutative() && lhs == keyRhs && rhs == keyLhs)))
                return value;
            else if(Instruction::isReverse(binary, binKey) && keyLhs == rhs && keyRhs == lhs)
                return value;
        }
    }
    return binary;
}

Value* GVN::getValueNumber(UnaryInst* unary)
{
    auto val = checkoutTable(unary->getValue());
    for(auto [key, value] : ValueNumber)
    {
        if(auto unaryKey = dynamic_cast<UnaryInst*>(key))
        {
            auto keyVal = checkoutTable(unaryKey->getValue());
            if(unary->getKind() == unaryKey->getKind() && val == keyVal)
                return value;
        }
    }
    return unary;
}

Value* GVN::getValueNumber(PhiInst* phi)
{
    for(auto [key, value] : ValueNumber)
    {
        if(auto keyPhi = dynamic_cast<PhiInst*>(key); keyPhi && keyPhi->getBasicBlock() == phi->getBasicBlock())
        {
            bool same = phi->size() == keyPhi->size();
            if(same)
                for(size_t i = 0; i < phi->size(); i++)
                {
                    // TODO use the checkLabel to the incoming values
                    auto income1 = phi->getPredecessors(i).first;
                    auto income2 = keyPhi->getPredecessors(i).first;
                    if(income1 != income2)
                    {
                        same = false;
                        break;
                    }
                }
            if(same)
            {
                // PrintMsg(phi->getSignature());
                // PrintMsg(keyPhi->getSignature());
                return value;
            }
        }
    }
    return phi;
}

Value* GVN::getValueNumber(GEPInst* gep)
{
    for(auto [key, value] : ValueNumber)
    {
        if(auto keyGep = dynamic_cast<GEPInst*>(key))
        {
            bool same = (gep->getPtr() == keyGep->getPtr()) && (gep->size() == keyGep->size());
            if(same)
                for(size_t i = 0; i < gep->size(); i++)
                {
                    // TODO use the checkLabel to the incoming values
                    auto income1 = checkoutTable(gep->getIndex(i));
                    auto income2 = checkoutTable(keyGep->getIndex(i));
                    if(income1 != income2)
                    {
                        same = false;
                        break;
                    }
                }
            if(same)
                return value;
        }
    }
    return gep;
}

Value* GVN::getValueNumber(CallInst* call)
{
    // if(!call->isPureCall())
    //     return call;
    
    // for(auto [key, value] : ValueNumber)
    // {
    //     if(auto keyCall = dynamic_cast<CallInst*>(key); keyCall
    //         && keyCall->getCallee() == call->getCallee())
    //     {
    //         for(auto i = 0; i < call->size(); i++)
    //             if(checkoutTable(keyCall->getArgs(i)) != checkoutTable(call->getArgs(i)))
    //                 return call;
    //         return value;
    //     }
    // }
    return call;
}

Value* GVN::getValueNumber(LoadInst* load)
{
    auto ptr = checkoutTable(load->getPtr());
    auto store = load->getStore();
    for(auto [key, value] : ValueNumber)
    {
        if(auto loadKey = dynamic_cast<LoadInst*>(key))
        {
            auto keyPtr = checkoutTable(loadKey->getPtr());
            auto keyStore = loadKey->getStore();
            if(ptr == keyPtr && store == keyStore)
                return value;
        }
        else if(auto storeKey = dynamic_cast<StoreInst*>(key))
        {
            auto keyPtr = checkoutTable(storeKey->getPtr());
            if(ptr == keyPtr && store == storeKey)
                return storeKey->getValue();
        }
    }
    return load;
}

void GVN::run(Function* func)
{
    PrintMsg("GVN in " + func->getName());
    // dumpFunction(func);
    std::vector<BasicBlock*> RPOWorkList;
    std::unordered_set<BasicBlock*> vis;
    BasicBlock::BasicBlockDfsPost(func->getEntryBlock(), [&](BasicBlock* bb) -> bool {
        if(vis.count(bb))
            return true;
        vis.insert(bb);
        return false;
    }, [&](BasicBlock* bb) -> void {
        RPOWorkList.push_back(bb);
    });


    ValueNumber.clear();
    do {
        fixed = true;
        for(auto bb = RPOWorkList.rbegin(); bb != RPOWorkList.rend(); bb++)
        {
            // PrintMsg("do in bb " + (*bb)->getName());
            // dumpBasickBlock(*bb);
            InstrToRemove.clear();
            for(auto instr : (*bb)->getInstructions())
                doPerInstr(instr);
            for(auto& instr : InstrToRemove)
               (*bb)->getInstructions().remove(instr);
            InstrToRemove.clear();
            // dumpBasickBlock(*bb);
        }
    } while (!fixed);
    // dumpFunction(func);
}

} // namespace ir
    
} // namespace sysy