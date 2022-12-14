//===-------------------AstDump.h - The dumper of Ast ---*- C++ -*---------===//
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
#ifndef SYSY_ASTDUMP_H
#define SYSY_ASTDUMP_H

#include "sysy/Frontend/Ast.h"

namespace sysy
{
namespace ast
{
    
void dump(AstNode* node);
    
} // namespace ast  
} // namespace sysy






#endif
