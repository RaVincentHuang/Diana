#ifndef SYSY_ASTTRAMSFORM_H
#define SYSY_ASTTRAMSFORM_H

#include "sysy/Frontend/Ast.h"

namespace sysy
{
namespace ast
{

void variableDelivery(StmtNode* stmt);

void removeDeadStmts(BlockNode* block);

void optimizeAst(CompUnitNode* ast);

}
} // namespace sysy

#endif