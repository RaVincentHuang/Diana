//===--------------SysYOp.h - Operations in SysY -*- C++ -*----------------===//
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
#ifndef SYSY_SYSYOP_H
#define SYSY_SYSYOP_H

namespace sysy
{
namespace ast
{
enum BinaryOp {BOP_ADD = 0, BOP_SUB, BOP_MUL, BOP_DIV, BOP_MOD, 
                ROP_EQL, ROP_NEQ, ROP_LES, ROP_GRT, ROP_LEQ, ROP_GEQ, ROP_OR, ROP_AND, BOP_NONE};

enum UnaryOp {UOP_ADD = 0, UOP_SUB, UOP_NOT, UOP_NONE};

// enum RelationOp {ROP_EQL = 0, ROP_NEQ, ROP_LES, ROP_GRT, ROP_LEQ, ROP_GEQ, ROP_OR, ROP_AND, ROP_NONE};
} // namespace ast

} // namespace sysy

#endif