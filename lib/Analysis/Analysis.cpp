#include "sysy/Analysis/Analysis.h"
#include "sysy/IR/IRDump.h"

namespace sysy
{
namespace ir
{

BasicBlock* lcaCompute(BasicBlock* bb1, BasicBlock* bb2)
{
    while(bb1->getDomLevel() > bb2->getDomLevel())
        bb1 = bb1->getIdom();
    while(bb2->getDomLevel() > bb1->getDomLevel())
        bb2 = bb2->getIdom();
    
    while (bb1 != bb2)
    {
        bb1 = bb1->getIdom();
        bb2 = bb2->getIdom();
    }
    return bb1;
}

} // namespace ir
    
} // namespace sysy
