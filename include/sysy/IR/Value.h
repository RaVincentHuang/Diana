#ifndef SYSY_IR_VALUE_H
#define SYSY_IR_VALUE_H

#include "sysy/IR/Type.h"
#include "sysy/IR/Use.h"
#include <string>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

namespace sysy
{
namespace ir
{

class Value
{
protected:
    std::string name;
    Type* type;
    std::list<std::shared_ptr<Use>> uses;
public:
    Value(Type* _type, std::string _name = "") : name(_name), type(_type) {}

    std::list<std::shared_ptr<Use>>& getUses() { return uses; };

    void addUse(std::shared_ptr<Use> use) { uses.push_back(use); }

    void removeUse(const std::shared_ptr<Use>& use) { uses.remove(use); }
    
    void removeUseIf(std::function<bool(std::shared_ptr<Use>)> func)
    {
        for(auto iter = uses.begin(); iter != uses.end(); iter++)
            if(func(*iter))
                uses.erase(iter);
    }

    void replaceAllUseWith(Value* value);

    void removeAllUseWith();

    std::string getName() { return name; }
    void setName(std::string _name) { name = _name; }

    Type* getType() { return type; }
    void setType(Type* _type) { type = _type; }
     
    virtual std::string getSignature() = 0;
};

class UndefValue : public Value
{
public:
    UndefValue() : Value(Type::getUndefTy(), "undef") {}
    std::string getSignature() override { return "undef"; }
    static UndefValue* Create();
};

} // namespace ir
} // namespace sysy

#endif