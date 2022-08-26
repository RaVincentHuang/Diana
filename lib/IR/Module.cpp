#include "sysy/IR/Module.h"

namespace sysy
{
namespace ir
{

Function* Module::getFunction(const std::string& name)
{
    if(auto iter = nameFunc.find(name); iter != nameFunc.end())
        return *iter->second;
    for(auto func : __function)
        if(func->getName() == name)
            return func;
    return nullptr;
}


} // namespace ir
    
} // namespace sysy
