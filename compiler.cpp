// #include "sysy/Support/CommandLine.h"
// #include "sysy/Frontend/Frontend.h"
// #include "sysy/IR/Module.h"
// #include "sysy/IR/PassManager.h"
// #include "sysy/Target/Module.h"
// #include "sysy/Transform/Transform.h"
// #include "sysy/IR/IRDump.h"
// #include <string>
// #include <iostream>

// using namespace sysy;

// cmdline::parser cmd;

// int main(int argc, char *argv[])
// {
//     cmd.add<std::string>("input", 0, "input filename");
//     cmd.add<std::string>("output", 'o', "output filename", false, "a.s");
//     cmd.add<std::string>("gen", 'g', "gen what type output", false, "asm", cmdline::oneof<std::string>("asm", "ast", "cst", "ir"));
//     cmd.add<std::string>("opt", 0, "opt level", false, "O0", cmdline::oneof<std::string>("O0", "O2"));
//     cmd.parse_check(argc, argv);
//     auto InputFilename = cmd.get<std::string>("input");
//     auto Gen = cmd.get<std::string>("gen");
//     auto OutputFilename = cmd.get<std::string>("output");
//     auto Opt = cmd.get<std::string>("opt");
//     std::cout << InputFilename << std::endl;
//     ir::Module* module;
//     if(!(module = FrontendMain(InputFilename, Gen)))
//         return 0;

//     auto pm = new ir::ModulePassManager;
//     ir::initialIRTransform(Opt, pm);

//     pm->run(module);

//     // B(ir::dumpIR(module));
//     B("here");

//     mc::Module* mc_module = new mc::Module(module, argc);
    
//     mc_module->run(OutputFilename);// TODO: 写成mc::PassManager的形式
//     return 0;
// }


#include "sysy/Support/CommandLine.h"
#include "sysy/Frontend/Frontend.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/PassManager.h"
#include "sysy/Target/Module.h"
#include "sysy/Transform/Transform.h"
#include "sysy/IR/IRDump.h"
#include <string>
#include <iostream>

using namespace sysy;

int main(int argc, char *argv[])
{
    ir::Module* module;
    module = FrontendMain(argv[4], "asm");
    auto pm = new ir::ModulePassManager;
    if (argc == 6)
        ir::initialIRTransform("O2", pm);
    else
        ir::initialIRTransform("O0", pm);
    pm->run(module);

    // B(ir::dumpIR(module));
    // B("here");

    mc::Module* mc_module = new mc::Module(module, argc);
    mc_module->run(argv[3]);// TODO: 写成mc::PassManager的形式
    return 0;
}

