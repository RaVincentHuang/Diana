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
        this->addChildren(child);   \            
}

#define SET_IT_PARENT_VEC(vec)      \
    for(auto iter : vec)            \
    {                               \
        iter->setParent(this);      \
        this->addChildren(iter);   \
    }

#define FREE_VEC(vec)               \
    for(auto iter : vec)            \
        delete iter;

// TODO  
#define TODOLIST() do                                                          \
{                                                                              \
    TODO("Features yet to be added:");      \
    TODO("Frontend->");                     \
    TODO("\tASTgen");                       \
    TODO("\tSyntax check");                 \
    TODO("\tSemantic check");             \
    TODO("\tMlirgen");                    \
    TODO("Backend");                      \
} while(false)

#endif