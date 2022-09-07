//===-------------------- Ast.h - Ast node declare  ---*- C++ -*-----------===//
//
// Part of the Diana, a SysY Compiler, under the MIT License.
// See https://github.com/RaVincentHuang/Diana/blob/master/LICENSE for license 
// information.
// SPDX-License-Identifier: MIT License.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
//===----------------------------------------------------------------------===//

#ifndef SYSY_AST_H
#define SYSY_AST_H
#include "sysy/Frontend/AstType.h"
#include "sysy/Frontend/SysYOp.h"
#include <vector>
#include <memory>


namespace sysy 
{

namespace ast
{

// Subclass of the AstNode
class FuncDefNode;
class FuncParamListNode;
class FuncParamNode;
class BlockNode;

class StmtNode;
class DeclNode;
class ConstDefNode;
class VarDefNode;
class InitValNode;
class LvalNode;

// class PrototypeNode;// TODO add the PrototypeNode


class ExprNode;
class CondExprNode;
class UnaryExprNode;
class PrimaryExprNode;

class AstNode
{
public:
    enum AstKind
    {
        AST_compUnit,
        AST_decl,
        AST_constDecl,
        AST_constDef,
        AST_varDecl,
        AST_varDef,
        AST_prototype,
        AST_initVal,
        AST_funcDef,
        AST_funcParams,
        AST_funcParam,
        AST_stmt,
        AST_block,
        AST_assignStmt,
        AST_expStmt,
        AST_ifStmt,
        AST_forStmt,
        AST_whileStmt,
        AST_breakStmt,
        AST_continueStmt,
        AST_returnStmt,
        AST_exp,
        AST_cond,
        AST_mulExp,
        AST_addExp,
        AST_relExp,
        AST_eqExp,
        AST_lAndExp,
        AST_lOrExp,
        AST_unaryExp,
        AST_binaryExp,
        AST_functCallExp,
        AST_primaryExp,
        AST_lVal,
        AST_ConditionNumber,
        AST_Number,
        AST_EOF,
        AST_Undefined
    };
public:
    AstKind kind;
    AstNode* parent; // Dangerous: maybe there is memory leak.
    std::vector<AstNode*> children;

    AstNode(AstKind _kind)
        : kind(_kind)
        , parent(nullptr) {}
    
    virtual ~AstNode() {}

    void setParent(AstNode* _parent) { parent = _parent; }
    void addChildren(AstNode* _children) { children.push_back(_children); }

    std::vector<AstNode*>& getChildren() { return children; }
    const AstNode* getParent() const { return parent; }
    AstNode* getParent() { return parent; }
    const AstKind& getKind() const { return kind; };

    FuncDefNode* getParentFuction() const;

    virtual std::string getSignature() const = 0;
}; // class ASTNode

class CompUnitNode : public AstNode
{
public:
    std::vector<std::unique_ptr<AstNode>> element;
    
    CompUnitNode(std::vector<std::unique_ptr<AstNode>> _element);

    ~CompUnitNode();

    std::string getSignature() const override;

    // std::vector<PrototypeNode*>& getPrototype();

    std::string getStr() const { return "compUnit"; }

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_compUnit; }
};

class FuncDefNode : public AstNode
{
public:
    AstType funcType;
    std::string funcName;
    std::unique_ptr<FuncParamListNode> funcParamList;
    std::unique_ptr<BlockNode> block;

    FuncDefNode(AstType _funcType, std::string _funcName,
                std::unique_ptr<FuncParamListNode> _funcParamList, 
                std::unique_ptr<BlockNode> _block);
    ~FuncDefNode();

    std::string getSignature() const override;

    std::string getStr() const { return "funcDef"; }

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_funcDef; }
};

class FuncParamListNode : public AstNode
{
public:
    std::vector<std::unique_ptr<FuncParamNode>> funcParam;

    FuncParamListNode(std::vector<std::unique_ptr<FuncParamNode>> _funcParam);
    ~FuncParamListNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_funcParams; }
};


class FuncParamNode : public AstNode
{
public:
    AstType type;
    std::string varName;
    std::vector<int> dimSize;
    
    FuncParamNode(AstType _type, std::string _varName, 
                    std::vector<int> _dimSize);
    ~FuncParamNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_funcParam; }
};

class StmtNode : public AstNode
{
public:
    StmtNode(AstKind _kind);
    ~StmtNode() {}

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c)
    {
        bool decl = c->getKind() >= AST_decl && c->getKind() <= AST_prototype;;

        bool stmt = c->getKind() >= AST_stmt && c->getKind() <= AST_returnStmt;

        return decl || stmt;
    }
};

class DeclNode : public StmtNode
{
public:
    bool is_const;
    AstType type;

    DeclNode(AstKind _kind, bool _is_const, AstType _type)
            : StmtNode(_kind), is_const(_is_const), type(_type) {}
    ~DeclNode() {}

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() >= AST_decl 
                                                && c->getKind() <= AST_prototype; }
};

class ConstDeclNode : public DeclNode
{
public:
    std::vector<std::unique_ptr<ConstDefNode>> constDef;

    ConstDeclNode(AstType _type, 
                    std::vector<std::unique_ptr<ConstDefNode>> _constDef);
    ~ConstDeclNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_constDecl; }
};

class VarDeclNode : public DeclNode
{
public:
    std::vector<std::unique_ptr<VarDefNode>> varDef;

    VarDeclNode(AstType _type, std::vector<std::unique_ptr<VarDefNode>> _varDef);
    ~VarDeclNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_varDecl; }
};

class ConstDefNode : public AstNode
{
public:
    std::vector<int> dimSize;
    std::unique_ptr<InitValNode> constInitVal;
    std::string name;

    ConstDefNode(std::vector<int> _dimSize, 
                    std::unique_ptr<InitValNode> _constInitVal, std::string _name);
    ~ConstDefNode();
 
    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_constDef; }
};

class VarDefNode : public AstNode
{
public:
    std::vector<int> dimSize;
    std::unique_ptr<InitValNode> varInitVal;
    std::string name;

    VarDefNode(std::vector<int> _dimSize, 
                std::unique_ptr<InitValNode> _varInitVal, std::string _name);

    VarDefNode(std::vector<int> _dimSize, std::string _name);

    ~VarDefNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_varDef; }
};

class InitValNode : public AstNode
{
public:
    std::vector<std::unique_ptr<ExprNode>> initItem;

    InitValNode(std::vector<std::unique_ptr<ExprNode>> _initItem);
    ~InitValNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_initVal; }
};

class AssignStmtNode : public StmtNode
{
public:
    std::unique_ptr<LvalNode> lval;
    std::unique_ptr<ExprNode> expr;

    AssignStmtNode(std::unique_ptr<LvalNode> _lval, 
                    std::unique_ptr<ExprNode> _expr);
    ~AssignStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_assignStmt; }
};

class ExprStmtNode : public StmtNode
{
public:
    std::unique_ptr<ExprNode> expr;

    ExprStmtNode(std::unique_ptr<ExprNode> _expr);
    ~ExprStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_ifStmt; }
};

class IfStmtNode : public StmtNode
{
public:
    std::unique_ptr<ExprNode> cond;
    std::unique_ptr<StmtNode> thenBlock, elseBlock;
    
    IfStmtNode(std::unique_ptr<ExprNode> _cond, 
                std::unique_ptr<StmtNode> _thenBlock, 
                std::unique_ptr<StmtNode> _elseBlock);
    ~IfStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_expStmt; }
};

class ForStmtNode : public StmtNode
{
public:
    std::unique_ptr<StmtNode> initial, increment, stmt;
    std::unique_ptr<ExprNode> cond;

    ForStmtNode(std::unique_ptr<StmtNode> _initial, 
                std::unique_ptr<ExprNode> _cond,
                std::unique_ptr<StmtNode> _increment, 
                std::unique_ptr<StmtNode> _stmt);
    ~ForStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_forStmt; }
};

class WhileStmtNode : public StmtNode
{
public:
    std::unique_ptr<ExprNode> cond;
    std::unique_ptr<StmtNode> stmt;

    WhileStmtNode(std::unique_ptr<ExprNode> _cond, std::unique_ptr<StmtNode> _stmt);
    ~WhileStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_whileStmt; }
};

class BreakStmtNode : public StmtNode
{
public:
    bool is_break;

    BreakStmtNode(bool _is_break);
    ~BreakStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_breakStmt; }
};

class ContinueStmtNode : public StmtNode
{
public:
    bool is_contiune;

    ContinueStmtNode(bool _is_contiune);
    ~ContinueStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_continueStmt; }
};

class ReturnStmtNode : public StmtNode
{
public:
    std::unique_ptr<ExprNode> expr;
    bool has_ret_val;

    ReturnStmtNode(std::unique_ptr<ExprNode> _expr);
    ReturnStmtNode();
    ~ReturnStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_returnStmt; }
};

// TODO add the PrototypeNode
// class PrototypeNode : public DeclNode
// {
// public:
//     std::string funcName;
//     FuncParamListNode* funcParamList;

//     PrototypeNode(bool _is_const, AstType _type,
//                     std::string _funcName, FuncParamListNode* _funcParamList);
//     ~PrototypeNode();

//     std::string getSignature() const override;

//     /// LLVM style RTTI
//     static bool classof(const AstNode* c) { return c->getKind() == AST_prototype; }
// };

class BlockNode : public StmtNode
{
public:
    std::vector<std::unique_ptr<StmtNode>> blockItems; 

    BlockNode(std::vector<std::unique_ptr<StmtNode>> _blockItems);
    ~BlockNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_block; }
};

class ExprNode : public AstNode
{
public:
    AstType type;
    ExprNode(AstKind _kind, AstType _type);
    ~ExprNode();

    std::string getSignature() const override;

    void setImmediate(Immediate _immediate);

    bool ready() { return this->kind == AST_Number || this->kind == AST_ConditionNumber; }

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() >= AST_exp && c->getKind() <= AST_Number; }
};

class UnaryExprNode : public ExprNode
{
public:
    UnaryOp op;
    std::unique_ptr<ExprNode> expr;
    // ExprNode* expr;

    UnaryExprNode(AstType _type, UnaryOp _op, std::unique_ptr<ExprNode> _unaryExpr);

    ~UnaryExprNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_unaryExp; }

};

class BinaryExprNode : public ExprNode
{
public:
    bool shortCircuit;
    BinaryOp op;
    std::unique_ptr<ExprNode> lhs, rhs;

    BinaryExprNode(AstType _type, BinaryOp _op, 
                    std::unique_ptr<ExprNode> _lhs, std::unique_ptr<ExprNode> _rhs);
    ~BinaryExprNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_binaryExp; }
};

class FunctionCallExprNode : public ExprNode
{
public:
    std::string funcName;
    std::vector<std::unique_ptr<ExprNode>> funcParamList;
    // std::vector<ExprNode*> funcParamList;
    std::string format;

    FunctionCallExprNode(AstType _type, std::string funcName, 
                            std::vector<std::unique_ptr<ExprNode>> funcParamList);
    ~FunctionCallExprNode();

    std::string getSignature() const override;
    static bool classof(const AstNode* c) { return c->getKind() == AST_functCallExp; }
};

class LvalNode : public ExprNode
{
public:
    std::string variable;
    std::vector<std::unique_ptr<ExprNode>> indexExpr;
    std::vector<int> dimSize;

    LvalNode(AstType _type, std::string _variable, std::vector<std::unique_ptr<ExprNode>> _indexExpr);
    
    LvalNode(AstType _type, std::string _variable, std::vector<std::unique_ptr<ExprNode>> _indexExpr, std::vector<int> _dimSize);

    ~LvalNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_lVal; }
};

class NumberNode : public ExprNode
{
public:
    Immediate immediate;

    NumberNode(AstType _type, Immediate _immediate);
    
    ~NumberNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_Number; }
};


} // namespace ast
} // namespace sysy

#endif