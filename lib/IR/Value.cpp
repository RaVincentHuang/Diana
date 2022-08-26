#include "sysy/IR/Value.h"
#include "sysy/IR/User.h"
#include "sysy/IR/Context.h"

namespace sysy
{
namespace ir
{

void Value::replaceAllUseWith(Value* value) 
{
    for(auto use : getUses())
        use->getUser()->replaceOperand(use->getPos(), value);
    this->getUses().clear();
}

void Value::removeAllUseWith() 
{
    replaceAllUseWith(nullptr);
}

UndefValue* UndefValue::Create()
{
    return new UndefValue;
}

    
} // namespace ir
    
} // namespace sysy
