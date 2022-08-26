#include "sysy/IR/User.h"

namespace sysy
{
namespace ir
{

User::User(Type* _type, std::vector<Value*> _operands, std::string _name) : Value(_type, _name)
{
    for(auto val : _operands)
        addOperand(val);
}

void User::addOperand(Value* _value) 
{
    auto ptr = std::make_shared<Use>(_value, this, operands.size());
    _value->addUse(ptr);
    operands.push_back(ptr);
}

void User::replaceOperand(unsigned index, Value* _value)
{
    assert(operands.size() > index);
    if(!_value) 
    {
        operands.at(index) = nullptr;
        return;
    }// FIXME Maybe bugs
    auto ptr = std::make_shared<Use>(_value, this, index); // 2. initial new use
    _value->addUse(ptr); // 3. add a new use
    operands.at(index) = ptr; // 4. set the operand.
}

void User::replaceOperandWith(unsigned index, Value* _value)
{
    assert(operands.size() > index);
    if(!_value) 
    {
        operands.at(index)->getValue()->removeUse(operands.at(index));
        operands.at(index) = nullptr;
        return;
    }// FIXME Maybe bugs
    operands.at(index)->getValue()->removeUse(operands.at(index)); // 1. remove corrent use
    auto ptr = std::make_shared<Use>(_value, this, index); // 2. initial new use
    _value->addUse(ptr); // 3. add a new use
    operands.at(index) = ptr; // 4. set the operand.
}

void User::removeOperand(const std::shared_ptr<Use>& use)
{
    use->getValue()->removeUse(use);
    operands.at(use->getPos()) = nullptr;
}

void User::UnUseAll()
{
    for(auto ops : operands)
        if(ops)
            ops->getValue()->getUses().remove(ops);
    operands.clear();
}



} // namespace ir
    
} // namespace sysy
