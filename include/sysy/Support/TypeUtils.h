//===------ TypeUtils.h - IR type system helper functions-*- C++ -*--------===//
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