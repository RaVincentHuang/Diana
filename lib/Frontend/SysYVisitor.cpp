#include "sysy/Frontend/SysYVisitor.h"
#include "sysy/Frontend/SysYParser.h"
#include "sysy/Frontend/Ast.h"
#include "sysy/Frontend/Semantics.h"
#include "sysy/Frontend/AstTransform.h"
#include "sysy/Support/Common.h"
#include "sysy/Support/StrUtils.h"
#include "sysy/Support/Debug.h"

namespace sysy
{

using namespace ast;

template<class T>
T unpackAny(antlrcpp::Any&& data)
{
    if (data.isNull())
        return nullptr;
    return data.as<T>();
}

template<class T>
std::vector<T> unpackAnyVec(antlrcpp::Any&& data)
{
    std::vector<T> vec;
    if(data.isNotNull())
        return data.as<std::vector<T>>();
    return vec;
}

static std::vector<int> dims;
static int depth;

void SysYVisitor::printCst(antlr4::ParserRuleContext *ctx, int depth)
{
    for(int i = 1;i < depth; i++)
        if (i == 1)
            std::cout << "\t";
        else
            std::cout << "|\t";
    if(ctx->depth() != depth)
    {
        std::cout <<"├-> " << "\033[32m<\033[m" << ctx->toStringTree() << "\033[32m>\033[m" << std::endl;
        return;
    }

    size_t num = ctx->getRuleIndex();
    if(ctx->getRuleIndex() != SysYParser::RuleCompUnit)
        std::cout << "├--> ";
    
    std::cout << "\033[36m[\033[m" << parser.getRuleNames().at(num) << "\033[36m]\033[m" << std::endl;
    for(auto child : ctx->children)
        printCst((antlr4::ParserRuleContext *)child, depth + 1);
}

void SysYVisitor::printSrc(SysYParser::CompUnitContext* ctx)
{
    auto vec = ctx->children;
    size_t num = ctx->getAltNumber();
    std::cout << parser.getRuleNames().at(num) << std::endl;
    std::cout << vec.size() << std::endl;
    for(auto iter: vec)
        std::cout<< iter->toString() << std::endl << iter->getText() << std::endl;
}

/// \return return type is \p CompUnitNode*
antlrcpp::Any SysYVisitor::visitCompUnit(SysYParser::CompUnitContext *ctx)
{
    PrintMsg("visitCompUnit");

    std::vector<std::unique_ptr<AstNode>> _element;

    auto& scope = AstScope::getCurrentPtr();
    for(auto [name, type] : storedRuntime)
        scope->insertVariable(name, type);

    for(auto item : ctx->element())
    {
        if(auto decl = item->decl())
        {
            auto ele = unpackAny<DeclNode*>(visitDecl(decl));
            _element.push_back(std::unique_ptr<DeclNode>(ele));
        }
        else if(auto funcDef = item->funcDef())
        {
            auto ele = unpackAny<FuncDefNode*>(visitFuncDef(funcDef));
            _element.push_back(std::unique_ptr<FuncDefNode>(ele));
        }
    }
    CompUnitNode* ast = new CompUnitNode(std::move(_element));
    return ast;
}

[[maybe_unused]] antlrcpp::Any SysYVisitor::visitElement(SysYParser::ElementContext *ctx) {}

/// \return return type is \p DeclNode*
antlrcpp::Any SysYVisitor::visitDecl(SysYParser::DeclContext *ctx)
{
    PrintMsg("visitDecl");
    if(auto constDecl = ctx->constDecl())
        return (DeclNode*) unpackAny<ConstDeclNode*>(visitConstDecl(constDecl));
    if(auto varDecl = ctx->varDecl())
        return (DeclNode*) unpackAny<VarDeclNode*>(visitVarDecl(varDecl));
}


static AstType constDeclType;
/// \return return type is \p ConstDeclNode*
antlrcpp::Any SysYVisitor::visitConstDecl(SysYParser::ConstDeclContext *ctx)
{
    PrintMsg("visitConstDecl");
    
    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if (ctx->bType()->FLOAT())
        _type = FLOAT;
    
    std::vector<std::unique_ptr<ConstDefNode>> _constDef;
    constDeclType = _type;
    for(auto constDef : ctx->constDef())
    {
        auto def = unpackAny<ConstDefNode*>(visitConstDef(constDef));
        _constDef.push_back(std::unique_ptr<ConstDefNode>(def));
    }
    
    ConstDeclNode* ret = new ConstDeclNode(_type, std::move(_constDef));
    return ret;
}
  
[[maybe_unused]] antlrcpp::Any SysYVisitor::visitBType(SysYParser::BTypeContext *ctx){}

/// \return return type is \p ConstDefNode*
antlrcpp::Any SysYVisitor::visitConstDef(SysYParser::ConstDefContext *ctx)
{
    PrintMsg("visitConstDef");
    std::vector<int> _dimSize;
    std::string _name = ctx->Ident()->toString();
    for(auto constExp : ctx->constExp())
    {
        auto expr = unpackAny<ExprNode*>(visitConstExp(constExp));
        if(auto number = dynamic_cast<NumberNode*>(expr))
            _dimSize.push_back(number->immediate.getInt());
        else
            ErrorMsg("The Initial number of the array is not the constant!"); 
    }
    auto& scope = AstScope::getCurrentPtr();

    scope->insertVariable(_name, constDeclType);
    if(!_dimSize.empty())
        scope->insertDim(_name, _dimSize);
    
    std::unique_ptr<InitValNode> _constInitVal;
    if(auto constInitVal = ctx->constInitVal()) 
    {   
        int size = 1, cnt = 0;
        for(auto s : _dimSize)
            size *= s;
        dims.clear();
        while (size > 1)
        {
            dims.push_back(size);
            size /= _dimSize[cnt++];
        }
        auto initval = unpackAny<InitValNode*>(visitConstInitVal(constInitVal));
        _constInitVal = std::unique_ptr<InitValNode>(initval);
        dims.clear();
    }
    
    scope->setCosnt(_name);
    if(_constInitVal)
    {
        for(auto& item : _constInitVal->initItem)
            if(auto number = dynamic_cast<NumberNode*>(item.get()))
            {
                auto& imm = number->immediate;
                imm.typeCast(constDeclType); 
            }
        scope->insertInitial(_name, _constInitVal.get());
    }
    
    ConstDefNode* ret = new ConstDefNode(_dimSize, std::move(_constInitVal), _name);
    return ret;
}

std::vector<std::unique_ptr<ExprNode>> SysYVisitor::constInitValDfs(SysYParser::ConstInitValContext *ctx, int idx)
{
    PrintMsg("constInitValDfs");
    
    std::vector<std::unique_ptr<ExprNode>> res;
    if(auto cosntExp = ctx->constExp())
    {
        auto expr = unpackAny<ExprNode*>(visitConstExp(cosntExp));
        res.push_back(std::unique_ptr<ExprNode>(expr));
        return res;
    }
    for(auto constInitVal : ctx->constInitVal())
    {
        auto vec = constInitValDfs(constInitVal, idx + 1);
        for(auto& item : vec)
            res.push_back(std::move(item));
        // res.insert(res.end(), vec.begin(), vec.end());
    }
    auto dim = idx ? dims.at(dims.size() - depth + idx) : dims.at(idx);
    if(!idx && res.empty() && dim > 40)
        return {};
    if(auto cle = dim - res.size(); cle > 0)
        for(auto i = 0; i < cle; i++)
            res.push_back(std::make_unique<NumberNode>(INT, 0));
        // res.insert(res.end(), cle, std::make_unique<NumberNode>(INT, 0));
    return res;
}

/// \return return type is \p InitValNode*
antlrcpp::Any SysYVisitor::visitConstInitVal(SysYParser::ConstInitValContext *ctx)
{
    std::function<int(SysYParser::ConstInitValContext*)> dfs = [&](SysYParser::ConstInitValContext* ctx) -> int {
        if(ctx->constExp())
            return 0;
        int maxn = 1;
        for(auto init : ctx->constInitVal())
            maxn = std::max(maxn, dfs(init) + 1);
        return maxn;
    };
    depth = dfs(ctx);
    auto _initItem = constInitValDfs(ctx, 0);

    InitValNode* init = new InitValNode(std::move(_initItem));
    return init;
}

/// \return return \p VarDeclNode*
antlrcpp::Any SysYVisitor::visitVarDecl(SysYParser::VarDeclContext *ctx)
{
    PrintMsg("visitVarDecl");

    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if(ctx->bType()->FLOAT())
        _type = FLOAT;
// TODO Add the other type    
    std::vector<std::unique_ptr<VarDefNode>> _varDef;
    for(auto varDef : ctx->varDef())
    {
        auto def = unpackAny<VarDefNode*>(visitVarDef(varDef));
        _varDef.push_back(std::unique_ptr<VarDefNode>(def));
    }

    auto& scope = AstScope::getCurrentPtr();
    for(auto& def : _varDef)
    {
        scope->insertVariable(def->name, _type);
        if(!def->dimSize.empty())
            scope->insertDim(def->name, def->dimSize);
    }
    
    
    auto ret = new VarDeclNode(_type, std::move(_varDef));
    return ret;
}

/// \return return type is \p VarDeclNode*
antlrcpp::Any SysYVisitor::visitVarDecl_in(SysYParser::VarDecl_inContext *ctx)
{
    PrintMsg("visitVarDecl_in");
    

    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if(ctx->bType()->FLOAT())
        _type = FLOAT;
// TODO Add the other type    
    std::vector<std::unique_ptr<VarDefNode>> _varDef;
    for(auto varDef : ctx->varDef())
    {
        auto def = unpackAny<VarDefNode*>(visitVarDef(varDef));
        _varDef.push_back(std::unique_ptr<VarDefNode>(def));
    }
    
    auto& scope = AstScope::getCurrentPtr();
    for(auto& def : _varDef)
    {
        scope->insertVariable(def->name, _type);
        if(!def->dimSize.empty())
            scope->insertDim(def->name, def->dimSize);
    }
    
    auto ret = new VarDeclNode(_type, std::move(_varDef));
    return ret;
}

/// \return return type is \p VarDefNode*
antlrcpp::Any SysYVisitor::visitVarDef(SysYParser::VarDefContext *ctx)
{
    PrintMsg("visitVarDef");

    std::string _name = ctx->Ident()->toString();
    std::vector<int> _dimSize;
    for(auto constExp : ctx->constExp())
    {
        auto expr = unpackAny<ExprNode*>(visitConstExp(constExp));
        if(auto number = dynamic_cast<NumberNode*>(expr))
            _dimSize.push_back(number->immediate.getInt());
        else
            ErrorMsg("The Initial number of the array is not the constant!"); 
    }

    if(auto initVal = ctx->initVal())
    {
        PrintMsg(_name);
        int size = 1, cnt = 0;
        for(auto s : _dimSize)
            size *= s;
        dims.clear();
        while (cnt < _dimSize.size())
        {
            dims.push_back(size);
            size /= _dimSize[cnt++];
        }
        InitValNode* init = unpackAny<InitValNode*>(visitInitVal(initVal));
        auto _varInitVal = std::unique_ptr<InitValNode>(init);
        dims.clear();
        auto ret = new VarDefNode(_dimSize, std::move(_varInitVal), _name);
        return ret;
    }

    auto ret = new VarDefNode( _dimSize, _name);
    return ret;
}

std::vector<std::unique_ptr<ExprNode>> SysYVisitor::varInitValDfs(SysYParser::InitValContext *ctx, int idx)
{
    PrintMsg("varInitValDfs");
    std::vector<std::unique_ptr<ExprNode>> res;
    if(auto exp = ctx->exp())
    {
        auto expr = unpackAny<ExprNode*>(visitExp(exp));
        res.push_back(std::unique_ptr<ExprNode>(expr));
        return res;
    }
    for(auto initVal : ctx->initVal())
    {
        auto vec = varInitValDfs(initVal, idx + 1);
        for(auto& item : vec)
            res.push_back(std::move(item));
        // res.insert(res.end(), vec.begin(), vec.end());
    }
    auto dim = idx ? dims.at(dims.size() - depth + idx) : dims.at(idx);
    if(!idx && res.empty() && dim > 40)
        return {};
    if(auto cle = dim - res.size(); cle > 0)
        for(auto i = 0; i < cle; i++)
            res.push_back(std::make_unique<NumberNode>(INT, 0));
        // res.insert(res.end(), cle, std::make_unique<NumberNode>( INT, 0));
    return res;
}

/// \return return type is \p InitValNode*
antlrcpp::Any SysYVisitor::visitInitVal(SysYParser::InitValContext *ctx)
{
    PrintMsg("visitInitVal");
    std::function<int(SysYParser::InitValContext*)> dfs = [&](SysYParser::InitValContext* ctx) -> int {
        if(ctx->exp())
            return 0;
        int maxn = 1;
        for(auto init : ctx->initVal())
            maxn = std::max(maxn, dfs(init) + 1);
        return maxn;
    };

    depth = dfs(ctx);

    auto _initItem = varInitValDfs(ctx, 0);

    auto init = new InitValNode(std::move(_initItem));
    return init;
}

/// \return return type is \p FuncDefNode*
antlrcpp::Any SysYVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx)
{
    PrintMsg("visitFuncDef");
    
    AstType _funcType;
    if(ctx->funcType()->INT())
        _funcType = INT;
    else if(ctx->funcType()->VOID())
        _funcType = VOID;
    else if(ctx->funcType()->FLOAT())
        _funcType = FLOAT;
// TODO Add the other type        
    std::string _funcName = ctx->Ident()->toString();

    std::unique_ptr<FuncParamListNode> _funcParamList;
    if(auto funcFParams = ctx->funcFParams())
    {
        auto funcPara = unpackAny<FuncParamListNode*>(visitFuncFParams(funcFParams));
        _funcParamList = std::unique_ptr<FuncParamListNode>(funcPara);
    }

    auto& scope = AstScope::getCurrentPtr();

    scope->insertVariable(_funcName, _funcType);
    
    scope = scope->createChild();

    if(_funcParamList)
        for(auto& arg : _funcParamList->funcParam)
        {
            scope->insertVariable(arg->varName, arg->type);
            if(!arg->dimSize.empty())
                scope->insertDim(arg->varName, arg->dimSize);
        }
    auto block  = ctx->block(); 
    BlockNode* _block = unpackAny<BlockNode*>(visitBlock(block));
    
    FuncDefNode* ret = new FuncDefNode( _funcType, _funcName, 
                    std::move(_funcParamList), std::unique_ptr<BlockNode>(_block));
    scope = AstScope::exitBlock(scope);
    return ret;
}
  
[[maybe_unused]] antlrcpp::Any SysYVisitor::visitFuncType(SysYParser::FuncTypeContext *ctx){}

/// \return return type is \p FuncParamListNode*  
antlrcpp::Any SysYVisitor::visitFuncFParams(SysYParser::FuncFParamsContext *ctx)
{
    PrintMsg("visitFuncFParams");
    

    std::vector<std::unique_ptr<FuncParamNode>> _funcParam;
    for(auto funcFParam : ctx->funcFParam())
    {
        auto funcPara = unpackAny<FuncParamNode*>(visitFuncFParam(funcFParam));
        _funcParam.push_back(std::unique_ptr<FuncParamNode>(funcPara));
    }
    
    auto ret = new FuncParamListNode(std::move(_funcParam));
    return ret;
}

/// \return return type is \p FuncParamNode*
antlrcpp::Any SysYVisitor::visitFuncFParam(SysYParser::FuncFParamContext *ctx)
{
    PrintMsg("visitFuncFParam");
    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if(ctx->bType()->FLOAT())
        _type = FLOAT;
// TODO Add the other type    
    std::string _varName = ctx->Ident()->toString();

    std::vector<int> _dimSize;
    if(ctx->LSBRA(0))
        _dimSize.push_back(0);
    for(auto exp : ctx->exp())
    {
        auto expr = unpackAny<ExprNode*>(visitExp(exp));
        if(auto number = dynamic_cast<NumberNode*>(expr))
            _dimSize.push_back(number->immediate.getInt());
        else
            ErrorMsg("The Initial number of the array is not the constant!"); 
    }
    
    auto ret = new FuncParamNode(_type, _varName, _dimSize);
    return ret;
}

/// \return return type is \p BlockNode*
antlrcpp::Any SysYVisitor::visitBlock(SysYParser::BlockContext *ctx)
{
    PrintMsg("visitBlock");
    

    auto& scope = AstScope::getCurrentPtr();
    scope = scope->createChild();

    std::vector<std::unique_ptr<StmtNode>> _blockItems;
    for(auto blockItem : ctx->blockItem())
        if(auto item = unpackAny<StmtNode*>(visitBlockItem(blockItem)))
            _blockItems.push_back(std::unique_ptr<StmtNode>(item));

    scope = AstScope::exitBlock(scope);
    
    auto ret = new BlockNode(std::move(_blockItems));
    ast::removeDeadStmts(ret);
    return ret;
}

/// \return return type is \p StmtNode*
antlrcpp::Any SysYVisitor::visitBlockItem(SysYParser::BlockItemContext *ctx)
{
    PrintMsg("visitBlockItem");
    if(auto decl = ctx->decl())
        return (StmtNode*) unpackAny<DeclNode*>(visitDecl(decl));
    else if(auto stmt = ctx->stmt())
        return unpackAny<StmtNode*>(visitStmt(stmt));
    else
        return nullptr;
} 

/// \return return type is \p StmtNode*
antlrcpp::Any SysYVisitor::visitStmt_in(SysYParser::Stmt_inContext *ctx)
{
    PrintMsg("visitStmt_in");
    

    if(auto lval = ctx->lVal())
    {
        LvalNode* l = unpackAny<LvalNode*>(visitLVal(lval));
        auto _lval = std::unique_ptr<LvalNode>(l);

        auto expr = ctx->exp();
        auto e = unpackAny<ExprNode*>(visitExp(expr));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto ret = new AssignStmtNode(std::move(_lval), std::move(_expr));
        return (StmtNode*) ret;
    }
    else if(auto exp = ctx->exp())
    {
        ExprNode* e = unpackAny<ExprNode*>(visitExp(exp));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto ret = new ExprStmtNode(std::move(_expr));
        return (StmtNode*) ret;
    }
    return nullptr;
}

/// \return return type is \p StmtNode*
antlrcpp::Any SysYVisitor::visitStmt(SysYParser::StmtContext *ctx)
{
    PrintMsg("visitStmt");
    if(ctx->lVal() && ctx->AGN() && ctx->exp())
    {
        auto lval = ctx->lVal();
        LvalNode* l = unpackAny<LvalNode*>(visitLVal(lval));
        auto _lval = std::unique_ptr<LvalNode>(l);

        auto expr = ctx->exp();
        ExprNode* e = unpackAny<ExprNode*>(visitExp(expr));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto ret = new AssignStmtNode(std::move(_lval), std::move(_expr));
        return (StmtNode*) ret;
    }
    else if(ctx->RETURN())
    {
        if(auto exp = ctx->exp())
        {
            ExprNode* e = unpackAny<ExprNode*>(visitExp(exp));
            auto _expr = std::unique_ptr<ExprNode>(e);

            auto ret = new ReturnStmtNode(std::move(_expr));
            return (StmtNode*) ret;
        }
        auto ret = new ReturnStmtNode;
        return (StmtNode*) ret;
    }
    else if(auto exp = ctx->exp())
    {
        ExprNode* e = unpackAny<ExprNode*>(visitExp(exp));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto ret = new ExprStmtNode(std::move(_expr));
        return (StmtNode*) ret;
    }
    else if(auto block = ctx->block())
        return (StmtNode*) unpackAny<BlockNode*>(visitBlock(block));
    else if(ctx->IF())
    {
        auto cond = ctx->cond();
        ExprNode* e = unpackAny<ExprNode*>(visitCond(cond));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto thenBlock = ctx->stmt().at(0);
        StmtNode* then = unpackAny<StmtNode*>(visitStmt(thenBlock));
        auto _thenBlock = std::unique_ptr<StmtNode>(then);

        std::unique_ptr<StmtNode> _elseBlock;
        if(ctx->stmt().size() == 2)
        {
            auto elseBlock = ctx->stmt().at(1);
            auto elseB = unpackAny<StmtNode*>(visitStmt(elseBlock));
            _elseBlock = std::unique_ptr<StmtNode>(elseB);
        }

        auto ret = new IfStmtNode(std::move(_expr), std::move(_thenBlock), std::move(_elseBlock));
        return (StmtNode*) ret;
    }
    else if(ctx->FOR())
    {

        auto cond = ctx->cond();
        ExprNode* e = unpackAny<ExprNode*>(visitCond(cond));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto stmt = ctx->stmt().at(0);
        StmtNode* s = unpackAny<StmtNode*>(visitStmt(stmt));
        auto _stmt = std::unique_ptr<StmtNode>(s);

        std::unique_ptr<StmtNode> _initial;
        std::unique_ptr<StmtNode> _increment;
        if(auto initial = ctx->varDecl_in())
        {
            auto init = unpackAny<VarDeclNode*>(visitVarDecl_in(initial));
            _initial = std::unique_ptr<StmtNode>(init);
            
            auto increment = ctx->stmt_in().at(0);
            auto incr = unpackAny<StmtNode*>(visitStmt_in(increment));
            _increment = std::unique_ptr<StmtNode>(incr);
        }
        else
        {
            auto initial_ = ctx->stmt_in().at(0);
            auto init = unpackAny<StmtNode*>(visitStmt_in(initial_));
            _initial = std::unique_ptr<StmtNode>(init);

            auto increment = ctx->stmt_in().at(1);
            auto incr = unpackAny<StmtNode*>(visitStmt_in(increment));
            _increment = std::unique_ptr<StmtNode>(incr);
        }

        auto ret = new ForStmtNode(std::move(_initial), std::move(_expr), 
                                    std::move(_increment), std::move(_stmt));
        return (StmtNode*) ret;
    }
    else if(ctx->WHILE())
    {
        auto cond = ctx->cond();
        ExprNode* e = unpackAny<ExprNode*>(visitCond(cond));
        auto _expr = std::unique_ptr<ExprNode>(e);

        auto stmt = ctx->stmt().at(0);
        StmtNode* s = unpackAny<StmtNode*>(visitStmt(stmt));
        auto _stmt = std::unique_ptr<StmtNode>(s);

        auto ret = new WhileStmtNode(std::move(_expr), std::move(_stmt));
        return (StmtNode*) ret;
    }
    else if(ctx->CONTINUE())
    {
        auto ret = new ContinueStmtNode(true);
        return (StmtNode*) ret;
    }
    else if(ctx->BREAK())
    {
        auto ret = new BreakStmtNode(true);
        return (StmtNode*) ret;
    }
    else
        return nullptr;
}

/// \return return type is \p ExprNode*
antlrcpp::Any SysYVisitor::visitExp(SysYParser::ExpContext *ctx)
{
    PrintMsg("visitExp");
    auto addExp = ctx->addExp();
    auto expr = unpackAny<ExprNode*>(visitAddExp(addExp));
    PrintMsg("Exit");
    return expr;
}

/// \return return type is \p ExprNode*
antlrcpp::Any SysYVisitor::visitCond(SysYParser::CondContext *ctx)
{
    PrintMsg("visitCond");
    auto lOrExp = ctx->lOrExp();
    ExprNode* expr = unpackAny<ExprNode*>(visitLOrExp(lOrExp));
    return expr;
}

/// \return return type is \p LvalNode*
antlrcpp::Any SysYVisitor::visitLVal(SysYParser::LValContext *ctx)
{
    PrintMsg("visitLVal");
    
    std::string _variable = ctx->Ident()->toString();
    std::vector<std::unique_ptr<ExprNode>> _expr;
    for(auto exp : ctx->exp())
    {
        auto expr = unpackAny<ExprNode*>(visitExp(exp));
        _expr.push_back(std::unique_ptr<ExprNode>(expr));
    }
        
    
    auto& scope = AstScope::getCurrentPtr();
    AstType _type = scope->getType(_variable);
    LvalNode* ret;
    if(!_expr.empty())
        ret = new LvalNode(_type, _variable, std::move(_expr), scope->getDim(_variable));
    else
        ret = new LvalNode(_type, _variable, std::move(_expr));
    PrintMsg("Exit");
    return ret;
}

/// \return return type is \p ExprNode*  
antlrcpp::Any SysYVisitor::visitPrimaryExp(SysYParser::PrimaryExpContext *ctx)
{
    PrintMsg("visitPrimaryExp");
    if(auto exp = ctx->exp())
    {
        ExprNode* _expr = unpackAny<ExprNode*>(visitExp(exp));
        PrintMsg("Exit");
        return _expr;
    }
    else if(auto lval = ctx->lVal())
    {
        auto& scope = AstScope::getCurrentPtr();
        auto name = lval->Ident()->toString();
        if(scope->isConst(name))
        {
            Immediate _immediate;
            AstType _type = scope->getType(name);
            if(auto initial = scope->getInitial(name))
            {
                std::vector<NumberNode*> _expr;
                for(auto exp : lval->exp())
                    if(auto number = dynamic_cast<NumberNode*>(unpackAny<ExprNode*>(visitExp(exp))))
                        _expr.push_back(number);
                    else
                        goto LVAL;
                if(_expr.empty())
                {
                    auto& init = initial->initItem.at(0);
                    if(auto number = dynamic_cast<NumberNode*>(init.get()))
                    {
                        auto ret = new NumberNode(number->type, number->immediate);
                        return (ExprNode*) ret;
                    }
                    ErrorMsg("Constant initial is not contant");
                }
                auto dim = scope->getDim(name);
                int index = 0, cnt = 0;
                for(auto riter = _expr.rbegin(); riter != _expr.rend(); riter++)
                {  
                    if(!cnt)
                        index += (*riter)->immediate.getInt();
                    else
                        index += (*riter)->immediate.getInt() * dim.at(dim.size() - cnt);
                    cnt++;
                }
                auto& init = initial->initItem.at(index);
                if(auto number = dynamic_cast<NumberNode*>(init.get()))
                {
                    auto ret = new NumberNode(number->type, number->immediate);
                    return (ExprNode*) ret;
                }
                ErrorMsg("Constant initial is not contant");
            }
            else
            {
                if(_type == INT)
                    _immediate = 0;
                else
                    _immediate = 0.0f;
                auto ret = new NumberNode(_type, _immediate);
                return (ExprNode*) ret;
            }
        }
LVAL:
        LvalNode* _lval = unpackAny<LvalNode*>(visitLVal(lval));
        PrintMsg("Exit");
        return (ExprNode*) _lval;
    }
    else
    {
        Immediate _immediate;
        AstType _type;
        if(auto intCtx = ctx->number()->intConst())
        {
            _immediate = visitIntConst(intCtx).as<int>();
            _type = INT;
        }
        else if(auto floatCtx = ctx->number()->floatConst())
        {
            _immediate = visitFloatConst(floatCtx).as<float>();
            _type = FLOAT;
        }
        auto ret = new NumberNode(_type, _immediate);
        PrintMsg("Exit");
        return (ExprNode*) ret;
    }
}
  
[[maybe_unused]] antlrcpp::Any SysYVisitor::visitNumber(SysYParser::NumberContext *ctx){}

/// \return return type is \p ExprNode*
antlrcpp::Any SysYVisitor::visitUnaryExp(SysYParser::UnaryExpContext *ctx)
{
    PrintMsg("visitUnaryExp");
    
    if(auto primaryExp = ctx->primaryExp())
    {
        ExprNode* _primaryExpr = unpackAny<ExprNode*>(visitPrimaryExp(primaryExp));
        PrintMsg("Exit");
        return _primaryExpr;
    }
    else if(ctx->Ident())
    {
        std::string _funcName = ctx->Ident()->toString();
        if(_funcName == "starttime")
        {
            auto& line = Line::getInstance();
            line.start = ctx->getStart()->getLine();
        }
        if(_funcName == "stoptime")
        {
            auto& line = Line::getInstance();
            line.end = ctx->getStart()->getLine();
        }
        std::vector<std::unique_ptr<ExprNode>> _funcParamList;
        if(auto funcRParams = ctx->funcRParams())
            for(auto exp : funcRParams->exp())
            {
                auto expr = unpackAny<ExprNode*>(visitExp(exp));
                _funcParamList.push_back(std::unique_ptr<ExprNode>(expr));
            }

        auto& scope = AstScope::getCurrentPtr();
        auto _type = scope->getType(_funcName);
        
        auto ret = new FunctionCallExprNode(_type, _funcName, std::move(_funcParamList));
        if(ctx->funcRParams() && !ctx->funcRParams()->STRING().empty())
            ret->format = ctx->funcRParams()->STRING().at(0)->toString();
        PrintMsg("Exit");
        return (ExprNode*) ret;
    }
    else
    {
        auto unaryExp = ctx->unaryExp();
        auto expr = unpackAny<ExprNode*>(visitUnaryExp(unaryExp));
        auto _unaryExpr = std::unique_ptr<ExprNode>(expr);

        if(_unaryExpr->ready())
        {
            // TODO Add the other type    
            auto number = dynamic_cast<NumberNode*>(_unaryExpr.get());
            Immediate _immediate;
            AstType _type = _unaryExpr->type;
            if(ctx->unaryOp()->ADD())
                _immediate.computeAssign(number->immediate, [](auto x) -> auto { return + x; });
            else if(ctx->unaryOp()->SUB())
                _immediate.computeAssign(number->immediate, [](auto x) -> auto { return - x; });
            else if(ctx->unaryOp()->NOT())
                _immediate.computeAssign(number->immediate, [](int x) -> int { return ! x; });

            auto ret = new NumberNode(_type, _immediate);
            PrintMsg("Exit");
            return (ExprNode*) ret;
        }
        UnaryOp _op;
        if(ctx->unaryOp()->ADD())
            _op = UOP_ADD;
        else if(ctx->unaryOp()->SUB())
            _op = UOP_SUB;
        else
            _op = UOP_NOT;
        AstType _type;
        if(_op == UOP_NOT)
            _type = INT;
        else
            _type = _unaryExpr->type;
        auto ret = new UnaryExprNode(_type, _op, std::move(_unaryExpr));
        PrintMsg("Exit");
        return (ExprNode*) ret;
    }
}
  
[[maybe_unused]] antlrcpp::Any SysYVisitor::visitUnaryOp(SysYParser::UnaryOpContext *ctx) {}
  
[[maybe_unused]] antlrcpp::Any SysYVisitor::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {}
  
/// \return return type is \p ExprNode*
antlrcpp::Any SysYVisitor::visitMulExp(SysYParser::MulExpContext *ctx)
{
    PrintMsg("visitMulExp");
    
    if(auto mulExp = ctx->mulExp())
    {
        auto unaryExp = ctx->unaryExp();
        auto lhs = unpackAny<ExprNode*>(visitMulExp(mulExp));
        auto _lhs = std::unique_ptr<ExprNode>(lhs);

        auto rhs = unpackAny<ExprNode*>(visitUnaryExp(unaryExp));
        auto _rhs = std::unique_ptr<ExprNode>(rhs);

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immediate;
            AstType _type = (_lhs->type) && (_rhs->type);

            if(ctx->MUL())
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                         [](auto x, auto y) -> auto { return x * y; });
            else if(ctx->DIV())
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                         [](auto x, auto y) -> auto { return x / y; });
            else
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                         [](int x, int y) -> int { return x % y; });
            
            auto ret = new NumberNode(_type, _immediate);
            PrintMsg("Exit");
            return (ExprNode*) ret;
        }

        BinaryExprNode* ret;
        auto _type = _lhs->type && _rhs->type;
        if(ctx->MUL())
            ret = new BinaryExprNode(_type, BOP_MUL, std::move(_lhs), std::move(_rhs));
        else if(ctx->DIV())
            ret = new BinaryExprNode(_type, BOP_DIV, std::move(_lhs), std::move(_rhs));
        else
            ret = new BinaryExprNode(_type, BOP_MOD, std::move(_lhs), std::move(_rhs));
        
        PrintMsg("Exit");
        return (ExprNode*) ret;
    }

    auto unaryExp = ctx->unaryExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitUnaryExp(unaryExp));

    PrintMsg("Exit");
    return _expr;
}

static int cnt = 0;
/// \return return type is \p ExprNode*
antlrcpp::Any SysYVisitor::visitAddExp(SysYParser::AddExpContext *ctx)
{
    PrintMsg("visitAddExp");

    if(auto addExp = ctx->addExp())
    {
        auto mulExp = ctx->mulExp();
        auto lhs = unpackAny<ExprNode*>(visitAddExp(addExp));
        auto _lhs = std::unique_ptr<ExprNode>(lhs);
        
        auto rhs = unpackAny<ExprNode*>(visitMulExp(mulExp));
        auto _rhs = std::unique_ptr<ExprNode>(rhs);

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immediate;
            AstType _type = _lhs->type && _rhs->type;

            if(ctx->ADD())
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                         [](auto x, auto y) -> auto { return x + y; });
            else
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                         [](auto x, auto y) -> auto { return x - y; });
            
            auto ret = new NumberNode(_type, _immediate);
            PrintMsg("Exit");
            return (ExprNode*) ret;
        }
        BinaryExprNode* ret;
        auto _type = _lhs->type && _rhs->type;
        if(ctx->ADD())
            ret = new BinaryExprNode(_type, BOP_ADD, std::move(_lhs), std::move(_rhs));
        else
            ret = new BinaryExprNode(_type, BOP_SUB, std::move(_lhs), std::move(_rhs));
        PrintMsg("Exit");
        return (ExprNode*) ret;
    }
    auto mulExp = ctx->mulExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitMulExp(mulExp));
    PrintMsg("Exit");
    return _expr;
}

/// \return return type is \p ExprNode* 
antlrcpp::Any SysYVisitor::visitRelExp(SysYParser::RelExpContext *ctx)
{
    PrintMsg("visitRelExp");
    
    if(auto relExp = ctx->relExp())
    {
        auto addExp = ctx->addExp();
        auto lhs = unpackAny<ExprNode*>(visitRelExp(relExp));
        auto _lhs = std::unique_ptr<ExprNode>(lhs);
        
        auto rhs = unpackAny<ExprNode*>(visitAddExp(addExp));
        auto _rhs = std::unique_ptr<ExprNode>(rhs);
        
        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT;

            if(ctx->LES())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                        [](auto x, auto y) -> auto { return x < y; });
            else if(ctx->GRT())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                        [](auto x, auto y) -> auto { return x > y; });
            else if(ctx->LEQ())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                        [](auto x, auto y) -> auto { return x <= y; });
            else
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                        [](auto x, auto y) -> auto { return x >= y; });

            auto ret = new NumberNode(_type, _immdiate);
            PrintMsg("Exit");
            return (ExprNode*) ret;
        }

        BinaryExprNode* ret;
        AstType type = _lhs->type && _rhs->type;
        if(ctx->LES())
            ret = new BinaryExprNode( type, ROP_LES, std::move(_lhs), std::move(_rhs));
        else if(ctx->GRT())
            ret = new BinaryExprNode( type, ROP_GRT, std::move(_lhs), std::move(_rhs));
        else if(ctx->LEQ())
            ret = new BinaryExprNode( type, ROP_LEQ, std::move(_lhs), std::move(_rhs));
        else
            ret = new BinaryExprNode( type, ROP_GEQ, std::move(_lhs), std::move(_rhs));
        
        PrintMsg("Exit");
        return (ExprNode*) ret;
    }
    auto addExp = ctx->addExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitAddExp(addExp));
    PrintMsg("Exit");
    return _expr;
}

/// \return return type is \p ExprNode*  
antlrcpp::Any SysYVisitor::visitEqExp(SysYParser::EqExpContext *ctx)
{
    PrintMsg("visitEqExp");
    
    if(auto eqExp = ctx->eqExp())
    {
        auto relExp = ctx->relExp();
        auto lhs = unpackAny<ExprNode*>(visitEqExp(eqExp));
        auto _lhs = std::unique_ptr<ExprNode>(lhs);

        auto rhs = unpackAny<ExprNode*>(visitRelExp(relExp));
        auto _rhs = std::unique_ptr<ExprNode>(rhs);

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT;

            if(ctx->EQL())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                        [](auto x, auto y) -> auto { return x == y; });
            else
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                        [](auto x, auto y) -> auto { return x != y; });

            auto ret = new NumberNode( _type, _immdiate);
            return (ExprNode*) ret;
        }

        BinaryExprNode* ret;
        AstType type = _lhs->type && _rhs->type;
        if(ctx->EQL())
            ret = new BinaryExprNode( type, ROP_EQL, std::move(_lhs), std::move(_rhs));
        else
            ret = new BinaryExprNode( type, ROP_NEQ, std::move(_lhs), std::move(_rhs));
        return (ExprNode*) ret;
    }

    auto relExp = ctx->relExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitRelExp(relExp));
    if(auto number = dynamic_cast<NumberNode*>(_expr); number 
        && number->immediate.getType() == FLOAT)
    {
        bool ans = number->immediate.getFloat() != 0;
        number->immediate = ans;
    }
    return _expr;
}

/// \return return type is \p ExprNode*
antlrcpp::Any SysYVisitor::visitLAndExp(SysYParser::LAndExpContext *ctx)
{
    PrintMsg("visitLAndExp");
    

    if(auto lAndExp = ctx->lAndExp())
    {
        auto eqExp = ctx->eqExp();

        auto lhs = unpackAny<ExprNode*>(visitLAndExp(lAndExp));
        auto _lhs = std::unique_ptr<ExprNode>(lhs);

        auto rhs = unpackAny<ExprNode*>(visitEqExp(eqExp));
        auto _rhs = std::unique_ptr<ExprNode>(rhs);

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT;
            _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                    dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                    [](int x, int y) -> int { return x && y; });

            auto ret = new NumberNode(_type, _immdiate);
            return (ExprNode*) ret;
        }
        // auto _type = _lhs->type && _rhs->type;
        auto ret = new BinaryExprNode(INT, BinaryOp::ROP_AND, std::move(_lhs), std::move(_rhs));
        ret->shortCircuit = true;
        return (ExprNode*) ret;
    }

    auto eqExp = ctx->eqExp();
    ExprNode* conditionExpr = unpackAny<ExprNode*>(visitEqExp(eqExp));
    return conditionExpr;
}

/// \return return type is \p ExprNode*  
antlrcpp::Any SysYVisitor::visitLOrExp(SysYParser::LOrExpContext *ctx)
{
    PrintMsg("visitLOrExp");
    
    if(auto lOrExp = ctx->lOrExp())
    {
        auto lAndExp = ctx->lAndExp();
        
        auto lhs = unpackAny<ExprNode*>(visitLOrExp(lOrExp));
        auto _lhs = std::unique_ptr<ExprNode>(lhs);

        auto rhs = unpackAny<ExprNode*>(visitLAndExp(lAndExp));
        auto _rhs = std::unique_ptr<ExprNode>(rhs);

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT; 
            _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs.get())->immediate, 
                                    dynamic_cast<NumberNode*>(_rhs.get())->immediate,
                                    [](int x, int y) -> int { return x || y; });

            auto ret = new NumberNode(_type, _immdiate);
            return (ExprNode*) ret;
        }
        // auto _type = _lhs->type && _rhs->type;
        auto ret = new BinaryExprNode( INT, BinaryOp::ROP_OR, std::move(_lhs), std::move(_rhs));
        ret->shortCircuit = true;
        return (ExprNode*) ret;
    }

    auto lAndExp = ctx->lAndExp();
    ExprNode* conditionExpr = unpackAny<ExprNode*>(visitLAndExp(lAndExp));
    return conditionExpr;
}

/// \return return type is \p ExprNode*  
antlrcpp::Any SysYVisitor::visitConstExp(SysYParser::ConstExpContext *ctx)
{
    PrintMsg("visitConstExp");
    auto addExp = ctx->addExp();
    auto ptr = unpackAny<ExprNode*>(visitAddExp(addExp));
    return ptr;
}

/// \return return type is \p int
antlrcpp::Any SysYVisitor::visitIntConst(SysYParser::IntConstContext *ctx)
{
    PrintMsg("visitIntConst");
    PrintMsg("Exit");
    if(ctx->DEC())
        return strDec(ctx->DEC()->toString().c_str());
    else if(ctx->HEX())
        return strHex(ctx->HEX()->toString().c_str());
    else if(ctx->OCT())
        return strOct(ctx->OCT()->toString().c_str());
    else
        ErrorMsg("Bad Ident");
}

/// \return return type is \p float
antlrcpp::Any SysYVisitor::visitFloatConst(SysYParser::FloatConstContext *ctx)
{
    PrintMsg("visitFloatConst");
    if(ctx->DECFLOAT())
        return strFloat(ctx->DECFLOAT()->toString());
    else
        return strFloatHex(ctx->HEXFLOAT()->toString());
}

} // namespace sysy