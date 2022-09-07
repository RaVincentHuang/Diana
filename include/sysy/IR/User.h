#ifndef SYSY_IR_USER_H
#define SYSY_IR_USER_H

#include "sysy/Support/Debug.h"
#include "sysy/IR/Value.h"
#include "sysy/IR/Use.h"
#include <list>
#include <memory>
#include <algorithm>

namespace sysy
{
namespace ir
{

class User : public Value
{
protected:
    using OprandList = std::vector<std::shared_ptr<Use>>;
    OprandList operands;
public:
    User(Type* _type, std::vector<Value*> _operands, std::string _name = "");
    User(Type* _type, std::string _name = "") : Value(_type, _name) {}

    void addOperand(Value* _value);

    OprandList& getOprands() { return operands; }

    void replaceOperand(unsigned index, Value* _value);

    void replaceOperandWith(unsigned index, Value* _value);

    void removeOperand(const std::shared_ptr<Use>& use);

    void UnUseAll(); // TODO replace it to the ~User()
};

} // namespace ir   
} // namespace sysy

#endif