#ifndef SYSY_SYSYVISITOR_H
#define SYSY_SYSYVISITOR_H

#include "sysy/Frontend/SysYParserBaseVisitor.h"
#include "sysy/Frontend/Ast.h"


namespace sysy
{

class SysYVisitor : public SysYParserBaseVisitor
{
protected:
    SysYParser& parser;
public:
    SysYVisitor(SysYParser& _parser) : parser(_parser) {}

    // ~SysYVisitor() {}

    void printCst(antlr4::ParserRuleContext *ctx, int depth);

    void printSrc(SysYParser::CompUnitContext* ctx);

    std::vector<ast::ExprNode*> constInitValDfs(SysYParser::ConstInitValContext *ctx, int idx);

    std::vector<ast::ExprNode*> varInitValDfs(SysYParser::InitValContext *ctx, int idx);
    
    antlrcpp::Any visitCompUnit(SysYParser::CompUnitContext *ctx) override;

    antlrcpp::Any visitElement(SysYParser::ElementContext *ctx) override;

    antlrcpp::Any visitDecl(SysYParser::DeclContext *ctx) override;
  
    antlrcpp::Any visitConstDecl(SysYParser::ConstDeclContext *ctx) override;
  
    antlrcpp::Any visitBType(SysYParser::BTypeContext *ctx) override;
  
    antlrcpp::Any visitConstDef(SysYParser::ConstDefContext *ctx) override;
  
    antlrcpp::Any visitConstInitVal(SysYParser::ConstInitValContext *ctx) override;
  
    antlrcpp::Any visitVarDecl(SysYParser::VarDeclContext *ctx) override;
  
    antlrcpp::Any visitVarDecl_in(SysYParser::VarDecl_inContext *ctx) override;
  
    antlrcpp::Any visitVarDef(SysYParser::VarDefContext *ctx) override;
  
    antlrcpp::Any visitInitVal(SysYParser::InitValContext *ctx) override;
  
    antlrcpp::Any visitFuncDef(SysYParser::FuncDefContext *ctx) override;
  
    antlrcpp::Any visitFuncType(SysYParser::FuncTypeContext *ctx) override;
  
    antlrcpp::Any visitFuncFParams(SysYParser::FuncFParamsContext *ctx) override;
  
    antlrcpp::Any visitFuncFParam(SysYParser::FuncFParamContext *ctx) override;
  
    antlrcpp::Any visitBlock(SysYParser::BlockContext *ctx) override;
  
    antlrcpp::Any visitBlockItem(SysYParser::BlockItemContext *ctx) override;
  
    antlrcpp::Any visitStmt_in(SysYParser::Stmt_inContext *ctx) override;
  
    antlrcpp::Any visitStmt(SysYParser::StmtContext *ctx) override;
  
    antlrcpp::Any visitExp(SysYParser::ExpContext *ctx) override;
  
    antlrcpp::Any visitCond(SysYParser::CondContext *ctx) override;
  
    antlrcpp::Any visitLVal(SysYParser::LValContext *ctx) override;
  
    antlrcpp::Any visitPrimaryExp(SysYParser::PrimaryExpContext *ctx) override;
  
    antlrcpp::Any visitNumber(SysYParser::NumberContext *ctx) override;
  
    antlrcpp::Any visitUnaryExp(SysYParser::UnaryExpContext *ctx) override;
  
    antlrcpp::Any visitUnaryOp(SysYParser::UnaryOpContext *ctx) override;
  
    antlrcpp::Any visitFuncRParams(SysYParser::FuncRParamsContext *ctx) override;
  
    antlrcpp::Any visitMulExp(SysYParser::MulExpContext *ctx) override;
  
    antlrcpp::Any visitAddExp(SysYParser::AddExpContext *ctx) override;
  
    antlrcpp::Any visitRelExp(SysYParser::RelExpContext *ctx) override;
  
    antlrcpp::Any visitEqExp(SysYParser::EqExpContext *ctx) override;
  
    antlrcpp::Any visitLAndExp(SysYParser::LAndExpContext *ctx) override;
  
    antlrcpp::Any visitLOrExp(SysYParser::LOrExpContext *ctx) override;
  
    antlrcpp::Any visitConstExp(SysYParser::ConstExpContext *ctx) override;
  
    antlrcpp::Any visitIntConst(SysYParser::IntConstContext *ctx) override;

    antlrcpp::Any visitFloatConst(SysYParser::FloatConstContext *ctx) override;
}; // class SysYVisitor

} // sysy

#endif