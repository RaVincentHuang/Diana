#include "sysy/Frontend/Ast.h"
#include "sysy/Support/common.h"

namespace sysy
{
namespace ast
{
// Basic

// To the higher
AstType operator&&(AstType x, AstType y)
{
    return x > y ? x : y;
}

// To the lower
AstType operator||(AstType x, AstType y)
{
    return x < y ? x : y;
}

FuncDefNode* AstNode::getParentFuction() const {}

CompUnitNode::CompUnitNode(std::vector<std::unique_ptr<AstNode>> _element)
    : AstNode(AST_compUnit), element(std::move(_element))
{
    SET_IT_PARENT_VEC(element);
}

CompUnitNode::~CompUnitNode() 
{
    FREE_VEC(element);
}

// std::vector<PrototypeNode*>& CompUnitNode::getPrototype() 
// {

// }

// FuncDefNode
FuncDefNode::FuncDefNode(AstType _funcType, std::string _funcName,
                            std::unique_ptr<FuncParamListNode> _funcParamList, 
                            std::unique_ptr<BlockNode> _block) 
    : AstNode(AST_funcDef), funcType(_funcType), funcName(_funcName), block(std::move(_block)), funcParamList(std::move(_funcParamList)) 
{
    SET_IT_PARENT(funcParamList);
    SET_IT_PARENT(block);
}

FuncDefNode::~FuncDefNode() 
{
    funcParamList.reset();
    block.reset();
}


// FuncParamListNode
FuncParamListNode::FuncParamListNode(std::vector<std::unique_ptr<FuncParamNode>> _funcParam) 
    : AstNode(AST_funcParams), funcParam(std::move(_funcParam))           
{
    SET_IT_PARENT_VEC(funcParam);
}

FuncParamListNode::~FuncParamListNode() 
{
    FREE_VEC(funcParam);
}

// FuncParamNode
FuncParamNode::FuncParamNode( AstType _type, std::string _varName, 
                    std::vector<int> _dimSize) 
    : AstNode(AST_funcParam), type(_type), varName(_varName), dimSize(std::move(_dimSize)) {}

FuncParamNode::~FuncParamNode() {}

// ConstDeclNode
ConstDeclNode::ConstDeclNode(AstType _type,
                                std::vector<std::unique_ptr<ConstDefNode>> _constDef)
    : DeclNode(AST_constDecl, true, _type), constDef(std::move(_constDef))
{
    SET_IT_PARENT_VEC(constDef);
}

ConstDeclNode::~ConstDeclNode() 
{
    FREE_VEC(constDef);
}


// VarDeclNode
VarDeclNode::VarDeclNode( AstType _type, std::vector<std::unique_ptr<VarDefNode>> _varDef) 
    : DeclNode(AST_varDecl, false, _type), varDef(std::move(_varDef))
{
    SET_IT_PARENT_VEC(varDef);
}

VarDeclNode::~VarDeclNode() 
{
    FREE_VEC(varDef);
}


// ConstDefNode
ConstDefNode::ConstDefNode( std::vector<int> _dimSize, 
                            std::unique_ptr<InitValNode> _constInitVal, std::string _name) 
    : AstNode(AST_constDef), dimSize(std::move(_dimSize)), constInitVal(std::move(_constInitVal)), name(_name)
{
    SET_IT_PARENT(constInitVal);
}

ConstDefNode::~ConstDefNode() 
{
    constInitVal.reset();
}


// VarDefNode
VarDefNode::VarDefNode( std::vector<int> _dimSize, 
                        std::unique_ptr<InitValNode> _varInitVal, std::string _name) 
    : AstNode(AST_varDef), dimSize(std::move(_dimSize)), varInitVal(std::move(_varInitVal)), name(_name)
{
    SET_IT_PARENT(varInitVal);
}

VarDefNode::VarDefNode( std::vector<int> _dimSize,  std::string _name) 
    : AstNode(AST_varDef), dimSize(std::move(_dimSize)), varInitVal(nullptr), name(_name) {}

VarDefNode::~VarDefNode() 
{
    varInitVal.reset();
}

// InitValNode
InitValNode::InitValNode(std::vector<std::unique_ptr<ExprNode>> _initItem) 
    : AstNode(AST_initVal), initItem(std::move(_initItem))
{
    SET_IT_PARENT_VEC(initItem);
}

InitValNode::~InitValNode() 
{
    FREE_VEC(initItem);
}

// StmtNode
StmtNode::StmtNode(AstKind _kind)
    : AstNode(_kind) {}

// AssignStmtNode
AssignStmtNode::AssignStmtNode(std::unique_ptr<LvalNode> _lval, 
                                std::unique_ptr<ExprNode> _expr) 
    : StmtNode(AST_assignStmt), lval(std::move(_lval)), expr(std::move(_expr))
{
    SET_IT_PARENT(lval);
    SET_IT_PARENT(expr);
}

AssignStmtNode::~AssignStmtNode() 
{
    lval.reset();
    expr.reset();
}

// ExprStmtNode
ExprStmtNode::ExprStmtNode(std::unique_ptr<ExprNode> _expr) 
    : StmtNode(AST_expStmt), expr(std::move(_expr))
{
    SET_IT_PARENT(expr);
}

ExprStmtNode::~ExprStmtNode() 
{
    expr.reset();
}

// IfStmtNode
IfStmtNode::IfStmtNode(std::unique_ptr<ExprNode> _cond, 
                        std::unique_ptr<StmtNode> _thenBlock, 
                        std::unique_ptr<StmtNode> _elseBlock) 
    : StmtNode(AST_ifStmt), cond(std::move(_cond)), 
        thenBlock(std::move(_thenBlock)), elseBlock(std::move(_elseBlock))
{
    SET_IT_PARENT(cond);
    SET_IT_PARENT(thenBlock);
    SET_IT_PARENT(elseBlock);
}

IfStmtNode::~IfStmtNode() 
{
    cond.reset();
    thenBlock.reset();
    elseBlock.reset();
}

// ForStmtNode
ForStmtNode::ForStmtNode(std::unique_ptr<StmtNode> _initial, 
                            std::unique_ptr<ExprNode> _cond,
                            std::unique_ptr<StmtNode> _increment, 
                            std::unique_ptr<StmtNode> _stmt) 
    : StmtNode(AST_forStmt), initial(std::move(_initial)), increment(std::move(_increment)), 
        cond(std::move(_cond)),  stmt(std::move(_stmt))
{
    SET_IT_PARENT(initial);
    SET_IT_PARENT(increment);
    SET_IT_PARENT(cond);
    SET_IT_PARENT(stmt);
}

ForStmtNode::~ForStmtNode() 
{
    initial.reset();
    increment.reset();
    stmt.reset();
    cond.reset();
}

// WhileStmtNode
WhileStmtNode::WhileStmtNode(std::unique_ptr<ExprNode> _cond, std::unique_ptr<StmtNode> _stmt)
    : StmtNode(AST_whileStmt), cond(std::move(_cond)), stmt(std::move(_stmt))
{
    SET_IT_PARENT(cond);
    SET_IT_PARENT(stmt);
}

WhileStmtNode::~WhileStmtNode() 
{
    cond.reset();
    stmt.reset();
}

// BreakStmtNode
BreakStmtNode::BreakStmtNode( bool _is_break) 
    : StmtNode(AST_breakStmt), is_break(_is_break) {}

BreakStmtNode::~BreakStmtNode() {}

// ContinueStmtNode
ContinueStmtNode::ContinueStmtNode( bool _is_contiune) 
    : StmtNode(AST_continueStmt), is_contiune(_is_contiune) {}

ContinueStmtNode::~ContinueStmtNode() {}

// ReturnStmtNode
ReturnStmtNode::ReturnStmtNode(std::unique_ptr<ExprNode> _expr) 
    : StmtNode(AST_returnStmt), expr(std::move(_expr)), has_ret_val(true) 
{ 
    SET_IT_PARENT(expr); 
}

ReturnStmtNode::ReturnStmtNode() 
    : StmtNode(AST_returnStmt), expr(nullptr), has_ret_val(false) {}

ReturnStmtNode::~ReturnStmtNode() 
{
    expr.reset();
}

// TODO add the PrototypeNode
// PrototypeNode::PrototypeNode( bool _is_const, AstType _type,
//                                 std::string _funcName, FuncParamListNode* _funcParamList) 
//     : DeclNode(AST_prototype, _is_const, _type), funcName(_funcName), funcParamList(_funcParamList)
// {
//     SET_IT_PARENT(funcParamList);
// }

// // FIXME  heap space memory release
// PrototypeNode::~PrototypeNode() {}

// BlockNode
BlockNode::BlockNode(std::vector<std::unique_ptr<StmtNode>> _blockItems) 
    : StmtNode(AST_block), blockItems(std::move(_blockItems))
{
    SET_IT_PARENT_VEC(blockItems);
}

BlockNode::~BlockNode() 
{
    FREE_VEC(blockItems);
}

// ExprNode
ExprNode::ExprNode(AstKind _kind,  AstType _type)
    : AstNode(_kind), type(_type) {}

ExprNode::~ExprNode() {}

// UnaryExprNode
UnaryExprNode::UnaryExprNode( AstType _type, UnaryOp _op, 
                                std::unique_ptr<ExprNode> _expr)
    : ExprNode(AST_unaryExp, _type),
    op(_op), expr(std::move(_expr))
{
    SET_IT_PARENT(_expr);
}

UnaryExprNode::~UnaryExprNode() 
{
    expr.reset();
}

// BinaryExprNode
BinaryExprNode::BinaryExprNode( AstType _type, BinaryOp _op, 
                                std::unique_ptr<ExprNode> _lhs, 
                                std::unique_ptr<ExprNode> _rhs)
    : ExprNode(AST_binaryExp, _type),
    op(_op), lhs(std::move(_lhs)), rhs(std::move(_rhs))
{
    SET_IT_PARENT(lhs);
    SET_IT_PARENT(rhs);
}

BinaryExprNode::~BinaryExprNode() 
{
    lhs.reset();
    rhs.reset();
}

// FunctionCallExprNode
FunctionCallExprNode::FunctionCallExprNode( AstType _type, std::string _funcName, 
                            std::vector<std::unique_ptr<ExprNode>> _funcParamList)
    : ExprNode(AST_unaryExp, _type),
    funcName(_funcName), funcParamList(std::move(_funcParamList)) {}

FunctionCallExprNode::~FunctionCallExprNode() 
{
    FREE_VEC(funcParamList);
}

// LvalNode
LvalNode::LvalNode( AstType _type,
                    std::string _variable,  std::vector<std::unique_ptr<ExprNode>> _indexExpr)
    : ExprNode(AST_lVal, _type), variable(_variable), indexExpr(std::move(_indexExpr)) {}

LvalNode::LvalNode( AstType _type,
                std::string _variable, std::vector<std::unique_ptr<ExprNode>> _indexExpr, std::vector<int> _dimSize)
    : ExprNode(AST_lVal, _type), variable(_variable), indexExpr(std::move(_indexExpr)),
    dimSize(std::move(_dimSize)) {}

LvalNode::~LvalNode() 
{
    FREE_VEC(indexExpr);
}


// NumberNode
NumberNode::NumberNode( AstType _type,
                        Immediate _immediate)
    : ExprNode(AST_Number, _type), immediate(_immediate) {}

NumberNode::~NumberNode() {}

} // namespace ast
} // namespace sysy