#ifndef SYSY_TARGET_MODULE_H
#define SYSY_TARGET_MODULE_H

#include "sysy/IR/Module.h"
#include "sysy/Target/GlobalObject.h"
#include <list>
#include <vector>
#include "sysy/Target/Regallocator.h"
namespace sysy 
{
namespace mc 
{

class Function;
class GlobalObject;

class Module {
public:
    std::list<Function*> functions;
    std::vector<GlobalObject*> global_objects;
    std::vector<int> reg_allocate(RegAllocStat *stat);
    
    bool data_flag = false;
    bool bss_flag = false;
    int argc;

    bool create_threads = false;

    std::map<int, GlobalObject*> float_map;
    std::map<ir::Function*, Function*> func_map;
    std::map<ir::GlobalValue*, GlobalObject*> global_map;
    std::map<GlobalObject*, ir::GlobalValue*> rev_global_map;

    Module(ir::Module *ir, int _argc);
    // void analyseCond();
    void run(std::string file_name);
    std::string gen_global();
    void gen_asm(std::string file_name);
};

} // namespace mc 
} // namespace sysy


#endif