#include "sysy/IR/Module.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Type.h"
#include <string>

namespace sysy
{
namespace ir
{

std::string Function::Arg::getSignature() { return type->getSignature() + " %" + name; }

Function::Function(FunctionType* _type, std::string _name) : Value(_type, _name), retType(_type->getRet())
{
    auto& mod = Module::getInstance();
    for(auto type : _type->getArg())
        args.push_back(std::make_unique<Arg>(type));
    mod.__function.push_back(this); 
    funcIter = mod.__function.end();
    funcIter--; // because we have inserted it at back just away.
    mod.registerFunc(name, funcIter);
}

Function::Function(FunctionType* _type) : Value(_type, ""), retType(_type->getRet())
{
    for(auto type : _type->getArg())
        args.push_back(std::make_unique<Arg>(type));
}

std::vector<Loop*>& Function::getTopLoops() { return topLoops; }

std::vector<Loop*>& Function::getAllLoops() { return allLoops; }

} // namespace ir
    
} // namespace sysy
