#include "ANTLRInputStream.h"
#include "tree/ParseTreeWalker.h"
#include "sysy/Frontend/SysYLexer.h"
#include "sysy/Frontend/SysYParser.h"
#include "sysy/Frontend/SysYParserBaseVisitor.h"
#include "sysy/Frontend/Frontend.h"
#include "sysy/Frontend/SysYVisitor.h"
#include "sysy/Frontend/Ast.h"
#include "sysy/Frontend/AstDump.h"
#include "sysy/Support/debug.h"
#include "sysy/Support/common.h"
#include "sysy/IR/IRGen.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/IRDump.h"
#include "sysy/IR/PassManager.h"
#include "sysy/Transform/Utils/Memory2Reg.h"
#include "sysy/Transform/Scalar/GVN.h"
#include "sysy/Transform/Scalar/DCE.h"
#include "sysy/Transform/Scalar/GCM.h"
#include "sysy/Transform/Utils/EliminatePhi.h"
#include "sysy/Transform/IPO/FunctionInline.h"
#include "sysy/Transform/IPO/IPDCE.h"
#include "sysy/Transform/Transform.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace sysy
{

ir::Module* FrontendMain(const std::string& filename, const std::string& Gen)
{
    std::ifstream infile(filename);
    std::stringstream buf;
    buf << infile.rdbuf();
    std::string str(buf.str());
    antlr4::ANTLRInputStream inputStream(str);
    SysYLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    SysYParser parser(&tokens);
    std::string Includedfile;
    SysYVisitor visitor(parser);
    // PrintMsg("Finish parser");
    if(Gen == "cst")
    {
        visitor.printCst(parser.compUnit(), 1);
        return nullptr;
    }
    else if(Gen == "src")
    {
        visitor.printSrc(parser.compUnit());
        return nullptr;
    }
    auto ast = visitor.visitCompUnit(parser.compUnit()).as<ast::CompUnitNode*>();
    // PrintMsg("Finish ast gen");
    if(Gen == "ast")
    {
        dump(ast);
        return nullptr;
    }
    ir::Module* mod = ir::emitIR(ast);
    if(Gen == "ir")
    {
        PrintMsg("Basic IR");
        ir::dumpIR(mod);
        auto pm = new ir::SSASet;
        pm->run(mod);
        PrintMsg("After Mem2Reg");
        ir::dumpIR(mod);

        auto scala = new ir::ModulePassManager;
        scala->addPass(new ir::ScalarPass(false));

        scala->run(mod);

        auto loopEaly = new ir::ModulePassManager;
        loopEaly->addPass(new ir::LoopEarlyPass);

        loopEaly->run(mod);
        PrintMsg("After Loop Unroll and LoopParallel");
        ir::dumpIR(mod);

        auto ipdce = new ir::ModulePassManager;
        ipdce->addPass(new ir::IPDCE);
        ipdce->run(mod);
        scala->run(mod);
        PrintMsg("After IPDCE");
        ir::dumpIR(mod);

        auto ipo = new ir::ModulePassManager;
        ipo->addPass(new ir::FunctionInline);

        ipo->run(mod);
        PrintMsg("After function inline");
        ir::dumpIR(mod);

        scala->run(mod);
        ir::dumpIR(mod);
        
        auto loop = new ir::LoopVariablePass;
        loop->run(mod);

        scala->run(mod);
        
        PrintMsg("After Loop opt");
        ir::dumpIR(mod);


        // auto unset = new ir::UnsetSSA;
        // unset->run(mod);
        // PrintMsg("After Unset");
        // ir::dumpIR(mod);

        // ir::llFileGen(mod);

        return nullptr;
    }
    return mod;
}

} // namespace sysy

