#ifndef SYSY_IR_IRDUMP_H
#define SYSY_IR_IRDUMP_H

namespace sysy
{
namespace ir
{
class Module;

std::string dumpIR(Module* mod);
void dumpFunction(Function* func);
void dumpBasickBlock(BasicBlock* bb);
void llFileGen(Module* mod);

} // namespace ir
    
} // namespace sysy


#endif