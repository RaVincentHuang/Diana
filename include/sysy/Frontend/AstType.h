#ifndef SYSY_ASTTYPE_H
#define SYSY_ASTTYPE_H

#include <functional>
#include <string>

namespace sysy
{
namespace ast
{

enum AstType
{
    VOID = 0,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    UNKNOW
};

AstType operator&&(AstType x, AstType y);
AstType operator||(AstType x, AstType y);

class Immediate
{
private:
    union
    {
        int Integer;
        float Float;
        double DoubleFloat;
    };
    AstType type;
public:
    Immediate() : type(UNKNOW) {}
    Immediate(int x) : Integer(x) ,type(INT) {}
    Immediate(float x) : Float(x), type(FLOAT) {}
    Immediate(double x) : DoubleFloat(x), type(DOUBLE) {}

    void operator=(int x) { this->Integer = x; this->type = INT; }

    void operator=(float x) { this->Float = x; this->type = FLOAT; }

    AstType getType() { return type; }

    // void operator=(Immediate other) { this->Integer = other.Integer; this->type = other.type; }

    std::string toString() const
    {
        switch(type)
        {
        case INT:
            return std::to_string(Integer);
        case FLOAT:
            return std::to_string(Float);
        default:
            return "";
        }
    }
    
    template<typename F>
    void computeAssign(Immediate x, F func) 
    {
        if(x.type == INT)
            this->Integer = func(x.Integer);
        else if(x.type == FLOAT)
            this->Float = func(x.Float);
        this->type = x.type;
    }
    
    template<typename F>
    void computeAssign(Immediate x, Immediate y, F func)
    {
        auto type = x.type && y.type;
        x.typeCast(type); y.typeCast(type);
        if(type == INT)
            this->Integer = func(x.Integer, y.Integer);
        else if(type == FLOAT)
            this->Float = func(x.Float, y.Float);
        this->type = type;
    }

    void typeCast(AstType type)
	{
		if(type == INT && this->type != INT)
            this->Integer = (int) this->Float;
        else if(type == FLOAT && this->type != FLOAT)
            this->Float = (float) this->Integer;

        this->type = type;
	}

    int getInt() 
    {
        if(type != INT)
            typeCast(INT);
        return this->Integer;
    }

    float getFloat() 
    {
        if(type != FLOAT)
            typeCast(FLOAT);
        return this->Float; 
    }
};

// class AstType
// {
// public:
//     enum BasicType
//     {
//         SYSY_int = 0,
//         SYSY_void
//     };

// protected:
//     BasicType basicType;

// public:
//     Location loc;
//     AstType(BasicType _basicType, Location _loc)
//         : basicType(_basicType),
//         loc(_loc) {}
//     virtual ~AstType() {}

//     const BasicType& getBasicType() const { return basicType; }

//     virtual bool sameType(AstType* type) { return type->basicType == basicType; }

//     virtual bool operator==(const AstType& type) { return type.basicType == basicType; }
// };

// class FuncType : AstType
// {

// };
    
} // namespace ast

} // namespace sysy



#endif