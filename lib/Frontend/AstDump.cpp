#include "sysy/Frontend/AstDump.h"
#include "sysy/Frontend/AstType.h"
#include "sysy/Support/StrUtils.h"
#include <iostream>

namespace sysy
{
namespace ast
{

std::string getSignature(AstType type)
{
    switch(type)
    {
    case VOID:
        return "void";
    case INT:
        return "int";
    case FLOAT:
        return "float";
    default:
        return "unknow";
    }
}

std::string getSignature(BinaryOp op)
{
    switch (op)
    {
    case BOP_ADD:
        return "+";
    case BOP_SUB:
        return "-";
    case BOP_MUL:
        return "*";
    case BOP_DIV:
        return "/";
    case BOP_MOD:
        return "%";
    case ROP_EQL:
        return "==";
    case ROP_NEQ:
        return "!=";
    case ROP_LES:
        return "<";
    case ROP_GRT:
        return ">";
    case ROP_LEQ:
        return "<=";
    case ROP_GEQ:
        return ">=";
    case ROP_OR:
        return "||";
    case ROP_AND:
        return "&&";
    default:
        return "unknow";
    }
}

std::string getSignature(UnaryOp op)
{
    switch(op)
    {
    case UOP_ADD:
        return "+";
    case UOP_SUB:
        return "-";
    case UOP_NOT:
        return "!";
    default:
        return "unknow";
    }
}

std::string CompUnitNode::getSignature() const 
{
    std::string signature = "compUnit: ";
    for(auto& iter : element)
        signature += subLine(iter->getSignature());
    // for(auto iter : funcDef)
    //     signature += subLine(iter->getSignature());
    // for(auto iter : decl)
    //     signature += subLine(iter->getSignature());
    return signature;
}

std::string FuncDefNode::getSignature() const 
{
    std::string signature = "function: ";
    signature += ast::getSignature(funcType) + " " + funcName;
    if(funcParamList)
        signature += subLine(funcParamList->getSignature());
    signature += subLine(block->getSignature());
    return signature;
}

std::string FuncParamListNode::getSignature() const 
{
    std::string signature = "funcParams: ";
    for(auto& iter : funcParam)
        signature += subLine(iter->getSignature());
    return signature;
    
}

std::string FuncParamNode::getSignature() const 
{
    std::string signature = "parameter: " + ast::getSignature(type) + " " + varName;
    for(auto iter : dimSize)
        signature += "[" + std::to_string(iter) + "]";
    return signature;
}

std::string DeclNode::getSignature() const 
{
    std::string signature = "decl: " + ast::getSignature(type);
    return signature;
}

std::string ConstDeclNode::getSignature() const 
{
    std::string signature = "constDecl: " + ast::getSignature(type);
    for(auto& iter : constDef)
        signature += subLine(iter->getSignature());
    return signature;
}

std::string VarDeclNode::getSignature() const 
{
    std::string signature = "constDecl: " + ast::getSignature(type);
    for(auto& iter : varDef)
        signature += subLine(iter->getSignature());
    return signature;
}

std::string ConstDefNode::getSignature() const 
{
    std::string signature = "constDef: " + name;
    for(auto iter : dimSize)
        signature += "[" + std::to_string(iter) + "]";
    if(constInitVal)
        signature += subLine(constInitVal->getSignature());
    return signature;
}

std::string VarDefNode::getSignature() const 
{
    std::string signature = "varDef: " + name;
    for(auto iter : dimSize)
        signature += "[" + std::to_string(iter) + "]";
    if(varInitVal)
        signature += subLine(varInitVal->getSignature());
    return signature;
}

std::string InitValNode::getSignature() const 
{
    std::string signature = "initVal: ";
    for(auto& iter : initItem)
        signature += subLine(iter->getSignature());
    return signature;
}

std::string StmtNode::getSignature() const 
{
    std::string signature = "stmt: ";
    return signature;
}

std::string AssignStmtNode::getSignature() const 
{
    std::string signature = "assignStmt: ";
    signature += subLine(lval->getSignature());
    signature += subLine(expr->getSignature());
    return signature;
}

std::string ExprStmtNode::getSignature() const 
{
    std::string signature = "exprStmt: ";
    signature += subLine(expr->getSignature());
    return signature;
}

std::string IfStmtNode::getSignature() const 
{
    std::string signature = "ifStmt: ";
    signature += subLine("if: " + cond->getSignature());
    if(thenBlock)
        signature += subLine("then: " + thenBlock->getSignature());
    if(elseBlock)
        signature += subLine("else: " + elseBlock->getSignature());
    return signature;
}

std::string ForStmtNode::getSignature() const 
{
    std::string signature = "forStmt: ";
    signature += subLine("init: " + initial->getSignature());
    signature += subLine("cond: " + cond->getSignature());
    signature += subLine("incre: " + increment->getSignature());
    signature += subLine("stmt: " + stmt->getSignature());
    return signature;
}

std::string WhileStmtNode::getSignature() const 
{
    std::string signature = "whileStmt: ";
    signature += subLine("cond: " + cond->getSignature());
    if(stmt)
        signature += subLine("stmt: " + stmt->getSignature());
    return signature;
}

std::string BreakStmtNode::getSignature() const 
{
    std::string signature = "breakStmt";
    return signature;
}
std::string ContinueStmtNode::getSignature() const 
{
    std::string signature = "continueStmt";
    return signature;
}

std::string ReturnStmtNode::getSignature() const 
{
    std::string signature = "returnStmt: ";
    if(expr)
        signature += subLine(expr->getSignature());
    return signature;
}

// std::string PrototypeNode::getSignature() const 
// {
//     return "";
// }

std::string BlockNode::getSignature() const 
{
    std::string signature = "blockStmt: ";
    for(auto& iter : blockItems)
        signature += subLine(iter->getSignature());
    return signature;
}

std::string ExprNode::getSignature() const 
{
    std::string signature = "Expr: " + ast::getSignature(type);
    return signature;
}

std::string UnaryExprNode::getSignature() const 
{
    std::string signature = "Expr: <" + ast::getSignature(type) + "> " + ast::getSignature(op);
    signature += subLine(expr->getSignature());
    return signature;
}

std::string BinaryExprNode::getSignature() const 
{
    std::string signature = "Expr: <" + ast::getSignature(type) + "> " + ast::getSignature(op);
    signature += subLine(lhs->getSignature());
    signature += subLine(rhs->getSignature());
    return signature;
}

std::string FunctionCallExprNode::getSignature() const 
{
    std::string signature = "functionCall: <" + ast::getSignature(type) + "> " + funcName + " (";
    if(!this->format.empty())
        signature += subLine(format);
    for(auto& iter : funcParamList)
        signature += subLine(iter->getSignature());
    signature += ")";
    return signature;
}

std::string LvalNode::getSignature() const 
{
    std::string signature = "lval: <" + ast::getSignature(type) + "> " + variable;
    // for(auto iter : dimSize)
    //     signature += "[" + std::to_string(iter) + "]";
    for(auto& iter : indexExpr)
        signature += subLine("[" + iter->getSignature() + "]");
    return signature;
}

std::string NumberNode::getSignature() const 
{
    std::string signature = "Number: <" + ast::getSignature(type)
                            + std::string("> = ") + immediate.toString();
    return signature;
}

void dump(AstNode* node)
{
    auto str = node->getSignature();
    std::cout << "==================================AST====================================" << std::endl;
    std::cout << str << std::endl;
    std::cout << "=========================================================================" << std::endl;
}

} // namespace ast
} // namespace sysy