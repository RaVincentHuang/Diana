#ifndef SYSY_TARGET_GLOBALOBJECT_H
#define SYSY_TARGET_GLOBALOBJECT_H

#include "sysy/IR/Module.h"
#include <string>
#include <vector>

namespace sysy 
{
namespace mc 
{

class GlobalObject {
public:
    std::string name;
    std::vector<int> values;
    std::string str_value;
    bool is_zero;
    bool is_string;
    bool is_float;
    int size = 0;

    static int id;

    GlobalObject(std::string _name) : name(_name), is_string(false), is_float(false) {}
    GlobalObject(std::string _name, bool _is_float) : name(_name), is_float(_is_float), is_string(false) {}
    void pushValue(ir::Constant* gv);
    static std::string dec2hex(int i);
    static std::string getName();
};

} // namespace mc 
} // namespace sysy

#endif