#include "sysy/IR/GlobalValue.h"
#include "sysy/IR/Module.h"

namespace sysy
{
namespace ir
{
    
GlobalValue* GlobalValue::Create(Type* _type, bool _isConst, Constant* _initial, std::string _name)
{
    auto ptr = new GlobalValue(Type::getPointerTy(_type), _isConst, _initial, _name);
    Module::getInstance().__globalValue.push_back(ptr);
    return ptr;
}
    

} // namespace ir
    
} // namespace sysy