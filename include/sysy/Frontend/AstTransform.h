//===-----------AstTransform.h - The Transform passes of Ast -*- C++ -*-----===//
//
// Part of the Diana, a SysY Compiler, under the MIT License.
// See https://github.com/RaVincentHuang/Diana/blob/master/LICENSE for license 
// information.
// SPDX-License-Identifier: MIT License.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
//===----------------------------------------------------------------------===//
#ifndef SYSY_ASTTRANSFORM_H
#define SYSY_ASTTRANSFORM_H

#include "sysy/Frontend/Ast.h"

namespace sysy
{
namespace ast
{

// void variableDelivery(StmtNode* stmt);

void removeDeadStmts(BlockNode* block);

void optimizeAst(CompUnitNode* ast);

}
} // namespace sysy

#endif