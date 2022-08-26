#ifndef SYSY_IR_CONSTANT_H
#define SYSY_IR_CONSTANT_H

#include "sysy/IR/User.h"
#include "sysy/Support/iterator_range.h"
#include <algorithm>

namespace sysy
{
namespace ir
{

class Constant : public User
{
protected:
    bool zeroinitializer = false;
public:
    Constant(Type* _type, std::vector<Value*> _operands) 
        : User(_type, _operands) {} 
    Constant(Type* _type) : User(_type) {}

    virtual bool isZero() = 0;

    void setZero() { zeroinitializer = true; }

    static Constant* getZero(Type* _type);

    static Constant* getString(const std::string& str);
};

class ConstantInt : public Constant
{
private:
    int value;
public:
    ConstantInt(Type* _type, int _value) : Constant(_type), value(_value) {}

    bool isZero() override { return value == 0; }

    bool isOne() { return value == 1; }

    static ConstantInt* get(Type* _type, int _value) ;

    static ConstantInt* get(unsigned width, int _value);

    static ConstantInt* getZero(Type* _type);

    int getValue() { return value; } 

    std::string getSignature() override;
};

class ConstantFP : public Constant
{
private:
    float value;
public:
    ConstantFP(float _value) : Constant(Type::getFloatTy()), value(_value) {}

    bool isZero() override { return value == 0; }
    bool isOne() { return value == 1; }

    static ConstantFP* get(float _value);

    static ConstantFP* getZero();

    float getValue() { return value; } 

    std::string getSignature() override;

};

class ConstantArray : public Constant
{
private:
    std::vector<Constant*> items;
public:
    ConstantArray(std::vector<Constant*> initial, ArrayType* _type) : Constant(_type),
        items(std::move(initial)) 
    {
        OprandList tmp; unsigned idx = 0;
        for(auto init : initial)
            tmp.push_back(std::make_shared<Use>(init, this, idx++));
        operands = std::move(tmp);
    }

    // bool isZero() { return std::all_of(items.begin(), items.end(), [](Constant* x) -> bool {
    //     return x->isZero();
    // }); }

    bool isZero() override { return zeroinitializer || std::all_of(items.begin(), items.end(), [](Constant* x) -> bool {
        return x->isZero();
    }); }

    static ConstantArray* get(ArrayType* _type, std::vector<Constant*> initial);

    static ConstantArray* getZero(ArrayType* _type);

    Constant* getValue(size_t idx) { return items.at(idx); }

    size_t size() { return items.size(); }

    std::string getSignature() override;
};

class ConstantString : public Constant
{
private:
    std::string str;
public:
    ConstantString(const std::string& _str) : Constant(Type::getInt8PtrTy()), str(_str) {}

    static ConstantString* get(const std::string& _str);

    bool isZero() override { return false; }

    std::string getStr() { return str; }

    std::string getSignature() override;
};

} // namespace ir
    
} // namespace sysy


#endif