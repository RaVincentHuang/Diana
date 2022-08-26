#ifndef SYSY_AST_H
#define SYSY_AST_H
#include "sysy/Frontend/AstType.h"
#include "sysy/Frontend/SysYOp.h"
#include <vector>


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

class PrototypeNode;


class ExprNode;
// class AddExprNode;
class CondExprNode;
// class LOrExprNode;
// class LAndExprNode;
// class EqExprNode;
// class RelExprNode;
// class MulExprNode;
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
        // AST_ConditionExp,
        // AST_ConditionUnaryExp,
        // AST_ConditionBinaryExp,
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
    AstNode* parent;
    std::vector<AstNode*> children;

    AstNode(AstKind _kind)
        : kind(_kind)
        , parent(nullptr) {}
    // AstNode()
    //     : loc()
    //     , kind(AST_Undefined)
    //     , parent(nullptr) {}
    
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
    std::vector<AstNode*> element;
    // std::vector<FuncDefNode*> funcDef;
    // std::vector<DeclNode*> decl;

    // CompUnitNode(const Location& _loc, std::vector<FuncDefNode*> _funcDef, 
    //             std::vector<DeclNode*> _decl, std::vector<AstNode*> _element);
    
    CompUnitNode(std::vector<AstNode*> _element);

    ~CompUnitNode();

    // Need override getSignature.
    std::string getSignature() const override;

    std::vector<PrototypeNode*>& getPrototype();

    std::string getStr() const { return "compUnit"; }

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_compUnit; }
};

class FuncDefNode : public AstNode
{
public:
    AstType funcType;
    std::string funcName;
    FuncParamListNode* funcParamList;
    BlockNode* block;

    FuncDefNode(AstType _funcType, std::string _funcName,
                FuncParamListNode* _funcParamList, BlockNode* _block);
    ~FuncDefNode();

    std::string getSignature() const override;

    std::string getStr() const { return "funcDef"; }

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_funcDef; }
};

class FuncParamListNode : public AstNode
{
public:
    std::vector<FuncParamNode*> funcParam;

    FuncParamListNode(std::vector<FuncParamNode*> _funcParam);
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

    virtual std::string getName() { return "globalVar"; };

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
    std::vector<ConstDefNode*> constDef;

    ConstDeclNode(AstType _type, 
                    std::vector<ConstDefNode*> _constDef);
    ~ConstDeclNode();

    // std::string getName() { return constDef }

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_constDecl; }
};

class VarDeclNode : public DeclNode
{
public:
    std::vector<VarDefNode*> varDef;

    VarDeclNode(AstType _type, std::vector<VarDefNode*> _varDef);
    ~VarDeclNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_varDecl; }
};

class ConstDefNode : public AstNode
{
public:
    std::vector<int> dimSize;
    InitValNode* constInitVal;
    std::string name;

    ConstDefNode(std::vector<int> _dimSize, InitValNode* _constInitVal, std::string _name);
    ~ConstDefNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_constDef; }
};

class VarDefNode : public AstNode
{
public:
    std::vector<int> dimSize;
    InitValNode* varInitVal;
    std::string name;

    VarDefNode(std::vector<int> _dimSize, InitValNode* _varInitVal, std::string _name);

    VarDefNode(std::vector<int> _dimSize, std::string _name);

    ~VarDefNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_varDef; }
};

class InitValNode : public AstNode
{
public:
    std::vector<ExprNode*> initItem;

    InitValNode(std::vector<ExprNode*> _initItem);
    ~InitValNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_initVal; }
};

class AssignStmtNode : public StmtNode
{
public:
    LvalNode* lval;
    ExprNode* expr;

    AssignStmtNode(LvalNode* _lval, ExprNode* _expr);
    ~AssignStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_assignStmt; }
};

class ExprStmtNode : public StmtNode
{
public:
    ExprNode* expr;

    ExprStmtNode(ExprNode* _expr);
    ~ExprStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_ifStmt; }
};

class IfStmtNode : public StmtNode
{
public:
    ExprNode* cond;
    StmtNode* thenBlock,* elseBlock;
    
    IfStmtNode(ExprNode* _cond, StmtNode* _thenBlock, StmtNode* _elseBlock);
    ~IfStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_expStmt; }
};

class ForStmtNode : public StmtNode
{
public:
    StmtNode* initial,* increment,* stmt;
    ExprNode* cond;

    ForStmtNode(StmtNode* _initial, ExprNode* _cond,
                StmtNode* _increment, StmtNode* _stmt);
    ~ForStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_forStmt; }
};

class WhileStmtNode : public StmtNode
{
public:
    ExprNode* cond;
    StmtNode* stmt;

    WhileStmtNode(ExprNode* _cond, StmtNode* _stmt);
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
    ExprNode* expr;
    bool has_ret_val;

    ReturnStmtNode(ExprNode* _expr);
    ReturnStmtNode();
    ~ReturnStmtNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_returnStmt; }
};


class PrototypeNode : public DeclNode
{
public:
    std::string funcName;
    FuncParamListNode* funcParamList;

    PrototypeNode(bool _is_const, AstType _type,
                    std::string _funcName, FuncParamListNode* _funcParamList);
    ~PrototypeNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_prototype; }
};

class BlockNode : public StmtNode
{
public:
    std::vector<StmtNode*> blockItems;

    BlockNode(std::vector<StmtNode*> _blockItems);
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
    ExprNode* expr;

    UnaryExprNode(AstType _type, UnaryOp _op, ExprNode* _unaryExpr);

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
    ExprNode* lhs,* rhs;

    BinaryExprNode(AstType _type, BinaryOp _op, ExprNode* _lhs, ExprNode* _rhs);
    ~BinaryExprNode();

    std::string getSignature() const override;

    /// LLVM style RTTI
    static bool classof(const AstNode* c) { return c->getKind() == AST_binaryExp; }
};

class FunctionCallExprNode : public ExprNode
{
public:
    std::string funcName;
    std::vector<ExprNode*> funcParamList;
    std::string format;

    FunctionCallExprNode(AstType _type, std::string funcName, std::vector<ExprNode*> funcParamList);
    ~FunctionCallExprNode();

    std::string getSignature() const override;
    static bool classof(const AstNode* c) { return c->getKind() == AST_functCallExp; }
};

class LvalNode : public ExprNode
{
public:
    std::string variable;
    std::vector<ExprNode*> indexExpr;
    std::vector<int> dimSize;

    LvalNode(AstType _type, std::string _variable, std::vector<ExprNode*> _indexExpr);
    
    LvalNode(AstType _type, std::string _variable, std::vector<ExprNode*> _indexExpr, std::vector<int> _dimSize);

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