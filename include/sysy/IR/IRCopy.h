#ifndef SYSY_IR_IRCOPY_H
#define SYSY_IR_IRCOPY_H
#include "sysy/IR/Instructions.h"
#include "sysy/IR/Loop.h"
#include <functional>
#include <unordered_map>
namespace sysy
{

namespace ir
{

class IRCopier
{
private:
    std::unordered_map<Value*, Value*> ValueCopy;
public:
    Function* getFunctionCopy(Function* func);
    Loop* getLoopCopy(Loop* loop, std::unordered_map<Value*, Value*>& LoopValueCpy, 
                        std::unordered_map<PhiInst*, Value*>& CurrentIncomingValue);
    static Instruction* getCopyOfInstruction(Instruction* instr, std::function<Value*(Value*)> findValue);
};
    
} // namespace ir


} // namespace sysy



#endif