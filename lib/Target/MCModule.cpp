#include "sysy/Target/Function.h"
#include "sysy/Target/Module.h"
#include "sysy/Target/GlobalObject.h"
#include "sysy/Target/Regallocator.h"
#include "sysy/Target/Instruction.h"
#include "sysy/Target/Pass.h"
#include <fstream>
namespace sysy 
{
namespace mc 
{

Module::Module(ir::Module *ir, int _argc) {
    argc = _argc;
    for (auto gv : ir->getInstance().__globalValue) {
        std::string name = "__" + gv->getName();
        GlobalObject* gb = new GlobalObject(name);

        if (gv->getInitial()->isZero()) {
            bss_flag = true;
            gb->size = gv->getValueType()->getAllocatedSize();
            gb->is_zero = true;
        }
        else {
            data_flag = true;
            gb->pushValue(gv->getInitial());
            gb->is_zero = false;
        }
        global_objects.push_back(gb);
        global_map[gv] = gb;
        rev_global_map[gb] = gv;
    }

    for (auto i : ir->getInstance().__function) {
        Function *func = new Function(i->getName(), this, i->isDeclare(), i->arg_size());
        functions.push_back(func);
        func_map[i] = func;
    }

    // std::cout << "finish mcfunc generate\n";


    for (auto i : ir->getInstance().__function) {
        if (i->isDeclare())
            continue;
        func_map[i]->analyseFunc(i);
    }


    // std::cout << "finish mcfunc analysis\n";
}

void Module::run(std::string file_name) {
    // std::cout<<"nide"<<std::endl;
    for (auto func : functions) {
        if (func->blocks.empty())
            continue;
        
        remove_bottom_to_top(func);

        remove_multi_store(func);
        // remove_multi_load(func);
        

        analyseType(func);
        func->reg_alloc();
        func->splitSP();
        func->recompute_offset();

        remove_bottom_to_top(func);
        merge_block(func);
        remove_branch(func);
        // for (auto block : func->blocks) {
        //     for (auto i : block->instructions) {
        //         if (auto binary = dynamic_cast<BinaryImm*>(i)) {
        //             int 
        //             assert(binary->rhs > 4096);
        //         }
        //     }
        // }


        // remove_same_move(func);
    }
    

    // std::cout << "finish reg allocate\n";

    gen_asm(file_name);
}

std::string Module::gen_global() {
    std::string code = "";
    if (data_flag) {
        code += ".section .data\n";
        for (auto gb : global_objects) {
            if (gb->is_zero)
                continue;            
            if (gb->is_string) {
                code += gb->name + ":\n";
                code += "\t.asciz\t";
                code += gb->str_value + "\n";
            }
            else if (gb->is_float) {
                code += ".align\n" + gb->name + ":\n";
                for (auto& value : gb->values) {
                    code += "\t.long\t";
                    code += GlobalObject::dec2hex(value) + "\n";
                }
            }
            else {
                code += ".align\n" + gb->name + ":\n";
                code += "\t.4byte\t";
                for (auto& value : gb->values) {
                    code += std::to_string(value);
                    code += (&value == &gb->values.back()) ? "\n" : ", ";
                }
            }
        }
    }
    if (bss_flag) {
        code += ".section .bss\n";
        for (auto gb : global_objects) {
            if (!gb->is_zero)
                continue;            
            code += ".align\n";
            code += gb->name + ":\n";
            code += "\t.space " + std::to_string(gb->size) + "\n";
        }
    }
    return code;
}

std::string gen_threads() {
    return "__thread_create:\n \
\tpush {r7}\n \
\tsub sp, sp, #16777216\n \
\tmov r2, #4\n \
__thread_create_1:\n \
\tsub r2, r2, #1\n \
\tcmp r2, #0\n \
\tbeq __thread_create_2\n \
\tmov r7, #120\n \
\tmov r0, #273\n \
\tmov r1, sp\n \
\tswi #0\n \
\tcmp r0, #0\n \
\tbne __thread_create_1\n \
__thread_create_2:\n \
\tmov r0, r2\n \
\tadd sp, sp, #16777216\n \
\tpop {r7}\n \
\tbx lr\n \
\n \
__thread_join:\n \
\tpush {r7} \n \
\tcmp r0, #0\n \
\tbeq __thread_join_2\n \
__thread_join_1:\n \
\tmov r7, #1\n \
\tswi #0\n \
__thread_join_2:\n \
\tpush {r0, r1, r2, r3}\n \
\tmov r1, #4\n \
__thread_join_3:\n \
\tsub r1, r1, #1\n \
\tcmp r1, #0\n \
\tbeq __thread_join_4\n \
\tpush {r1, lr}\n \
\tsub sp, sp, #4\n \
\tmov r0, sp\n \
\tbl wait\n \
\tadd sp, sp, #4\n \
\tpop {r1, lr}\n \
\tb __thread_join_3\n \
__thread_join_4:\n \
\tpop {r0, r1, r2, r3}\n \
\tpop {r7} \n \
\tbx lr";
}


void Module::gen_asm(std::string file_name) { 
    std::string code = "";
    code += ".arch armv7ve\n";
    code += ".arm\n";
    
    // std::cout << "before gen global asm\n";
    code += gen_global();
    // std::cout << "finish gen global asm\n";
    
    code += ".global main\n";
    code += ".section .text\n";
    for (auto fun : functions) 
        code += fun->gen_asm();

    if (create_threads)
        code += gen_threads();

    // std::cout << "finish functions ASM generate\n";

    std::ofstream ofs;
    ofs.open(file_name, std::ios::out);
    ofs << code << "\n";
    ofs.close();
}


}
}