#ifndef SYSY_IR_TYPE_H
#define SYSY_IR_TYPE_H

#include <string>
#include <vector>

namespace sysy
{
namespace ir
{

class IntegerType;
class PointerType;

class Type
{
public:
    enum TypeID 
    {
        FloatTyID = 0,     ///< 32-bit floating point type
        VoidTyID,      ///< type with no size
        LabelTyID,     ///< Labels
        IntegerTyID,       ///< Arbitrary bit width integers
        FunctionTyID,      ///< Functions
        PointerTyID,       ///< Pointers
        ArrayTyID,         ///< Arrays
        FixedVectorTyID,   ///< Fixed width SIMD vector type
        ScalableVectorTyID, ///< Scalable SIMD vector type
        UndefTyID
    };
private:
    TypeID ID;
    unsigned size;

protected:
    explicit Type(TypeID id) :
        ID(id), size(0) {}
    
    TypeID getTypeId() { return ID; }
    unsigned getSize() { return size; }

public:
    bool isFloatTy() { return getTypeId() == FloatTyID; }
    bool isVoidTy() { return getTypeId() == VoidTyID; }
    bool isLabelTy() { return getTypeId() == LabelTyID; }
    bool isIntegerTy() { return getTypeId() == IntegerTyID; }
    bool isIntegerTy(unsigned width);
    bool isFunctionTy() { return getTypeId() == FunctionTyID; }
    bool isPointerTy() { return getTypeId() == PointerTyID; }
    bool isVectorTy() { return getTypeId() == FixedVectorTyID || getTypeId() == ScalableVectorTyID; }
    bool isArrayTy() { return getTypeId() == ArrayTyID; }
    bool isUndefTy() { return getTypeId() == UndefTyID; }

    virtual Type* getInnerType() = 0;
    virtual std::string getSignature() = 0;

    virtual size_t getAllocatedSize() = 0;

    friend bool sameType(Type* x, Type* y) { return x == y; }

    static Type* getFloatTy();
    static Type* getVoidTy();
    static Type* getUndefTy();
    static Type* getFunctionTy(Type* _retType, std::vector<Type*> _argType, bool isVarArg);
    static Type* getLabelTy();
    static Type* getIntegerTy(unsigned width);
    static Type* getInt1Ty();
    static Type* getInt32Ty();
    static Type* getInt8Ty();
    static Type* getPointerTy(Type* inner);
    static Type* getArrayTy(Type* inner, unsigned size);

    static Type* getInt8PtrTy();
    static Type* getInt32PtrTy();
    static Type* getFloatPtrTy();
}; // class Type

class LableType : public Type
{
public:
    LableType() : Type(LabelTyID) {}

    Type* getInnerType() override { return nullptr; }
    std::string getSignature() override { return "lable"; }

    size_t getAllocatedSize() override { return 0; }

    static LableType* get();
};

class VoidType : public Type
{
public:
    VoidType() : Type(VoidTyID) {}

    Type* getInnerType() override { return nullptr; }
    std::string getSignature() override { return "void"; }

    size_t getAllocatedSize() override { return 0; }
    
    static VoidType* get();
};

class UndefType : public Type
{
public:
    UndefType() : Type(UndefTyID) {}

    Type* getInnerType() override { return nullptr; }
    std::string getSignature() override { return "undef"; }

    size_t getAllocatedSize() override { return 0; }
    
    static UndefType* get();
};

class FloatType : public Type
{
public:
    FloatType() : Type(FloatTyID) {}

    Type* getInnerType() override { return nullptr; }
    std::string getSignature() override { return "float"; }

    size_t getAllocatedSize() override { return 4; }

    static FloatType* get();
}; // class Type

class IntegerType : public Type
{
private:
    unsigned width;
public:
    IntegerType(unsigned _width) : Type(IntegerTyID), width(_width) {}

    Type* getInnerType() override { return nullptr; }
    std::string getSignature() override { return "i" + std::to_string(width); }

    size_t getAllocatedSize() override { return width / 8; }

    static IntegerType* get(unsigned width);
    static IntegerType* getInt1Ty();
    static IntegerType* getInt32Ty();
    static IntegerType* getInt8Ty();
};

class PointerType : public Type
{
private:
    Type* inner;
public:
    PointerType(Type* _inner) : Type(PointerTyID), inner(_inner) {}
    PointerType() : Type(PointerTyID), inner(nullptr) {}

    Type* getInnerType() override { return inner; }
    std::string getSignature() override { return inner->getSignature() + "*"; }

    size_t getAllocatedSize() override { return 4; }

    static PointerType* get(Type* inner);

    static PointerType* getInt8PtrTy();
    static PointerType* getInt32PtrTy();
    static PointerType* getFloatPtrTy();

};

class ArrayType : public Type
{
private:
    Type* inner;
    unsigned size;
public:
    ArrayType(Type* _inner, unsigned _size) : Type(ArrayTyID), inner(_inner), size(_size) {}
    ArrayType() : Type(ArrayTyID), inner(nullptr) {}

    Type* getInnerType() override { return inner; }
    std::string getSignature() override { return "[" + std::to_string(size) + " x " + inner->getSignature() + "]"; }

    unsigned getSize() { return size; }

    size_t getAllocatedSize() override;

    static ArrayType* get(Type* inner, unsigned size);
};

class FunctionType : public Type
{
private:
    Type* retType;
    std::vector<Type*> argType;
    bool isVarArg;
public:
    FunctionType(Type* _retType, std::vector<Type*> _argType, bool _isVarArg) 
        : Type(FunctionTyID), retType(_retType), argType(std::move(_argType)), isVarArg(_isVarArg) {}

    FunctionType(Type* _retType, bool _isVarArg) 
        : Type(FunctionTyID), retType(_retType), isVarArg(_isVarArg) {}
    
    Type* getInnerType() override { return retType; }

    friend bool operator<(const FunctionType& x, const FunctionType& y) { return x.retType < y.retType; }

    std::vector<Type*>& getArg() { return argType; }

    Type* getRet() { return retType; }

    std::string getSignature() override
    {
        return retType->getSignature(); 
    }
    static FunctionType* get(Type* _retType, std::vector<Type*> _argType, bool isVarArg);

    static FunctionType* get(Type* _retType, bool isVarArg);

    size_t getAllocatedSize() override { return 0; }
};

// class 

} // namespace ir
} // namespace sysy

# endif