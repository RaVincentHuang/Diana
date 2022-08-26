#ifndef SYSY_ANALYSIS_ALIAS_H
#define SYSY_ANALYSIS_ALIAS_H
#include "sysy/IR/Value.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/GlobalValue.h"
namespace sysy
{
namespace ir
{

Value* getOriginArray(Value* ptr);

bool isArgArray(Value* val);

bool alias(Value* array1, Value* array2);

bool aliasAtCall(Value* array, CallInst* call);

} // namespace ir
    
} // namespace sysy


#endif