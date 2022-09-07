//===- -----Frontend.h - The Main Function of Frontend -*- C++ -*----------===//
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
#ifndef SYSY_FRONTENDMAIN_H
#define SYSY_FRONTENDMAIN_H
#include "sysy/IR/Module.h"

#include <string>
namespace sysy
{
ir::Module* FrontendMain(const std::string& filename, const std::string& Gen);
}
#endif