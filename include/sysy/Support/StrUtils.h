//===-------- StrUtils.h - String helper function---*- C++ -*--------------===//
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

#ifndef SYSY_STR_UTILS_H
#define SYSY_STR_UTILS_H

#include <iomanip>
#include <sstream>
#include <string>
#include <regex>

int strHex(std::string str);

int strOct(std::string str);

int strDec(std::string str);

float strFloat(std::string str);

float strFloatHex(std::string str);

std::string addIndent(const std::string& str);

std::string subLine(std::string str);

#endif