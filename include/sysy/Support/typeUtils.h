#ifndef SYSY_TYPE_UTILS_H
#define SYSY_TYPE_UTILS_H
#include "sysy/Frontend/AstType.h"
#include "sysy/IR/Type.h"


namespace sysy
{

namespace ast
{

ir::Type* AstType2IrType(AstType type);


} // namespace ast

namespace ir
{
    
bool isSameType(ir::Type* type1, ir::Type* type2);

} // namespace ir


} // namespace sysy

#endif