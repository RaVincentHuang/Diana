#ifndef SYSY_TRANSFORM_IPDCE_H
#define SYSY_TRANSFORM_IPDCE_H
#include "sysy/IR/PassManager.h"
namespace sysy
{
namespace ir
{
class IPDCE : public ModulePass
{
private:
    void eraseAllUnuseReturn(Module* mod);
public:
    IPDCE();
    void run(Module* mod) override;
    std::string getName() override { return "IPDCE"; }
};
} // namespace ir
    
} // namespace sysy


#endif
