#ifndef SYSY_COMMON_H
#define SYSY_COMMON_H

#include "sysy/Support/debug.h"
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