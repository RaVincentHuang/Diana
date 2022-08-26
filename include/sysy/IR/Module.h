#ifndef SYSY_IR_MODULE_H
#define SYSY_IR_MODULE_H

#include "sysy/IR/GlobalValue.h"
#include "sysy/IR/Instruction.h"
#include "sysy/IR/Function.h"
#include <vector>
#include <list>
#include <map>

namespace sysy
{
namespace ir
{

class Module
{
private:
    std::unordered_map<std::string, std::list<Function*>::iterator> nameFunc;
public:
    std::vector<GlobalValue*> __globalValue;
    std::list<Function*> __function;

    static Module& getInstance()
    {
        static Module mood;
        return mood;
    }

    void registerFunc(const std::string& name, std::list<Function*>::iterator iter)
        { nameFunc[name] = iter; }

    Function* getFunction(const std::string& name);
};

} // namespace ir
    
} // namespace sysy


#endif