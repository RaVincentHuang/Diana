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

// CompUnitNode
// CompUnitNode::CompUnitNode( std::vector<FuncDefNode*> _funcDef, 
//                             std::vector<DeclNode*> _decl) 
//     : AstNode(AST_compUnit), funcDef(std::move(_funcDef)), decl(std::move(_decl)) 
// {
//     SET_IT_PARENT_VEC(funcDef);
//     SET_IT_PARENT_VEC(decl);
// }
// CompUnitNode::CompUnitNode( std::vector<FuncDefNode*> _funcDef, 
//                 std::vector<DeclNode*> _decl, std::vector<AstNode*> _element)
//     : AstNode(AST_compUnit), funcDef(std::move(_funcDef)), decl(std::move(_decl)), element(std::move(_element))
// {
//     SET_IT_PARENT_VEC(funcDef);
//     SET_IT_PARENT_VEC(decl);
//     SET_IT_PARENT_VEC(element);
// }

CompUnitNode::CompUnitNode(std::vector<AstNode*> _element)
    : AstNode(AST_compUnit), element(std::move(_element))
{
    SET_IT_PARENT_VEC(element);
}

// FIXME  heap space memory release
CompUnitNode::~CompUnitNode() 
{
    // FREE_VEC(funcDef);
    // FREE_VEC(decl);
}

std::vector<PrototypeNode*>& CompUnitNode::getPrototype() 
{

}

// FuncDefNode
FuncDefNode::FuncDefNode(AstType _funcType, std::string _funcName,
                            FuncParamListNode* _funcParamList, BlockNode* _block) 
    : AstNode(AST_funcDef), funcType(_funcType), funcName(_funcName), block(_block), funcParamList(_funcParamList) 
{
    SET_IT_PARENT(funcParamList);
    SET_IT_PARENT(block);
}

// FIXME  heap space memory release
FuncDefNode::~FuncDefNode() {}


// FuncParamListNode
FuncParamListNode::FuncParamListNode( std::vector<FuncParamNode*> _funcParam) 
    : AstNode(AST_funcParams), funcParam(std::move(_funcParam))           
{
    SET_IT_PARENT_VEC(funcParam);
}

// FIXME  heap space memory release
FuncParamListNode::~FuncParamListNode() {}

// FuncParamNode
FuncParamNode::FuncParamNode( AstType _type, std::string _varName, 
                    std::vector<int> _dimSize) 
    : AstNode(AST_funcParam), type(_type), varName(_varName), dimSize(std::move(_dimSize)) {}

FuncParamNode::~FuncParamNode() {}

// ConstDeclNode
ConstDeclNode::ConstDeclNode( AstType _type,
                                std::vector<ConstDefNode*> _constDef)
    : DeclNode(AST_constDecl, true, _type), constDef(std::move(_constDef))
{
    SET_IT_PARENT_VEC(constDef);
}

// FIXME  heap space memory release
ConstDeclNode::~ConstDeclNode() {}


// VarDeclNode
VarDeclNode::VarDeclNode( AstType _type, std::vector<VarDefNode*> _varDef) 
    : DeclNode(AST_varDecl, false, _type), varDef(std::move(_varDef))
{
    SET_IT_PARENT_VEC(varDef);
}

// FIXME  heap space memory release
VarDeclNode::~VarDeclNode() {}


// ConstDefNode
ConstDefNode::ConstDefNode( std::vector<int> _dimSize, 
                            InitValNode* _constInitVal, std::string _name) 
    : AstNode(AST_constDef), dimSize(std::move(_dimSize)), constInitVal(_constInitVal), name(_name)
{
    SET_IT_PARENT(constInitVal);
}

ConstDefNode::~ConstDefNode() {}


// VarDefNode
VarDefNode::VarDefNode( std::vector<int> _dimSize, 
                        InitValNode* _varInitVal, std::string _name) 
    : AstNode(AST_varDef), dimSize(std::move(_dimSize)), varInitVal(_varInitVal), name(_name)
{
    SET_IT_PARENT(varInitVal);
}

VarDefNode::VarDefNode( std::vector<int> _dimSize,  std::string _name) 
    : AstNode(AST_varDef), dimSize(std::move(_dimSize)), varInitVal(nullptr), name(_name) {}

// FIXME  heap space memory release
VarDefNode::~VarDefNode() {}

// InitValNode
InitValNode::InitValNode( std::vector<ExprNode*> _initItem) 
    : AstNode(AST_initVal), initItem(std::move(_initItem))
{
    SET_IT_PARENT_VEC(initItem);
}

// FIXME  heap space memory release
InitValNode::~InitValNode() {}

// StmtNode
StmtNode::StmtNode(AstKind _kind)
    : AstNode(_kind) {}

// AssignStmtNode
AssignStmtNode::AssignStmtNode( LvalNode* _lval, ExprNode* _expr) 
    : StmtNode(AST_assignStmt), lval(_lval), expr(_expr)
{
    SET_IT_PARENT(lval);
    SET_IT_PARENT(expr);
}

// FIXME  heap space memory release    
AssignStmtNode::~AssignStmtNode() {}

// ExprStmtNode
ExprStmtNode::ExprStmtNode( ExprNode* _expr) 
    : StmtNode(AST_expStmt), expr(_expr)
{
    SET_IT_PARENT(expr);
}

// FIXME  heap space memory release
ExprStmtNode::~ExprStmtNode() {}

// IfStmtNode
IfStmtNode::IfStmtNode( ExprNode* _cond, StmtNode* _thenBlock, StmtNode* _elseBlock) 
    : StmtNode(AST_ifStmt), cond(_cond), thenBlock(_thenBlock), elseBlock(_elseBlock)
{
    SET_IT_PARENT(cond);
    SET_IT_PARENT(thenBlock);
    SET_IT_PARENT(elseBlock);
}

// FIXME  heap space memory release
IfStmtNode::~IfStmtNode() {}

// ForStmtNode
ForStmtNode::ForStmtNode( StmtNode* _initial, ExprNode* _cond,
                            StmtNode* _increment, StmtNode* _stmt) 
    : StmtNode(AST_forStmt), initial(_initial), increment(_increment), 
        cond(_cond),  stmt(_stmt)
{
    SET_IT_PARENT(initial);
    SET_IT_PARENT(increment);
    SET_IT_PARENT(cond);
    SET_IT_PARENT(stmt);
}

// FIXME  heap space memory release
ForStmtNode::~ForStmtNode() {}

// WhileStmtNode
WhileStmtNode::WhileStmtNode( ExprNode* _cond, StmtNode* _stmt)
    : StmtNode(AST_whileStmt), cond(_cond), stmt(_stmt)
{
    SET_IT_PARENT(cond);
    SET_IT_PARENT(stmt);
}

// FIXME  heap space memory release
WhileStmtNode::~WhileStmtNode() {}

// BreakStmtNode
BreakStmtNode::BreakStmtNode( bool _is_break) 
    : StmtNode(AST_breakStmt), is_break(_is_break) {}

// FIXME  heap space memory release    
BreakStmtNode::~BreakStmtNode() {}

// ContinueStmtNode
ContinueStmtNode::ContinueStmtNode( bool _is_contiune) 
    : StmtNode(AST_continueStmt), is_contiune(_is_contiune) {}

// FIXME  heap space memory release
ContinueStmtNode::~ContinueStmtNode() {}

// ReturnStmtNode
ReturnStmtNode::ReturnStmtNode( ExprNode* _expr) 
    : StmtNode(AST_returnStmt), expr(_expr), has_ret_val(true) { SET_IT_PARENT(expr); }

ReturnStmtNode::ReturnStmtNode() 
    : StmtNode(AST_returnStmt), expr(nullptr), has_ret_val(false) {}

ReturnStmtNode::~ReturnStmtNode() {}

// PrototypeNode
PrototypeNode::PrototypeNode( bool _is_const, AstType _type,
                                std::string _funcName, FuncParamListNode* _funcParamList) 
    : DeclNode(AST_prototype, _is_const, _type), funcName(_funcName), funcParamList(_funcParamList)
{
    SET_IT_PARENT(funcParamList);
}

// FIXME  heap space memory release
PrototypeNode::~PrototypeNode() {}

// BlockNode
BlockNode::BlockNode( std::vector<StmtNode*> _blockItems) 
    : StmtNode(AST_block), blockItems(std::move(_blockItems))
{
    SET_IT_PARENT_VEC(blockItems);
}

// FIXME  heap space memory release
BlockNode::~BlockNode() {}

// ExprNode
ExprNode::ExprNode(AstKind _kind,  AstType _type)
    : AstNode(_kind), type(_type) {}

// FIXME  heap space memory release
ExprNode::~ExprNode() {}

// // CondExprNode
// CondExprNode::CondExprNode( AstType _type,
//                             LOrExprNode* _lOrExpr)
//     : ExprNode(AST_cond, _type), lOrExpr(_lOrExpr)
// {
//     SET_IT_PARENT(lOrExpr);
// }

// CondExprNode::~CondExprNode() {}

// // LOrExprNode
// LOrExprNode::LOrExprNode( AstType _type,
//                             LAndExprNode* _lAndExpr)
//     : ExprNode(AST_lOrExp, _type), lAndExpr(_lAndExpr), lOrExpr(nullptr)
// {
//     SET_IT_PARENT(lAndExpr);
// }

// LOrExprNode::LOrExprNode( AstType _type,
//                             LAndExprNode* _lAndExpr, LOrExprNode* _lOrExpr)
//     : ExprNode(AST_lOrExp, _type), lAndExpr(_lAndExpr), lOrExpr(_lOrExpr)
// {
//     SET_IT_PARENT(lAndExpr);
//     SET_IT_PARENT(lOrExpr);
// }

// LOrExprNode::~LOrExprNode() {}

// // LAndExprNode
// LAndExprNode::LAndExprNode( AstType _type,
//                             EqExprNode* _eqExpr)
//     : ExprNode(AST_lAndExp, _type), eqExpr(_eqExpr), lAndExpr(nullptr)
// {
//     SET_IT_PARENT(eqExpr);
// }

// LAndExprNode::LAndExprNode( AstType _type,
//                             EqExprNode* _eqExpr, LAndExprNode* _lAndExpr)
//     : ExprNode(AST_lAndExp, _type), eqExpr(_eqExpr), lAndExpr(_lAndExpr)
// {
//     SET_IT_PARENT(eqExpr);
//     SET_IT_PARENT(lAndExpr);
// }

// LAndExprNode::~LAndExprNode() {}

// // EqExprNode
// EqExprNode::EqExprNode( AstType _type,
//                         RelExprNode* _relExpr)
//     : ExprNode(AST_eqExp, _type), relExpr(_relExpr), eqExpr(nullptr), op(ROP_NONE)
// {
//     SET_IT_PARENT(relExpr);
// }

// EqExprNode::EqExprNode( AstType _type,
//                         RelExprNode* _relExpr, EqExprNode* _eqExpr, RelationOp _op)
//     : ExprNode(AST_eqExp, _type), relExpr(_relExpr), eqExpr(_eqExpr), op(_op) 
// {
//     SET_IT_PARENT(relExpr);
//     SET_IT_PARENT(eqExpr);
// }

// EqExprNode::~EqExprNode() {}

// // RelExprNode
// RelExprNode::RelExprNode( AstType _type,
//                             AddExprNode* _addExpr)
//     : ExprNode(AST_relExp, _type), addExpr(_addExpr), relExpr(nullptr), op(ROP_NONE)
// {
//     SET_IT_PARENT(addExpr);
// }

// RelExprNode::RelExprNode( AstType _type,
//                             AddExprNode* _addExpr, RelExprNode* _relExpr, RelationOp _op)
//     : ExprNode(AST_relExp, _type), addExpr(_addExpr), relExpr(_relExpr), op(_op)
// {
//     SET_IT_PARENT(addExpr);
//     SET_IT_PARENT(relExpr);
// }

// RelExprNode::~RelExprNode() {}

// // AddExprNode
// AddExprNode::AddExprNode( AstType _type,
//                             MulExprNode* _mulExpr, AddExprNode* _addExpr, BinaryOp _op)
//     : ExprNode(AST_addExp, _type), mulExpr(_mulExpr), addExpr(_addExpr), op(_op)
// {
//     SET_IT_PARENT(mulExpr);
//     SET_IT_PARENT(addExpr);
// }

// AddExprNode::AddExprNode( AstType _type,
//                             MulExprNode* _mulExpr)
//     : ExprNode(AST_addExp, _type), mulExpr(_mulExpr), addExpr(nullptr), op(BOP_NONE)
// {
//     SET_IT_PARENT(mulExpr);
// }

// AddExprNode::~AddExprNode() {}

// // MulExprNode
// MulExprNode::MulExprNode( AstType _type,
//                             UnaryExprNode* _unaryExpr, MulExprNode* _mulExpr, BinaryOp _op)
//     : ExprNode(AST_mulExp, _type), unaryExpr(_unaryExpr), mulExpr(_mulExpr), op(op)
// {
//     SET_IT_PARENT(unaryExpr);
//     SET_IT_PARENT(mulExpr);
// }

// MulExprNode::MulExprNode( AstType _type,
//                             UnaryExprNode* _unaryExpr)
//     : ExprNode(AST_mulExp, _type), unaryExpr(_unaryExpr), mulExpr(nullptr), op(BOP_NONE)
// {
//     SET_IT_PARENT(unaryExpr);
// }

// MulExprNode::~MulExprNode() {}

// // PrimaryExprNode
// PrimaryExprNode::PrimaryExprNode( AstType _type,
//                                     ExprNode* _expr)
//     : ExprNode(AST_primaryExp, _type), expr(_expr), lval(nullptr)
// {
//     SET_IT_PARENT(expr);
// }
// PrimaryExprNode::PrimaryExprNode( AstType _type,
//                                     LvalNode* _lval)
//     : ExprNode(AST_primaryExp, _type), expr(nullptr), lval(_lval)
// {
//     SET_IT_PARENT(lval);
// }

// PrimaryExprNode::PrimaryExprNode( AstType _type)
//     : ExprNode(AST_primaryExp, _type),
//     expr(nullptr), lval(nullptr) {}

// PrimaryExprNode::~PrimaryExprNode() {}

// UnaryExprNode
UnaryExprNode::UnaryExprNode( AstType _type,
                                UnaryOp _op, ExprNode* _expr)
    : ExprNode(AST_unaryExp, _type),
    op(_op), expr(_expr)
{
    SET_IT_PARENT(_expr);
}

// FIXME  heap space memory release
UnaryExprNode::~UnaryExprNode() {}

// BinaryExprNode
BinaryExprNode::BinaryExprNode( AstType _type,
                    BinaryOp _op, ExprNode* _lhs, ExprNode* _rhs)
    : ExprNode(AST_binaryExp, _type),
    op(_op), lhs(_lhs), rhs(_rhs)
{
    SET_IT_PARENT(lhs);
    SET_IT_PARENT(rhs);
}

// FIXME  heap space memory release 
BinaryExprNode::~BinaryExprNode() {}

// // ConditionExprNode
// ConditionExprNode::ConditionExprNode( AstType _type)
//     : ExprNode(AST_ConditionExp, _type), shortCircuit(false) {}
    
// ConditionExprNode::~ConditionExprNode() {}

// // ConditionUnaryExprNode
// ConditionUnaryExprNode::ConditionUnaryExprNode( AstType _type, 
//                             UnaryOp _op, ConditionExprNode* _conditionExpr)
//     : ConditionExprNode(_loc, _type), op(_op), conditionExpr(_conditionExpr)
// {
//     SET_IT_PARENT(conditionExpr);
// }

// ConditionUnaryExprNode::~ConditionUnaryExprNode() {}


// // ConditionBinaryExprNode
// ConditionBinaryExprNode::ConditionBinaryExprNode( AstType _type, 
//                                 BinaryOp _op, ConditionExprNode* _lhs, ConditionExprNode* _rhs)
//     : ConditionExprNode(_loc, _type), op(_op), lhs(_lhs), rhs(_rhs)
// {
//     SET_IT_PARENT(lhs);
//     SET_IT_PARENT(rhs);
// }

// ConditionBinaryExprNode::~ConditionBinaryExprNode() {}

// FunctionCallExprNode
FunctionCallExprNode::FunctionCallExprNode( AstType _type,
                            std::string _funcName, std::vector<ExprNode*> _funcParamList)
    : ExprNode(AST_unaryExp, _type),
    funcName(_funcName), funcParamList(std::move(_funcParamList)) {}

// FIXME  heap space memory release
FunctionCallExprNode::~FunctionCallExprNode() {}

// LvalNode
LvalNode::LvalNode( AstType _type,
                    std::string _variable,  std::vector<ExprNode*> _indexExpr)
    : ExprNode(AST_lVal, _type), variable(_variable), indexExpr(std::move(_indexExpr)) {}

LvalNode::LvalNode( AstType _type,
                std::string _variable, std::vector<ExprNode*> _indexExpr, std::vector<int> _dimSize)
    : ExprNode(AST_lVal, _type), variable(_variable), indexExpr(std::move(_indexExpr)),
    dimSize(std::move(_dimSize)) {}

// FIXME  heap space memory release
LvalNode::~LvalNode() {}

// // ConditionNumberNode
// ConditionNumberNode::ConditionNumberNode( AstType _type,
//                             Immediate _immediate)
//     : ConditionExprNode(_loc, _type), immediate(_immediate) {}

// ConditionNumberNode::~ConditionNumberNode() {}

// NumberNode
NumberNode::NumberNode( AstType _type,
                        Immediate _immediate)
    : ExprNode(AST_Number, _type), immediate(_immediate) {}

// FIXME  heap space memory release
NumberNode::~NumberNode() {}

} // namespace ast
} // namespace sysy