#include "sysy/Target/Reg.h"
#include <map>
#include <string>

namespace sysy 
{
namespace mc 
{


std::map<Reg::Type, std::string> Reg::type_map = {
    {Reg::PHY, "physical"}, {Reg::VIRTUAL, "virtual"}
};

std::map<Reg::Kind, std::string> Reg::kind_map = {
    {Reg::SREG, "sreg"}, {Reg::VREG, "vreg"}
};

std::string Reg::getSignature() {
    std::string signature = "";
    if (kind == Reg::SREG) {
        if (id == 13) signature = "sp";
        else if (id == 14) signature = "lr";
        else if (id == 15) signature = "pc";
        else signature = "r" + std::to_string(id);
    }
    else 
        signature = "s" + std::to_string(id);

    return signature;
}

int REGISTER_USAGE[16] = {
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 3
};

bool allocable(int reg_id) {
  return (REGISTER_USAGE[reg_id] == 1) ||
         (REGISTER_USAGE[reg_id] == 2);
}

}
}