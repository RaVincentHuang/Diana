#include "sysy/Analysis/AliasAnalysis.h"
#include "sysy/IR/GlobalValue.h"
#include "sysy/IR/Function.h"

namespace sysy
{
namespace ir
{

Value* getOriginArray(Value* ptr)
{
    do
    {
        if(auto gep = dynamic_cast<GEPInst*>(ptr))
            ptr = gep->getPtr();
        else if(auto load = dynamic_cast<LoadInst*>(ptr))
            ptr = load->getPtr();
        else
            break;
    } while(true);

    if(auto alloca = dynamic_cast<AllocaInst*>(ptr))
    {
        // It is must be args, we get the arg
        if(alloca->getType()->isPointerTy())
            for(auto use : alloca->getUses())
                if(auto store = dynamic_cast<StoreInst*>(use->getUser()))
                    ptr = store->getPtr();
        return ptr;
    }
    else if(auto gv = dynamic_cast<GlobalValue*>(ptr))
        return gv;
    else
        return nullptr;

}

bool isGlobal(Value* val) { return dynamic_cast<GlobalValue*>(val); }

bool isArgArray(Value* val)
{
    if(auto alloca = dynamic_cast<AllocaInst*>(val); 
        alloca && alloca->getAllocatedType()->isPointerTy())
        return true;
    return false;
}

bool isLocal(Value* val) { return !isGlobal(val) && !isArgArray(val); }

bool isAliasByGVToArg(GlobalValue* gv, AllocaInst* arg)
{  
    std::vector<int> gvDim;
    std::vector<int> argDim;
    auto type = gv->getValueType();
    while(auto arrType = dynamic_cast<ArrayType*>(type))
    {
        gvDim.push_back(arrType->getSize());
        type = arrType->getInnerType();
    }

    type = arg->getAllocatedType()->getInnerType();
    while(auto arrType = dynamic_cast<ArrayType*>(type))
    {
        argDim.push_back(arrType->getSize());
        type = arrType->getInnerType();
    }

    for(auto gvIter = gvDim.rbegin(), argIter = argDim.rbegin(); 
        gvIter != gvDim.rend() && argIter != argDim.rend(); gvIter++, argIter++)
        if(*gvIter != *argIter)
            return false;
    return true;
}

bool alias(Value* array1, Value* array2)
{
    // FIXME f(a, a) will faild
    if ((isGlobal(array1) && isGlobal(array2)) || (isArgArray(array1) && isArgArray(array2)) || (isLocal(array1) && isLocal(array2)))
        return array1 == array2;
    
    if(auto gv1 = dynamic_cast<GlobalValue*>(array1))
        if(auto arg2 = dynamic_cast<AllocaInst*>(array2))
            return isAliasByGVToArg(gv1, arg2);
    
    if(auto gv2 = dynamic_cast<GlobalValue*>(array2))
        if(auto arg1 = dynamic_cast<AllocaInst*>(array1))
            return isAliasByGVToArg(gv2, arg1);

    return false;
}

bool aliasAtCall(Value* array, CallInst* call)
{
    if(auto gv = dynamic_cast<GlobalValue*>(array); gv 
        && call->getCallee()->getStoreGV().count(gv))
        return true;
    for(auto arg : call->getArgs())
        if(auto gep = dynamic_cast<GEPInst*>(arg); gep && alias(array, getOriginArray(gep)))
            return true;
    return false;
}

} // namespace ir
    
} // namespace sysy
