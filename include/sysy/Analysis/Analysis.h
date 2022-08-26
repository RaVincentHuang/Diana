#ifndef SYSY_ANALYSIS_ANALYSIS_H
#define SYSY_ANALYSIS_ANALYSIS_H
#include "sysy/IR/BasicBlock.h"
namespace sysy
{
namespace ir
{

BasicBlock* lcaCompute(BasicBlock* bb1, BasicBlock* bb2);

} // namespace ir
    
} // namespace sysy


#endif