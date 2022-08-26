#ifndef SYSY_TARGET_Pass_H
#define SYSY_TARGET_Pass_H

#include "sysy/Target/Function.h"
#include "sysy/Target/Block.h"
#include "sysy/Target/Instruction.h"


namespace sysy 
{
namespace mc 
{

void analyseType(Function* func);
void remove_top_to_bottom(Function* func);
void remove_bottom_to_top(Function* func);
void remove_same_move(Function* func);

void remove_multi_store(Function* func);
void remove_multi_load(Function* func);
void remove_redundant_move (Function* func);

void merge_block(Function* func);
void remove_branch(Function* func);

}
}

#endif