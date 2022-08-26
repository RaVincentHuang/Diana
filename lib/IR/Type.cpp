#include "sysy/IR/Type.h"
#include "sysy/IR/Context.h"
#include "sysy/Support/debug.h"

namespace sysy
{
namespace ir
{

Type* Type::getFunctionTy(Type* _retType, std::vector<Type*> _argType, bool isVarArg) 
{
    return FunctionType::get(_retType, _argType, isVarArg);
}

Type* Type::getFloatTy() { return FloatType::get(); }

Type* Type::getVoidTy() { return VoidType::get(); }

Type* Type::getUndefTy() { return UndefType::get(); }

Type* Type::getLabelTy() { return LableType::get(); }

Type* Type::getIntegerTy(unsigned width) { return IntegerType::get(width); }

Type* Type::getInt1Ty() { return IntegerType::getInt1Ty(); }

Type* Type::getInt32Ty() { return IntegerType::getInt32Ty(); }

Type* Type::getInt8Ty() { return IntegerType::getInt8Ty(); }

Type* Type::getPointerTy(Type* inner) { return PointerType::get(inner); }

Type* Type::getInt8PtrTy() { return PointerType::getInt8PtrTy(); }

Type* Type::getInt32PtrTy() { return PointerType::getInt32PtrTy(); }

Type* Type::getFloatPtrTy() { return PointerType::getFloatPtrTy(); }

Type* Type::getArrayTy(Type* inner, unsigned size) { return ArrayType::get(inner, size); }

LableType* LableType::get() 
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.LabelTy);
}

VoidType* VoidType::get() 
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.VoidTy);
}

UndefType* UndefType::get()
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.UndefTy);
}

FloatType* FloatType::get()
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.FloatTy);
}

IntegerType* IntegerType::get(unsigned width) 
{   
    switch (width)
    {
    case 1:
        return getInt1Ty();
    case 8:
        return getInt8Ty();
    case 32:
        return getInt32Ty();
    default:
        ErrorMsg("Invalid Int type.");
    }
}

IntegerType* IntegerType::getInt1Ty() 
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.Int1Ty);
}

IntegerType* IntegerType::getInt32Ty() 
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.Int32Ty);
}

IntegerType* IntegerType::getInt8Ty()
{
    auto& ctx = Context::getInstance();
    return std::addressof(ctx.Int8Ty);
}

PointerType* PointerType::get(Type* inner) 
{
    auto& ctx = Context::getInstance(); 
    if(!ctx.PointerTy.count(inner))
        ctx.PointerTy[inner] = std::move(PointerType(inner));
    return std::addressof(ctx.PointerTy[inner]);
}

PointerType* PointerType::getInt8PtrTy() { return get(Type::getInt8Ty()); }

PointerType* PointerType::getInt32PtrTy() { return get(Type::getInt32Ty()); }

PointerType* PointerType::getFloatPtrTy() { return get(Type::getFloatTy()); }

ArrayType* ArrayType::get(Type* inner, unsigned size) 
{
    auto& ctx = Context::getInstance();
    auto type = std::make_pair(inner, size);
    if(!ctx.ArrayTy.count(type))
        ctx.ArrayTy[type] = std::move(ArrayType(inner, size));
    return std::addressof(ctx.ArrayTy[type]);
}

size_t ArrayType::getAllocatedSize()
{
    size_t res = 4;
    Type* _type = this;
    while(auto type = dynamic_cast<ArrayType*>(_type))
    {
        res *= type->getSize();
        _type = type->getInnerType(); 
    }
    return res;
}

FunctionType* FunctionType::get(Type* _retType, std::vector<Type*> _argType, bool isVarArg = false)
{
    return new FunctionType(_retType, _argType, isVarArg);
}

FunctionType* FunctionType::get(Type* _retType, bool isVarArg)
{
    return new FunctionType(_retType, isVarArg);
}

} // namespace ir
} // namespace sysy
