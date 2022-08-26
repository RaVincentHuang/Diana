
// Generated from lib/Frontend/SysYParser.g4 by ANTLR 4.9.2

#pragma once


#include "antlr4-runtime.h"
#include "sysy/Frontend/SysYParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by SysYParser.
 */
class  SysYParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by SysYParser.
   */
    virtual antlrcpp::Any visitCompUnit(SysYParser::CompUnitContext *context) = 0;

    virtual antlrcpp::Any visitElement(SysYParser::ElementContext *context) = 0;

    virtual antlrcpp::Any visitDecl(SysYParser::DeclContext *context) = 0;

    virtual antlrcpp::Any visitConstDecl(SysYParser::ConstDeclContext *context) = 0;

    virtual antlrcpp::Any visitBType(SysYParser::BTypeContext *context) = 0;

    virtual antlrcpp::Any visitConstDef(SysYParser::ConstDefContext *context) = 0;

    virtual antlrcpp::Any visitConstInitVal(SysYParser::ConstInitValContext *context) = 0;

    virtual antlrcpp::Any visitVarDecl(SysYParser::VarDeclContext *context) = 0;

    virtual antlrcpp::Any visitVarDecl_in(SysYParser::VarDecl_inContext *context) = 0;

    virtual antlrcpp::Any visitVarDef(SysYParser::VarDefContext *context) = 0;

    virtual antlrcpp::Any visitInitVal(SysYParser::InitValContext *context) = 0;

    virtual antlrcpp::Any visitFuncDef(SysYParser::FuncDefContext *context) = 0;

    virtual antlrcpp::Any visitFuncType(SysYParser::FuncTypeContext *context) = 0;

    virtual antlrcpp::Any visitFuncFParams(SysYParser::FuncFParamsContext *context) = 0;

    virtual antlrcpp::Any visitFuncFParam(SysYParser::FuncFParamContext *context) = 0;

    virtual antlrcpp::Any visitBlock(SysYParser::BlockContext *context) = 0;

    virtual antlrcpp::Any visitBlockItem(SysYParser::BlockItemContext *context) = 0;

    virtual antlrcpp::Any visitStmt_in(SysYParser::Stmt_inContext *context) = 0;

    virtual antlrcpp::Any visitStmt(SysYParser::StmtContext *context) = 0;

    virtual antlrcpp::Any visitExp(SysYParser::ExpContext *context) = 0;

    virtual antlrcpp::Any visitCond(SysYParser::CondContext *context) = 0;

    virtual antlrcpp::Any visitLVal(SysYParser::LValContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryExp(SysYParser::PrimaryExpContext *context) = 0;

    virtual antlrcpp::Any visitNumber(SysYParser::NumberContext *context) = 0;

    virtual antlrcpp::Any visitUnaryExp(SysYParser::UnaryExpContext *context) = 0;

    virtual antlrcpp::Any visitUnaryOp(SysYParser::UnaryOpContext *context) = 0;

    virtual antlrcpp::Any visitFuncRParams(SysYParser::FuncRParamsContext *context) = 0;

    virtual antlrcpp::Any visitMulExp(SysYParser::MulExpContext *context) = 0;

    virtual antlrcpp::Any visitAddExp(SysYParser::AddExpContext *context) = 0;

    virtual antlrcpp::Any visitRelExp(SysYParser::RelExpContext *context) = 0;

    virtual antlrcpp::Any visitEqExp(SysYParser::EqExpContext *context) = 0;

    virtual antlrcpp::Any visitLAndExp(SysYParser::LAndExpContext *context) = 0;

    virtual antlrcpp::Any visitLOrExp(SysYParser::LOrExpContext *context) = 0;

    virtual antlrcpp::Any visitConstExp(SysYParser::ConstExpContext *context) = 0;

    virtual antlrcpp::Any visitIntConst(SysYParser::IntConstContext *context) = 0;

    virtual antlrcpp::Any visitFloatConst(SysYParser::FloatConstContext *context) = 0;


};

