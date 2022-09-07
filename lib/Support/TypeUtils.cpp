#include "sysy/Support/TypeUtils.h"
#include "sysy/Support/Debug.h"
#include "sysy/Support/Common.h"
#include "sysy/IR/Type.h"


namespace sysy
{

Line& Line::getInstance()
{
    static Line ins;
    return ins;
}

namespace ast
{

ir::Type* AstType2IrType(AstType type)
{
    switch (type)
    {
    case INT:
        return ir::Type::getInt32Ty();
    case FLOAT:
        return ir::Type::getFloatTy();
    }
    ErrorMsg("Invalid AstType");
}

} // namespace ast 

namespace ir
{

bool isSameType(ir::Type* type1, ir::Type* type2)
{
    return type1->isFloatTy() ^ type2->isIntegerTy();
}
    
} // namespace ir



} // namespace sysy
