#ifndef SYSY_IR_GLOBALVALUE_H
#define SYSY_IR_GLOBALVALUE_H

#include "sysy/IR/User.h"
#include "sysy/IR/Constant.h"

namespace sysy
{
namespace ir
{

class GlobalValue : public User
{
private:
    Constant* initial;
    bool isConst;
public:
    GlobalValue(Type* _type, bool _isConst, Constant* _initial = nullptr, std::string _name = "")
        : User(_type, _name), initial(_initial), isConst(_isConst) {}

    Type* getValueType() { return getType()->getInnerType(); }

    bool isConstant() { return isConst; }

    Constant* getInitial() { return initial; }

    std::string getSignature() override;

    static GlobalValue* Create(Type* _type, bool _isConst, Constant* _initial = nullptr, std::string _name = ""); 
};

} // namespace ir
    
} // namespace sysy


#endif