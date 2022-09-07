//===-----------Common.h - Common functions and macros ---*- C++ -*-------===//
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
#ifndef SYSY_COMMON_H
#define SYSY_COMMON_H

#include "sysy/Support/Debug.h"
#include <iostream>


namespace sysy
{
class Line
{
public:
    int start, end;
    static Line& getInstance();
};
}



#define SET_IT_PARENT(child)        \
    if(child != nullptr)            \
    {                               \
        child->setParent(this);     \
        this->addChildren(child.get());   \            
}

#define SET_IT_PARENT_VEC(vec)      \
    for(auto& iter : vec)            \
    {                               \
        iter->setParent(this);      \
        this->addChildren(iter.get());   \
    }

#define FREE_VEC(vec)               \
    for(auto& iter : vec)            \
        iter.reset();

#endif