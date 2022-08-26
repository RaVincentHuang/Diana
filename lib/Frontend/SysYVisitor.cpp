#include "sysy/Frontend/SysYVisitor.h"
#include "sysy/Frontend/SysYParser.h"
#include "sysy/Frontend/Ast.h"
#include "sysy/Frontend/Semantics.h"
#include "sysy/Frontend/AstTramsform.h"
#include "sysy/Support/common.h"
#include "sysy/Support/strUtils.h"
#include "sysy/Support/debug.h"

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

antlrcpp::Any SysYVisitor::visitCompUnit(SysYParser::CompUnitContext *ctx)
{
    // PrintMsg("visitCompUnit");

    std::vector<DeclNode*> _decl;
    std::vector<FuncDefNode*> _funcDef;
    std::vector<AstNode*> _element;

    auto& scope = AstScope::getCurrentPtr();
    for(auto [name, type] : storedRuntime)
        scope->insertVariable(name, type);

    for(auto item : ctx->element())
    {
        if(auto decl = item->decl())
        {
            DeclNode* ele = unpackAny<DeclNode*>(visitDecl(decl));
            _element.push_back(ele);
        }
        else if(auto funcDef = item->funcDef())
        {
            FuncDefNode* ele = unpackAny<FuncDefNode*>(visitFuncDef(funcDef));
            _element.push_back(ele);
        }
    }
    CompUnitNode* ast = new CompUnitNode(_element);
    return ast;
}

antlrcpp::Any SysYVisitor::visitElement(SysYParser::ElementContext *ctx) {}

antlrcpp::Any SysYVisitor::visitDecl(SysYParser::DeclContext *ctx)
{
    // PrintMsg("visitDecl");
    if(auto constDecl = ctx->constDecl())
        return (DeclNode*) unpackAny<ConstDeclNode*>(visitConstDecl(constDecl));
    if(auto varDecl = ctx->varDecl())
        return (DeclNode*) unpackAny<VarDeclNode*>(visitVarDecl(varDecl));
}

static AstType constDeclType;
antlrcpp::Any SysYVisitor::visitConstDecl(SysYParser::ConstDeclContext *ctx)
{
    // PrintMsg("visitConstDecl");
    
    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if (ctx->bType()->FLOAT())
        _type = FLOAT;
    
    std::vector<ConstDefNode*> _constDef;
    constDeclType = _type;
    for(auto constDef : ctx->constDef())
        _constDef.push_back(unpackAny<ConstDefNode*>(visitConstDef(constDef)));
    
    // auto& scope = AstScope::getCurrentPtr();
    // for(auto def : _constDef)
    // {
    //     scope->insertVariable(def->name, _type);
    //     if(!def->dimSize.empty())
    //         scope->insertDim(def->name, def->dimSize);
    // }
    ConstDeclNode* ret = new ConstDeclNode( _type, _constDef);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitBType(SysYParser::BTypeContext *ctx){}

antlrcpp::Any SysYVisitor::visitConstDef(SysYParser::ConstDefContext *ctx)
{
    // PrintMsg("visitConstDef");
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
    
    InitValNode* _constInitVal = nullptr;
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
        _constInitVal = unpackAny<InitValNode*>(visitConstInitVal(constInitVal));
        dims.clear();
    }
    
    scope->setCosnt(_name);
    if(_constInitVal)
    {
        for(auto item : _constInitVal->initItem)
            if(auto number = dynamic_cast<NumberNode*>(item))
            {
                auto& imm = number->immediate;
                imm.typeCast(constDeclType); 
            }
        scope->insertInitial(_name, _constInitVal);
    }
    
    ConstDefNode* ret = new ConstDefNode( _dimSize, _constInitVal, _name);
    return ret;
}

// FIXME Change as the better way
std::vector<ExprNode*> SysYVisitor::constInitValDfs(SysYParser::ConstInitValContext *ctx, int idx)
{
    // PrintMsg("constInitValDfs");
    
    std::vector<ExprNode*> res;
    if(auto cosntExp = ctx->constExp())
    {
        res.push_back(unpackAny<ExprNode*>(visitConstExp(cosntExp)));
        return res;
    }
    for(auto constInitVal : ctx->constInitVal())
    {
        auto vec = constInitValDfs(constInitVal, idx + 1);
        res.insert(res.end(), vec.begin(), vec.end());
    }
    auto dim = idx ? dims.at(dims.size() - depth + idx) : dims.at(idx);
    if(!idx && res.empty() && dim > 40)
        return {};
    if(auto cle = dim - res.size(); cle > 0)
        res.insert(res.end(), cle, new NumberNode(INT, 0));
    return res;
}
  
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
    std::vector<ExprNode*> _initItem = constInitValDfs(ctx, 0);

    InitValNode* init = new InitValNode( _initItem);
    return init;
}
  
antlrcpp::Any SysYVisitor::visitVarDecl(SysYParser::VarDeclContext *ctx)
{
    // PrintMsg("visitVarDecl");

    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if(ctx->bType()->FLOAT())
        _type = FLOAT;
// TODO Add the other type    
    std::vector<VarDefNode*> _varDef;
    for(auto varDef : ctx->varDef())
        _varDef.push_back(unpackAny<VarDefNode*>(visitVarDef(varDef)));

    auto& scope = AstScope::getCurrentPtr();
    for(auto def : _varDef)
    {
        scope->insertVariable(def->name, _type);
        if(!def->dimSize.empty())
            scope->insertDim(def->name, def->dimSize);
    }
    
    
    VarDeclNode* ret = new VarDeclNode( _type, _varDef);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitVarDecl_in(SysYParser::VarDecl_inContext *ctx)
{
    // PrintMsg("visitVarDecl_in");
    

    AstType _type;
    if(ctx->bType()->INT())
        _type = INT;
    else if(ctx->bType()->FLOAT())
        _type = FLOAT;
// TODO Add the other type    
    std::vector<VarDefNode*> _varDef;
    for(auto varDef : ctx->varDef())
        _varDef.push_back(unpackAny<VarDefNode*>(visitVarDef(varDef)));
    
    auto& scope = AstScope::getCurrentPtr();
    for(auto def : _varDef)
    {
        scope->insertVariable(def->name, _type);
        if(!def->dimSize.empty())
            scope->insertDim(def->name, def->dimSize);
    }
    
    VarDeclNode* ret = new VarDeclNode( _type, _varDef);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitVarDef(SysYParser::VarDefContext *ctx)
{
    // PrintMsg("visitVarDef");

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
        // PrintMsg(_name);
        int size = 1, cnt = 0;
        for(auto s : _dimSize)
            size *= s;
        dims.clear();
        while (cnt < _dimSize.size())
        {
            dims.push_back(size);
            size /= _dimSize[cnt++];
        }
        InitValNode* _varInitVal = unpackAny<InitValNode*>(visitInitVal(initVal));
        dims.clear();
        VarDefNode* ret = new VarDefNode( _dimSize, _varInitVal, _name);
        return ret;
    }

    VarDefNode* ret = new VarDefNode( _dimSize, _name);
    return ret;
}

// TODO change as the better way
std::vector<ExprNode*> SysYVisitor::varInitValDfs(SysYParser::InitValContext *ctx, int idx)
{
    // PrintMsg("varInitValDfs");
    std::vector<ExprNode*> res;
    if(auto exp = ctx->exp())
    {
        res.push_back(unpackAny<ExprNode*>(visitExp(exp)));

        return res;
    }
    for(auto initVal : ctx->initVal())
    {
        auto vec = varInitValDfs(initVal, idx + 1);
        res.insert(res.end(), vec.begin(), vec.end());
    }
    auto dim = idx ? dims.at(dims.size() - depth + idx) : dims.at(idx);
    if(!idx && res.empty() && dim > 40)
        return {};
    if(auto cle = dim - res.size(); cle > 0)
        res.insert(res.end(), cle, new NumberNode( INT, 0));
    return res;
}

antlrcpp::Any SysYVisitor::visitInitVal(SysYParser::InitValContext *ctx)
{
    // PrintMsg("visitInitVal");
    std::function<int(SysYParser::InitValContext*)> dfs = [&](SysYParser::InitValContext* ctx) -> int {
        if(ctx->exp())
            return 0;
        int maxn = 1;
        for(auto init : ctx->initVal())
            maxn = std::max(maxn, dfs(init) + 1);
        return maxn;
    };

    depth = dfs(ctx);

    std::vector<ExprNode*> _initItem = varInitValDfs(ctx, 0);

    InitValNode* init = new InitValNode( _initItem);
    return init;
}
  
antlrcpp::Any SysYVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx)
{
    // PrintMsg("visitFuncDef");
    

    AstType _funcType;
    if(ctx->funcType()->INT())
        _funcType = INT;
    else if(ctx->funcType()->VOID())
        _funcType = VOID;
    else if(ctx->funcType()->FLOAT())
        _funcType = FLOAT;
// TODO Add the other type        
    std::string _funcName = ctx->Ident()->toString();

    FuncParamListNode* _funcParamList;
    if(auto funcFParams = ctx->funcFParams())
        _funcParamList = unpackAny<FuncParamListNode*>(visitFuncFParams(funcFParams));
    else
        _funcParamList = nullptr;

    auto& scope = AstScope::getCurrentPtr();

    scope->insertVariable(_funcName, _funcType);
    
    scope = scope->createChild();

    if(_funcParamList)
        for(auto arg : _funcParamList->funcParam)
        {
            scope->insertVariable(arg->varName, arg->type);
            if(!arg->dimSize.empty())
                scope->insertDim(arg->varName, arg->dimSize);
        }
    auto block  = ctx->block(); 
    BlockNode* _block = unpackAny<BlockNode*>(visitBlock(block));
    FuncDefNode* ret = new FuncDefNode( _funcType, _funcName, _funcParamList, _block);
    scope = AstScope::exitBlock(scope);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitFuncType(SysYParser::FuncTypeContext *ctx){}
  
antlrcpp::Any SysYVisitor::visitFuncFParams(SysYParser::FuncFParamsContext *ctx)
{
    // PrintMsg("visitFuncFParams");
    

    std::vector<FuncParamNode*> _funcParam;
    for(auto funcFParam : ctx->funcFParam())
        _funcParam.push_back(unpackAny<FuncParamNode*>(visitFuncFParam(funcFParam)));
    
    FuncParamListNode* ret = new FuncParamListNode( _funcParam);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitFuncFParam(SysYParser::FuncFParamContext *ctx)
{
    // PrintMsg("visitFuncFParam");
    

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
    
    FuncParamNode* ret = new FuncParamNode( _type, _varName, _dimSize);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitBlock(SysYParser::BlockContext *ctx)
{
    // PrintMsg("visitBlock");
    

    auto& scope = AstScope::getCurrentPtr();
    scope = scope->createChild();

    std::vector<StmtNode*> _blockItems;
    for(auto blockItem : ctx->blockItem())
        if(auto item = unpackAny<StmtNode*>(visitBlockItem(blockItem)))
            _blockItems.push_back(item);

    scope = AstScope::exitBlock(scope);
    
    BlockNode* ret = new BlockNode( _blockItems);
    ast::removeDeadStmts(ret);
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitBlockItem(SysYParser::BlockItemContext *ctx)
{
    // PrintMsg("visitBlockItem");
    if(auto decl = ctx->decl())
        return (StmtNode*) unpackAny<DeclNode*>(visitDecl(decl));
    else if(auto stmt = ctx->stmt())
        return unpackAny<StmtNode*>(visitStmt(stmt));
    else
        return nullptr;
}

antlrcpp::Any SysYVisitor::visitStmt_in(SysYParser::Stmt_inContext *ctx)
{
    // PrintMsg("visitStmt_in");
    

    if(auto lval = ctx->lVal())
    {
        LvalNode* _lval = unpackAny<LvalNode*>(visitLVal(lval));

        auto expr = ctx->exp();
        ExprNode* _expr = unpackAny<ExprNode*>(visitExp(expr));

        AssignStmtNode* ret = new AssignStmtNode( _lval, _expr);
        return (StmtNode*) ret;
    }
    else if(auto exp = ctx->exp())
    {
        ExprNode* _expr = unpackAny<ExprNode*>(visitExp(exp));

        ExprStmtNode* ret = new ExprStmtNode( _expr);
        return (StmtNode*) ret;
    }
    return nullptr;
}

  
antlrcpp::Any SysYVisitor::visitStmt(SysYParser::StmtContext *ctx)
{
    // PrintMsg("visitStmt");
    

    if(ctx->lVal() && ctx->AGN() && ctx->exp())
    {
        auto lval = ctx->lVal();
        LvalNode* _lval = unpackAny<LvalNode*>(visitLVal(lval));

        auto expr = ctx->exp();
        ExprNode* _expr = unpackAny<ExprNode*>(visitExp(expr));

        AssignStmtNode* ret = new AssignStmtNode( _lval, _expr);
        return (StmtNode*) ret;
    }
    else if(ctx->RETURN())
    {
        if(auto exp = ctx->exp())
        {
            ExprNode* _expr = unpackAny<ExprNode*>(visitExp(exp));

            ReturnStmtNode* ret = new ReturnStmtNode( _expr);
            return (StmtNode*) ret;
        }
        ReturnStmtNode* ret = new ReturnStmtNode;
        return (StmtNode*) ret;
    }
    else if(auto exp = ctx->exp())
    {
        ExprNode* _expr = unpackAny<ExprNode*>(visitExp(exp));

        ExprStmtNode* ret = new ExprStmtNode( _expr);
        return (StmtNode*) ret;
    }
    else if(auto block = ctx->block())
        return (StmtNode*) unpackAny<BlockNode*>(visitBlock(block));
    else if(ctx->IF())
    {
        auto cond = ctx->cond();
        ExprNode* _expr = unpackAny<ExprNode*>(visitCond(cond));

        auto thenBlock = ctx->stmt().at(0);
        StmtNode* _thenBlock = unpackAny<StmtNode*>(visitStmt(thenBlock));

        StmtNode* _elseBlock;
        if(ctx->stmt().size() == 2)
        {
            auto elseBlock = ctx->stmt().at(1);
            _elseBlock = unpackAny<StmtNode*>(visitStmt(elseBlock));
        }
        else
            _elseBlock = nullptr;

        IfStmtNode* ret = new IfStmtNode( _expr, _thenBlock, _elseBlock);
        return (StmtNode*) ret;
    }
    else if(ctx->FOR())
    {

        auto cond = ctx->cond();
        ExprNode* _expr = unpackAny<ExprNode*>(visitCond(cond));

        auto stmt = ctx->stmt().at(0);
        StmtNode* _stmt = unpackAny<StmtNode*>(visitStmt(stmt));

        StmtNode* _initial;
        StmtNode* _increment;
        if(auto initial = ctx->varDecl_in())
        {
            _initial = (StmtNode*) unpackAny<VarDeclNode*>(visitVarDecl_in(initial));
            
            auto increment = ctx->stmt_in().at(0);
            _increment = unpackAny<StmtNode*>(visitStmt_in(increment));
        }
        else
        {
            auto initial_ = ctx->stmt_in().at(0);
            _initial = unpackAny<StmtNode*>(visitStmt_in(initial_));

            auto increment = ctx->stmt_in().at(1);
            _increment = unpackAny<StmtNode*>(visitStmt_in(increment));
        }

        ForStmtNode* ret = new ForStmtNode( _initial, _expr, _increment, _stmt);
        return (StmtNode*) ret;
    }
    else if(ctx->WHILE())
    {
        auto cond = ctx->cond();
        ExprNode* _expr = unpackAny<ExprNode*>(visitCond(cond));

        auto stmt = ctx->stmt().at(0);
        StmtNode* _stmt = unpackAny<StmtNode*>(visitStmt(stmt));

        WhileStmtNode* ret = new WhileStmtNode( _expr, _stmt);
        return (StmtNode*) ret;
    }
    else if(ctx->CONTINUE())
    {
        ContinueStmtNode* ret = new ContinueStmtNode( true);
        return (StmtNode*) ret;
    }
    else if(ctx->BREAK())
    {
        BreakStmtNode* ret = new BreakStmtNode( true);
        return (StmtNode*) ret;
    }
    else
        return nullptr;
}

// Expr
antlrcpp::Any SysYVisitor::visitExp(SysYParser::ExpContext *ctx)
{
    // PrintMsg("visitExp");
    auto addExp = ctx->addExp();
    auto expr = unpackAny<ExprNode*>(visitAddExp(addExp));
    // PrintMsg("Exit");
    return expr;
}

antlrcpp::Any SysYVisitor::visitCond(SysYParser::CondContext *ctx)
{
    // PrintMsg("visitCond");
    auto lOrExp = ctx->lOrExp();
    ExprNode* expr = unpackAny<ExprNode*>(visitLOrExp(lOrExp));
    return expr;
}
  
antlrcpp::Any SysYVisitor::visitLVal(SysYParser::LValContext *ctx)
{
    // PrintMsg("visitLVal");
    
    std::string _variable = ctx->Ident()->toString();
    std::vector<ExprNode*> _expr;
    for(auto exp : ctx->exp())
        _expr.push_back(unpackAny<ExprNode*>(visitExp(exp)));
        
    
    auto& scope = AstScope::getCurrentPtr();
    AstType _type = scope->getType(_variable);
    LvalNode* ret;
    if(!_expr.empty())
        ret = new LvalNode( _type, _variable, _expr, scope->getDim(_variable));
    else
        ret = new LvalNode( _type, _variable, _expr);
    // PrintMsg("Exit");
    return ret;
}
  
antlrcpp::Any SysYVisitor::visitPrimaryExp(SysYParser::PrimaryExpContext *ctx)
{
    // PrintMsg("visitPrimaryExp");
    

    if(auto exp = ctx->exp())
    {
        ExprNode* _expr = unpackAny<ExprNode*>(visitExp(exp));
        // PrintMsg("Exit");
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
                    return (ExprNode*) initial->initItem.at(0);
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
                auto number = initial->initItem.at(index);
                return (ExprNode*) number;
            }
            else
            {
                if(_type == INT)
                    _immediate = 0;
                else
                    _immediate = 0.0f;
                return (ExprNode*) new NumberNode( _type, _immediate);
            }
        }
LVAL:
        LvalNode* _lval = unpackAny<LvalNode*>(visitLVal(lval));
        // PrintMsg("Exit");
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
        NumberNode* ret = new NumberNode( _type, _immediate);
        // PrintMsg("Exit");
        return (ExprNode*) ret;
    }
}
  
antlrcpp::Any SysYVisitor::visitNumber(SysYParser::NumberContext *ctx){}
  
antlrcpp::Any SysYVisitor::visitUnaryExp(SysYParser::UnaryExpContext *ctx)
{
    // PrintMsg("visitUnaryExp");
    

    if(auto primaryExp = ctx->primaryExp())
    {
        ExprNode* _primaryExpr = unpackAny<ExprNode*>(visitPrimaryExp(primaryExp));
        // PrintMsg("Exit");
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
        std::vector<ExprNode*> _funcParamList;
        if(auto funcRParams = ctx->funcRParams())
            for(auto exp : funcRParams->exp())
                _funcParamList.push_back(unpackAny<ExprNode*>(visitExp(exp)));

        auto& scope = AstScope::getCurrentPtr();
        auto _type = scope->getType(_funcName);
        
        FunctionCallExprNode* ret = new FunctionCallExprNode( _type, _funcName, _funcParamList);
        // FIXME  
        if(ctx->funcRParams() && !ctx->funcRParams()->STRING().empty())
        {
            // std::cout << ctx->funcRParams()->STRING().at(0)->toString() << std::endl;
            ret->format = ctx->funcRParams()->STRING().at(0)->toString();
        }
        // PrintMsg("Exit");
        return (ExprNode*) ret;
    }
    else
    {
        auto unaryExp = ctx->unaryExp();
        ExprNode* _unaryExpr = unpackAny<ExprNode*>(visitUnaryExp(unaryExp));

        if(_unaryExpr->ready())
        {
            // TODO Add the other type    
            auto number = dynamic_cast<NumberNode*>(_unaryExpr);
            Immediate _immediate;
            AstType _type = _unaryExpr->type;
            if(ctx->unaryOp()->ADD())
                _immediate.computeAssign(number->immediate, [](auto x) -> auto { return + x; });
            else if(ctx->unaryOp()->SUB())
                _immediate.computeAssign(number->immediate, [](auto x) -> auto { return - x; });
            else if(ctx->unaryOp()->NOT())
                _immediate.computeAssign(number->immediate, [](int x) -> int { return ! x; });

            NumberNode* ret = new NumberNode( _type, _immediate);
            // PrintMsg("Exit");
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
        UnaryExprNode* ret = new UnaryExprNode( _type, _op, _unaryExpr);
        // PrintMsg("Exit");
        return (ExprNode*) ret;
    }
}
  
antlrcpp::Any SysYVisitor::visitUnaryOp(SysYParser::UnaryOpContext *ctx) {}
  
antlrcpp::Any SysYVisitor::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {}
  
antlrcpp::Any SysYVisitor::visitMulExp(SysYParser::MulExpContext *ctx)
{
    // PrintMsg("visitMulExp");
    

    if(auto mulExp = ctx->mulExp())
    {
        auto unaryExp = ctx->unaryExp();
        ExprNode* _lhs = unpackAny<ExprNode*>(visitMulExp(mulExp));

        ExprNode* _rhs = unpackAny<ExprNode*>(visitUnaryExp(unaryExp));

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immediate;
            AstType _type = (_lhs->type) && (_rhs->type);

            if(ctx->MUL())
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs)->immediate,
                                         [](auto x, auto y) -> auto { return x * y; });
            else if(ctx->DIV())
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs)->immediate,
                                         [](auto x, auto y) -> auto { return x / y; });
            else
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs)->immediate,
                                         [](int x, int y) -> int { return x % y; });
            
            NumberNode* ret = new NumberNode( _type, _immediate);
            // PrintMsg("Exit");
            return (ExprNode*) ret;
        }

        BinaryExprNode* ret;
        auto _type = _lhs->type && _rhs->type;
        if(ctx->MUL())
            ret = new BinaryExprNode( _type, BOP_MUL, _lhs, _rhs);
        else if(ctx->DIV())
            ret = new BinaryExprNode( _type, BOP_DIV, _lhs, _rhs);
        else
            ret = new BinaryExprNode( _type, BOP_MOD, _lhs, _rhs);
        
        // PrintMsg("Exit");
        return (ExprNode*) ret;
    }

    auto unaryExp = ctx->unaryExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitUnaryExp(unaryExp));

    // PrintMsg("Exit");
    return _expr;
}
static int cnt = 0;
antlrcpp::Any SysYVisitor::visitAddExp(SysYParser::AddExpContext *ctx)
{
    // PrintMsg("visitAddExp");
    

    if(auto addExp = ctx->addExp())
    {
        auto mulExp = ctx->mulExp();
        ExprNode* _lhs = unpackAny<ExprNode*>(visitAddExp(addExp));
        
        ExprNode* _rhs = unpackAny<ExprNode*>(visitMulExp(mulExp));

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immediate;
            AstType _type = _lhs->type && _rhs->type;

            if(ctx->ADD())
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs)->immediate,
                                         [](auto x, auto y) -> auto { return x + y; });
            else
                _immediate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                         dynamic_cast<NumberNode*>(_rhs)->immediate,
                                         [](auto x, auto y) -> auto { return x - y; });
            
            NumberNode* ret = new NumberNode( _type, _immediate);
            // PrintMsg("Exit");
            return (ExprNode*) ret;
        }
        BinaryExprNode* ret;
        auto _type = _lhs->type && _rhs->type;
        if(ctx->ADD())
            ret = new BinaryExprNode( _type, BOP_ADD, _lhs, _rhs);
        else
            ret = new BinaryExprNode( _type, BOP_SUB, _lhs, _rhs);
        // PrintMsg("Exit");
        return (ExprNode*) ret;
    }
    auto mulExp = ctx->mulExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitMulExp(mulExp));
    // PrintMsg("Exit");
    return _expr;
}
  
antlrcpp::Any SysYVisitor::visitRelExp(SysYParser::RelExpContext *ctx)
{
    // PrintMsg("visitRelExp");
    

    if(auto relExp = ctx->relExp())
    {
        auto addExp = ctx->addExp();
        ExprNode* _lhs = unpackAny<ExprNode*>(visitRelExp(relExp));
        
        ExprNode* _rhs = unpackAny<ExprNode*>(visitAddExp(addExp));
        
        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT;

            if(ctx->LES())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs)->immediate,
                                        [](auto x, auto y) -> auto { return x < y; });
            else if(ctx->GRT())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs)->immediate,
                                        [](auto x, auto y) -> auto { return x > y; });
            else if(ctx->LEQ())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs)->immediate,
                                        [](auto x, auto y) -> auto { return x <= y; });
            else
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs)->immediate,
                                        [](auto x, auto y) -> auto { return x >= y; });

            NumberNode* ret = new NumberNode( _type, _immdiate);
            // PrintMsg("Exit");
            return (ExprNode*) ret;
        }

        BinaryExprNode* ret;
        AstType type = _lhs->type && _rhs->type;
        if(ctx->LES())
            ret = new BinaryExprNode( type, ROP_LES, _lhs, _rhs);
        else if(ctx->GRT())
            ret = new BinaryExprNode( type, ROP_GRT, _lhs, _rhs);
        else if(ctx->LEQ())
            ret = new BinaryExprNode( type, ROP_LEQ, _lhs, _rhs);
        else
            ret = new BinaryExprNode( type, ROP_GEQ, _lhs, _rhs);
        
        // PrintMsg("Exit");
        return (ExprNode*) ret;
    }
    auto addExp = ctx->addExp();
    ExprNode* _expr = unpackAny<ExprNode*>(visitAddExp(addExp));
    // PrintMsg("Exit");
    return _expr;
}
  
antlrcpp::Any SysYVisitor::visitEqExp(SysYParser::EqExpContext *ctx)
{
    // PrintMsg("visitEqExp");
    

    if(auto eqExp = ctx->eqExp())
    {
        auto relExp = ctx->relExp();
        ExprNode* _lhs = unpackAny<ExprNode*>(visitEqExp(eqExp));

        ExprNode* _rhs = unpackAny<ExprNode*>(visitRelExp(relExp));

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT;

            if(ctx->EQL())
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs)->immediate,
                                        [](auto x, auto y) -> auto { return x == y; });
            else
                _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                        dynamic_cast<NumberNode*>(_rhs)->immediate,
                                        [](auto x, auto y) -> auto { return x != y; });

            NumberNode* ret = new NumberNode( _type, _immdiate);
            return (ExprNode*) ret;
        }

        BinaryExprNode* ret;
        AstType type = _lhs->type && _rhs->type;
        if(ctx->EQL())
            ret = new BinaryExprNode( type, ROP_EQL, _lhs, _rhs);
        else
            ret = new BinaryExprNode( type, ROP_NEQ, _lhs, _rhs);
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
  
antlrcpp::Any SysYVisitor::visitLAndExp(SysYParser::LAndExpContext *ctx)
{
    // PrintMsg("visitLAndExp");
    

    if(auto lAndExp = ctx->lAndExp())
    {
        auto eqExp = ctx->eqExp();

        ExprNode* _lhs = unpackAny<ExprNode*>(visitLAndExp(lAndExp));

        ExprNode* _rhs = unpackAny<ExprNode*>(visitEqExp(eqExp));

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT;
            _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                    dynamic_cast<NumberNode*>(_rhs)->immediate,
                                    [](int x, int y) -> int { return x && y; });

            NumberNode* ret = new NumberNode( _type, _immdiate);
            return (ExprNode*) ret;
        }
        // auto _type = _lhs->type && _rhs->type;
        BinaryExprNode* ret = new BinaryExprNode( INT, BinaryOp::ROP_AND, _lhs, _rhs);
        ret->shortCircuit = true;
        return (ExprNode*) ret;
    }

    auto eqExp = ctx->eqExp();
    ExprNode* conditionExpr = unpackAny<ExprNode*>(visitEqExp(eqExp));
    return conditionExpr;
}
  
antlrcpp::Any SysYVisitor::visitLOrExp(SysYParser::LOrExpContext *ctx)
{
    // PrintMsg("visitLOrExp");
    

    if(auto lOrExp = ctx->lOrExp())
    {
        auto lAndExp = ctx->lAndExp();
        
        ExprNode* _lhs = unpackAny<ExprNode*>(visitLOrExp(lOrExp));

        ExprNode* _rhs = unpackAny<ExprNode*>(visitLAndExp(lAndExp));

        if(_lhs->ready() && _rhs->ready())
        {
            // TODO Add the other type    
            Immediate _immdiate;
            AstType _type = INT; 
            _immdiate.computeAssign(dynamic_cast<NumberNode*>(_lhs)->immediate, 
                                    dynamic_cast<NumberNode*>(_rhs)->immediate,
                                    [](int x, int y) -> int { return x || y; });

            NumberNode* ret = new NumberNode( _type, _immdiate);
            return (ExprNode*) ret;
        }
        // auto _type = _lhs->type && _rhs->type;
        BinaryExprNode* ret = new BinaryExprNode( INT, BinaryOp::ROP_OR, _lhs, _rhs);
        ret->shortCircuit = true;
        return (ExprNode*) ret;
    }

    auto lAndExp = ctx->lAndExp();
    ExprNode* conditionExpr = unpackAny<ExprNode*>(visitLAndExp(lAndExp));
    return conditionExpr;
}
  
antlrcpp::Any SysYVisitor::visitConstExp(SysYParser::ConstExpContext *ctx)
{
    // PrintMsg("visitConstExp");
    auto addExp = ctx->addExp();
    auto ptr = unpackAny<ExprNode*>(visitAddExp(addExp));
    return ptr;
}
  
antlrcpp::Any SysYVisitor::visitIntConst(SysYParser::IntConstContext *ctx)
{
    // PrintMsg("visitIntConst");
    // PrintMsg("Exit");
    if(ctx->DEC())
        return strDec(ctx->DEC()->toString().c_str());
    else if(ctx->HEX())
        return strHex(ctx->HEX()->toString().c_str());
    else if(ctx->OCT())
        return strOct(ctx->OCT()->toString().c_str());
    else
        ErrorMsg("Bad Ident");
}

antlrcpp::Any SysYVisitor::visitFloatConst(SysYParser::FloatConstContext *ctx)
{
    // PrintMsg("visitFloatConst");
    if(ctx->DECFLOAT())
        return strFloat(ctx->DECFLOAT()->toString());
    else
        return strFloatHex(ctx->HEXFLOAT()->toString());
}

} // namespace sysy