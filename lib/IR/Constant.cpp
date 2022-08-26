#include "sysy/IR/Constant.h"

namespace sysy
{
namespace ir
{

Constant* Constant::getZero(Type* _type)
{
    if(_type->isIntegerTy())
        return ConstantInt::getZero(_type);
    else if(_type->isFloatTy())
        return ConstantFP::getZero();
    else if(_type->isArrayTy())
        return ConstantArray::getZero(dynamic_cast<ArrayType*>(_type));

}

Constant* Constant::getString(const std::string& str)
{
    return ConstantString::get(str);
}

ConstantInt* ConstantInt::get(Type* _type, int _value) 
{
    return new ConstantInt(_type, _value);
}

ConstantInt* ConstantInt::get(unsigned width, int _value)
{
    if(width == 1)
        return get(Type::getInt1Ty(), (bool) _value);
    else if(width == 8)
        return get(Type::getInt8Ty(), _value);
    else if(width == 32)
        return get(Type::getInt32Ty(), _value);
    else
        ErrorMsg("Bad type in ConstantInt");
}

ConstantInt* ConstantInt::getZero(Type* _type) { return ConstantInt::get(_type, 0); }

ConstantFP* ConstantFP::get(float _value)
{
    return new ConstantFP(_value);
}

ConstantFP* ConstantFP::getZero() { return ConstantFP::get(0); }

ConstantArray* ConstantArray::get(ArrayType* _type, std::vector<Constant*> initial)
{
    return new ConstantArray(initial, _type);
}    

ConstantArray* ConstantArray::getZero(ArrayType* _type)
{
    // auto tmp = std::vector<Constant*>(_type->getSize(), Constant::getZero(_type->getInnerType()));
    // return get(_type, std::move(tmp)); 
    auto res = get(_type, {});
    res->setZero();
    return res;
}

ConstantString* ConstantString::get(const std::string& _str)
{
    return new ConstantString(_str);
}

} // namespace ir
    
} // namespace sysy
