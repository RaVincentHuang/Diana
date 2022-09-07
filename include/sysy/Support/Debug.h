//=== --------\Debug.h - Debug functions and macros -----*- C++ -*---------===//
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
#ifndef SYSY_DEBUG_H
#define SYSY_DEBUG_H

#include <iostream>
#include <cassert>

#define SHELL_DEFAULT "\033[0m"
#define SHELL_BLACK "\033[30m"
#define SHELL_RED "\033[31m"
#define SHELL_GREEN "\033[32m"
#define SHELL_ORANGE "\033[33m"
#define SHELL_BLUE "\033[34m"
#define SHELL_PURPLE "\033[35m"
#define SHELL_CYAN "\033[36m"
#define SHELL_TODO "\033[30;43m"
#define TM_DARK_WHITE "\033[37m"
#define TM_BRIGHT_BLACK "\033[90m"
#define TM_BRIGHT_RED "\033[91m"
#define TM_BRIGHT_GREEN "\033[92m"
#define TM_BRIGHT_YELLOW "\033[93m"
#define TM_BRIGHT_BLUE "\033[94m"
#define TM_BRIGHT_MAGENTA "\033[95m"
#define TM_BRIGHT_CYAN "\033[96m"
#define TM_WHITE "\033[97m"

#define B(str) std::cout << SHELL_BLUE << str << SHELL_DEFAULT << std::endl;
#define R(str) std::cout << SHELL_RED << str << SHELL_DEFAULT << std::endl;
#define G(str) std::cout << SHELL_GREEN << str << SHELL_DEFAULT << std::endl;
// #define B(str)
// #define R(str)
// #define G(str)

#define PrintMsg(str) std::cout << SHELL_GREEN << str << SHELL_DEFAULT << std::endl;
// #define PrintMsg(str)

#define ErrorMsg(str)  do{ std::cout << SHELL_RED << str << SHELL_DEFAULT << std::endl; \
                    assert(false); } while(false)

#define TODO(str) std::cout << SHELL_TODO << str << SHELL_DEFAULT << std::endl;

#define EXIT assert(false);

#endif