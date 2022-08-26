#ifndef SYSY_IR_IRGEN_H
#define SYSY_IR_IRGEN_H

#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/Frontend/Ast.h"
#include <stack>


namespace sysy
{
namespace ir
{
class Module;
using namespace ast;

class BasicBlockTarget
{
private:
    std::stack<BasicBlock*> loopEntry, loopAfter;
    std::stack<BasicBlock*> condTrue, condFalse;
    // BasicBlock* loopEntry = nullptr,* loopAfter = nullptr;
    // BasicBlock* condTrue = nullptr,* condFalse = nullptr;
public:
    BasicBlock* getEntry() { return loopEntry.top(); }
    BasicBlock* getAfter() { return loopAfter.top(); }

    BasicBlock* getTrue() { return condTrue.top(); }
    BasicBlock* getFalse() { return condFalse.top(); }
    
    void setEntry(BasicBlock* entry) { loopEntry.push(entry); }
    void setAfter(BasicBlock* after) { loopAfter.push(after); }
    void setTrue(BasicBlock* _condTrue) { condTrue.push(_condTrue); }
    void exitTrue() { condTrue.pop(); }
    void setFalse(BasicBlock* _condFalse) { condFalse.push(_condFalse); }
    void exitFalse() { condFalse.pop(); }
    void setLoop(BasicBlock* entry, BasicBlock* after) { setEntry(entry); setAfter(after); }
    void exitLoop() { loopEntry.pop(); loopAfter.pop(); }
    void setCond(BasicBlock* _condTrue, BasicBlock* _condFalse) {setTrue(_condTrue); setFalse(_condFalse); }
    void exitCond() { condTrue.pop(); condFalse.pop(); }

    static BasicBlockTarget& getInstance()
    {
        static BasicBlockTarget instance;
        return instance;
    }
};

Value* irEmit(ExprNode* expr);
Value* irEmit(FunctionCallExprNode* funcCall);
Value* irEmit(BinaryExprNode* binExpr);
Value* irEmit(UnaryExprNode* unaryExpr);
Value* irEmit(NumberNode* number);
Value* irEmit(LvalNode* lval);
Value* irEmit(StmtNode* stmt);
Value* irEmit(BlockNode* block);
Value* irEmit(IfStmtNode* ifStmt);
Value* irEmit(AssignStmtNode* assignStmt);
Value* irEmit(ExprStmtNode* exprStmt);
Value* irEmit(ForStmtNode* forStmt);
Value* irEmit(WhileStmtNode* whileStmt);
Value* irEmit(BreakStmtNode* breakStmt);
Value* irEmit(ContinueStmtNode* continueStmt);
Value* irEmit(ReturnStmtNode* returnStmt);
Value* irEmit(DeclNode* decl);
Value* irEmit(ConstDeclNode* constDecl);
Value* irEmit(VarDeclNode* varDecl);
Function* irEmit(FuncDefNode* funcDef);

Module* emitIR(CompUnitNode* compUnit);
std::string dumpIR(Module* TheModule);

}

} // namespace ir



#endif