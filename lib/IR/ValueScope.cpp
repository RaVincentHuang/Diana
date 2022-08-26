#include "sysy/IR/ValueScope.h"
#include "sysy/Support/debug.h"

namespace sysy
{


namespace ir
{
ValueScope* ValueScope::getFather() 
{
    return father;
}

Value* ValueScope::getValue(std::string varName) 
{
    auto iter = namedValues.find(varName);
    if(iter != namedValues.end())
        return iter->second;
    else if(father)
        return father->getValue(varName);
    ErrorMsg(varName + ": Variable unalloca");
}

void ValueScope::insertAlloca(std::string name, Value* alloca) 
{
    if(namedValues.count(name))
        ErrorMsg(name + ": alloca has been allocated");
    namedValues[name] = alloca;
}

ValueScope* ValueScope::createChild() 
{
    ValueScope* child = new ValueScope(this);
    return child;
}

ValueScope* ValueScope::exitBlock(ValueScope* ptr) 
{
    auto father = ptr->father;
    delete ptr;
    return father;
}

} // namespace ir


} // namespace sysy
