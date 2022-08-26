#ifndef SYSY_FRONTENDMAIN_H
#define SYSY_FRONTENDMAIN_H
#include "sysy/IR/Module.h"

#include <string>
namespace sysy
{
ir::Module* FrontendMain(const std::string& filename, const std::string& Gen);
}
#endif