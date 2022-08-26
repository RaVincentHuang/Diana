#ifndef SYSY_IR_CONTEXT_H
#define SYSY_IR_CONTEXT_H

#include "sysy/IR/Type.h"
#include "sysy/IR/Constant.h"
#include <map>
#include <set>

namespace sysy
{
namespace ir
{
class Constant;
class UndefValue;
class Context
{
private:
    // unsigned localCount = 0, golbalCount = 0;
public:
    FloatType FloatTy;
    IntegerType Int1Ty, Int8Ty, Int32Ty;

    LableType LabelTy;
    VoidType VoidTy;
    UndefType UndefTy;

    std::map<Type*, PointerType> PointerTy;
    std::map<std::pair<Type*, unsigned>, ArrayType> ArrayTy;

    std::set<FunctionType> FunctionTy;

    Context() : FloatTy(), Int1Ty(1), Int8Ty(8), Int32Ty(32), LabelTy(), VoidTy(), UndefTy() {}

    static Context& getInstance()
    {
        static Context ctx;
        return ctx;
    }
};
}
}

#endif