#include "sysy/Frontend/Ast.h"
#include "sysy/IR/IRGen.h"
#include "sysy/IR/Builder.h"
#include "sysy/IR/ValueScope.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/Module.h"
#include "sysy/IR/IRDump.h"
#include "sysy/Transform/Utils/SimplifyCFG.h"
#include "sysy/Support/TypeUtils.h"
#include "sysy/Support/Debug.h"

namespace sysy
{
namespace ir
{

using namespace ast;

// TODO Add to the project global. 
static IRBuilder Builder;
// static Module* TheModule;
// static legacy::FunctionPassManager* funcPassMgr;
static std::vector<Function*> InitFuncList;

#define FUNC_NUM 18
struct 
{
    Type* retType;
    std::vector<Type*> argType;
    std::string funcName;
    bool isVarArg;
    
} funcInclude[FUNC_NUM] = {
    { Type::getInt32Ty(), {}, "getint", false },
    { Type::getInt32Ty(), {}, "getch", false },
    { Type::getFloatTy(), {}, "getfloat", false },
    { Type::getInt32Ty(), { Type::getInt32PtrTy() }, "getarray", false },
    { Type::getInt32Ty(), { Type::getFloatPtrTy() }, "getfarray", false },
    { Type::getVoidTy(), { Type::getInt32Ty() }, "putint", false },
    { Type::getVoidTy(), { Type::getInt32Ty() }, "putch", false },
    { Type::getVoidTy(), { Type::getFloatTy() }, "putfloat", false },
    { Type::getVoidTy(), { Type::getInt32Ty(), Type::getInt32PtrTy() }, "putarray", false },
    { Type::getVoidTy(), { Type::getInt32Ty(), Type::getFloatPtrTy() }, "putfarray", false },
    { Type::getVoidTy(), { Type::getInt8PtrTy()}, "putf", true },
    { Type::getVoidTy(), {}, "starttime", false },
    { Type::getVoidTy(), {}, "stoptime", false },
    { Type::getVoidTy(), { Type::getInt8PtrTy(), Type::getInt8PtrTy(), Type::getInt32Ty(), Type::getInt1Ty()}, "llvm.memcpy.inline.p0.p0.i32", false},
    { Type::getVoidTy(), { Type::getInt32PtrTy(), Type::getInt32Ty(), Type::getInt32Ty()}, "memset", false},
    { Type::getVoidTy(), { Type::getInt32PtrTy(), Type::getInt32PtrTy(), Type::getInt32Ty()}, "memcpy", false },
    { Type::getInt32Ty(), {}, "__thread_create", false },
    { Type::getVoidTy(), { Type::getInt32Ty() }, "__thread_join", false }
};

static void initRuntime()
{
    for(auto func : funcInclude)
    {
        FunctionType* FT = FunctionType::get(func.retType, func.argType, func.isVarArg);
        Function* F = Function::Create(FT, func.funcName);
    }
}

void getIDX(int index, const std::vector<int>& dims, std::vector<Value*>& IDX)
{
    IDX.push_back(ConstantInt::get(Type::getInt32Ty(), 0));
    int scale = 1;
    for(auto size : dims)
        scale *= size;
    for(auto size : dims)
    {
        scale /= size;
        IDX.push_back(ConstantInt::get(Type::getInt32Ty(), index / scale));
        index %= scale;
    }
}

void emitInitial(Value* gpr, std::vector<std::unique_ptr<ExprNode>>& initial, ArrayType* type)
{
    std::vector<int> dims;
    Type* _type = type;
    int cnt = 0;
    while(auto type = dynamic_cast<ArrayType*>(_type))
    {
        dims.push_back(type->getSize());
        _type = type->getInnerType();
        cnt++; 
    }
    
    if(initial.empty())
    {
        std::vector<Value*> IDX;
        IDX.push_back(ConstantInt::get(32, 0));
        for(auto i = 0; i < cnt; i++)
            IDX.push_back(ConstantInt::get(32, 0));
        Value* ptr = Builder.CreateInBoundsGEP(gpr->getType()->getInnerType(), 
                                gpr, IDX, "ptrtomemset");
        auto memset = Module::getInstance().getFunction("memset");
        std::vector<Value*> args = { ptr, ConstantInt::get(32, 0), 
                        ConstantInt::get(32, type->getAllocatedSize()) };
        Builder.CreateCall(memset, args);
        return;
    }

    for(int index = 0; index < initial.size(); index++)
    {
        std::vector<Value*> IDX;
        getIDX(index, dims, IDX);
        Value* val = irEmit(initial.at(index).get());
        Value* _gpr = Builder.CreateInBoundsGEP(gpr->getType()->getInnerType(), 
                                gpr, IDX, "initial");
        if(!isSameType(val->getType(), _type))
        {
            if(val->getType()->isIntegerTy())
                val = Builder.CreateSIToFP(val, _type, "int2float");
            else
                val = Builder.CreateFPToSI(val, _type, "float2int");
        }
        Builder.CreateStore(val, _gpr);
    }
}

Function* createGlobalInit()
{
    FunctionType* FT = FunctionType::get(Type::getVoidTy(), {}, false);
    std::string name = "__sysy_global_var_initial";
    Function* F = Function::Create(FT, name);
    BasicBlock* block = BasicBlock::Create("entry", F);
    Builder.SetInsertPoint(block);
    for(auto func : InitFuncList)
        Builder.CreateCall(func);
    Builder.CreateRetVoid();
    return F;
}

Function* createGlobalInit(GlobalValue* G, std::vector<std::unique_ptr<ExprNode>>& initial)
{
    FunctionType* FT = FunctionType::get(Type::getVoidTy(), {}, false);

    std::string name = "__sysy_global_array_initial." + G->getName();

    Function* F = Function::Create(FT, name);

    BasicBlock* block = BasicBlock::Create("entry", F);
    Builder.SetInsertPoint(block);

    Value* gpr = Builder.CreateInBoundsGEP(G->getValueType(), G, 
            { ConstantInt::get(Type::getInt32Ty(), 0) }, "gpr");
    if(!G->getValueType()->isArrayTy())
    {
        Value* val = irEmit((*initial.begin()).get());
        if(!isSameType(val->getType(), G->getValueType()))
        {
            if(val->getType()->isIntegerTy())
                val = Builder.CreateSIToFP(val, G->getValueType(), "int2float");
            else
                val = Builder.CreateFPToSI(val, G->getValueType(), "float2int");
        }
        Builder.CreateStore(val, G);
    }
    else
        emitInitial(G, initial, dynamic_cast<ArrayType*>(G->getValueType()));
    Builder.CreateRetVoid();
    return F;
}

static Value* getElementPtr(AllocaInstruction* A, std::vector<std::unique_ptr<ExprNode>>& expr)
{
    // auto iter = expr.begin();
    std::vector<Value*> IDX;
    IDX.push_back(ConstantInt::get(Type::getInt32Ty(), 0));
    for(auto& idx : expr)
    {
        auto val = irEmit(idx.get());
        if(val->getType()->isFloatTy())
            val = Builder.CreateFPToSI(val, Type::getInt32Ty(), "float2int");
        IDX.push_back(val);
    }
    
    Value* addr = Builder.CreateInBoundsGEP(A->getAllocatedType(), A, IDX, "arrayitem");
    // Value* idx = irEmit(*iter); iter++;
    // Value* addr = Builder.CreateInBoundsGEP(A->getAllocatedType(), A, 
    //     {ConstantInt::get(Type::getInt32Ty(), 0), idx}, "arrayitem");
    // for(;iter != expr.end(); iter++)
    // {
    //     idx = irEmit(*iter);
    //     addr = Builder.CreateInBoundsGEP(addr->getType()->getInnerType(), addr,
    //         {ConstantInt::get(Type::getInt32Ty(), 0), idx}, "arrayitem");
    // }
    return addr;
}

static Value* getPtrPtr(AllocaInstruction* A, std::vector<std::unique_ptr<ExprNode>>& expr)
{
    Value* array = Builder.CreateLoad(A->getAllocatedType(), A, "array.cpy");
    std::vector<Value*> IDX;
    for(auto& idx : expr)
    {
        auto val = irEmit(idx.get());
        if(val->getType()->isFloatTy())
            val = Builder.CreateFPToSI(val, Type::getInt32Ty(), "float2int");
        IDX.push_back(val);
    }
    Value* addr = Builder.CreateInBoundsGEP(array->getType()->getInnerType(), array, IDX, "arrayitem");

    return addr;
}

static Value* getElementPtr(GlobalValue* G, std::vector<std::unique_ptr<ExprNode>>& expr)
{
    std::vector<Value*> IDX;
    IDX.push_back(ConstantInt::get(Type::getInt32Ty(), 0));
    for(auto& idx : expr)
    {
        auto val = irEmit(idx.get());
        if(val->getType()->isFloatTy())
            val = Builder.CreateFPToSI(val, Type::getInt32Ty(), "float2int");
        IDX.push_back(val);
    }
    Value* addr = Builder.CreateInBoundsGEP(G->getValueType(), G, IDX, "arrayitem");
    return addr;
}

static AllocaInstruction* CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type*& type, std::vector<int> dimSize) 
{
    IRBuilder TmpB(TheFunction->getEntryBlock(), TheFunction->getEntryBlock()->begin());

    for(auto iter = dimSize.rbegin(); iter != dimSize.rend(); iter++)
        type = ArrayType::get(type, *iter);
    return TmpB.CreateAlloca(type, VarName.c_str());
}

static AllocaInstruction* CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type* type) 
{
    IRBuilder TmpB(TheFunction->getEntryBlock(), TheFunction->getEntryBlock()->begin());
    return TmpB.CreateAlloca(type, VarName.c_str());
}

static void globalInit()
{
    if(InitFuncList.empty())
        return;
    Function* init = createGlobalInit();
    auto& TheModule = Module::getInstance();
    Function* mainFunc = TheModule.getFunction("main");
    Builder.SetInsertPoint(mainFunc->getEntryBlock(), mainFunc->getEntryBlock()->begin());
    Builder.CreateCall(init);
}

Value* irEmit(ExprNode* expr)
{
    if(auto binExpr = dynamic_cast<BinaryExprNode*>(expr))
        return irEmit(binExpr);
    else if(auto number = dynamic_cast<NumberNode*>(expr))
        return irEmit(number);
    else if(auto lval = dynamic_cast<LvalNode*>(expr))
        return irEmit(lval);
    else if(auto funcCall = dynamic_cast<FunctionCallExprNode*>(expr))
        return irEmit(funcCall);
    else if(auto unaryExpr = dynamic_cast<UnaryExprNode*>(expr))
        return irEmit(unaryExpr);
}

Value* irEmit(NumberNode* number)
{
    switch (number->type)
    {
    case INT:
        return ConstantInt::get(32, number->immediate.getInt());
    case FLOAT:
        return ConstantFP::get(number->immediate.getFloat());
    }
}

int cnt = 0;
Value* irEmit(LvalNode* lval)
{
    // PrintMsg(cnt++);
    // PrintMsg(lval->variable);
    auto& scope = ValueScope::getCurrentPtr();
    auto V = scope->getValue(lval->variable);
    if(lval->indexExpr.empty())
    {
        if(AllocaInst* A = dynamic_cast<AllocaInst*>(V))
        {
            if(A->getAllocatedType()->isArrayTy())
                return A;
            else
                return Builder.CreateLoad(A->getAllocatedType(), A, lval->variable + ".cpy");
        }
        else if(GlobalValue* G = dynamic_cast<GlobalValue*>(V))
        {
            if(G->getValueType()->isArrayTy())
                return G;
            else
                return Builder.CreateLoad(G->getValueType(), G, G->getName() + ".cpy");
        }
    }
    else
    {
        Value* val;
        if(AllocaInst* A = dynamic_cast<AllocaInst*>(V))
        {
            if(A->getAllocatedType()->isPointerTy())
                val = getPtrPtr(A, lval->indexExpr);
            else
                val = getElementPtr(A, lval->indexExpr);
        }
        else if(GlobalValue* G = dynamic_cast<GlobalValue*>(V))
            val = getElementPtr(G, lval->indexExpr);
        
        if(val->getType()->isPointerTy() && val->getType()->getInnerType()->isArrayTy())
            return val;
        
        return Builder.CreateLoad(val->getType()->getInnerType(), val, "item");
    }
}
Value* irEmit(BinaryExprNode* binExpr)
{
    Value* lhs = nullptr;
    if(binExpr->op == ROP_OR || binExpr->op == ROP_AND)
    {
        Function* theFunc = Builder.GetInsertBlock()->getParent();
        BasicBlock* still = BasicBlock::Create("still", theFunc);
        if(binExpr->op == ROP_OR)
        {
            BasicBlock* trueBlock = BasicBlockTarget::getInstance().getTrue();

            BasicBlockTarget::getInstance().setFalse(still);
            lhs = irEmit(binExpr->lhs.get());
            BasicBlockTarget::getInstance().exitFalse();

            if(lhs->getType()->isFloatTy())
                lhs = Builder.CreateFCmpUNE(lhs, ConstantFP::get(0.0f), "lhs");
            else
                lhs = Builder.CreateICmpNE(lhs, ConstantInt::get(lhs->getType(), 0), "lhs");

            Builder.CreateCondBr(lhs, trueBlock, still);
            Builder.SetInsertPoint(still);
        }
        else
        {
            BasicBlock* falseBlock = BasicBlockTarget::getInstance().getFalse();

            BasicBlockTarget::getInstance().setTrue(still);
            lhs = irEmit(binExpr->lhs.get());
            BasicBlockTarget::getInstance().exitTrue();

            if(lhs->getType()->isFloatTy())
                lhs = Builder.CreateFCmpUNE(lhs, ConstantFP::get(0.0f), "lhs");
            else
                lhs = Builder.CreateICmpNE(lhs, ConstantInt::get(lhs->getType(), 0), "lhs");

            Builder.CreateCondBr(lhs, still, falseBlock);
            Builder.SetInsertPoint(still);
        }
    }
    if(!lhs)
        lhs = irEmit(binExpr->lhs.get());
    Value* rhs = irEmit(binExpr->rhs.get());
    if(binExpr->type == INT)
    {
        if(lhs->getType()->isFloatTy())
            lhs = Builder.CreateFPToSI(lhs, IntegerType::getInt32Ty(), "float2int");
        if(rhs->getType()->isFloatTy())
            rhs = Builder.CreateFPToSI(rhs, IntegerType::getInt32Ty(), "float2int");
        switch (binExpr->op)
        {
        case BOP_ADD:
            return Builder.CreateAdd(lhs, rhs, "addtmp");
        case BOP_SUB: 
            return Builder.CreateSub(lhs, rhs, "subtmp");
        case BOP_MUL:
            return Builder.CreateMul(lhs, rhs, "multmp");
        case BOP_DIV:
            return Builder.CreateSDiv(lhs, rhs, "divtmp");
        case BOP_MOD: 
            return Builder.CreateSRem(lhs, rhs, "modtmp");
        case ROP_EQL:
            return Builder.CreateICmpEQ(lhs, rhs, "eqltmp");
        case ROP_NEQ:
            return Builder.CreateICmpNE(lhs, rhs, "neqtmp");
        case ROP_LES:
            return Builder.CreateICmpSLT(lhs, rhs, "lestmp");
        case ROP_GRT:
            return Builder.CreateICmpSGT(lhs, rhs, "grttmp");
        case ROP_LEQ:
            return Builder.CreateICmpSLE(lhs, rhs, "leqtmp");
        case ROP_GEQ:
            return Builder.CreateICmpSGE(lhs, rhs, "geqtmp");
        // TODO add the short circuit using the condbr
        case ROP_OR:
            return Builder.CreateICmpNE(rhs, ConstantInt::get(rhs->getType(), 0), "ortmp");
        case ROP_AND:
            return Builder.CreateICmpNE(rhs, ConstantInt::get(rhs->getType(), 0), "andtmp");
        default:
            ErrorMsg("Bad Binary op");
        }
    }
    else if (binExpr->type == FLOAT)
    {
        if(lhs->getType()->isIntegerTy())
            lhs = Builder.CreateSIToFP(lhs, Type::getFloatTy(), "int2float");
        if(rhs->getType()->isIntegerTy())
            rhs = Builder.CreateSIToFP(rhs, Type::getFloatTy(), "int2float");
        switch (binExpr->op)
        {
        case BOP_ADD:
            return Builder.CreateFAdd(lhs, rhs, "addtmp");
        case BOP_SUB: 
            return Builder.CreateFSub(lhs, rhs, "subtmp");
        case BOP_MUL:
            return Builder.CreateFMul(lhs, rhs, "multmp");
        case BOP_DIV:
            return Builder.CreateFDiv(lhs, rhs, "divtmp");
        case BOP_MOD: 
            return Builder.CreateFRem(lhs, rhs, "modtmp");
        case ROP_EQL:
            return Builder.CreateFCmpUEQ(lhs, rhs, "eqltmp");
        case ROP_NEQ:
            return Builder.CreateFCmpUNE(lhs, rhs, "neqtmp");
        case ROP_LES:
            return Builder.CreateFCmpULT(lhs, rhs, "lestmp");
        case ROP_GRT:
            return Builder.CreateFCmpUGT(lhs, rhs, "grttmp");
        case ROP_LEQ:
            return Builder.CreateFCmpULE(lhs, rhs, "leqtmp");
        case ROP_GEQ:
            return Builder.CreateFCmpUGE(lhs, rhs, "geqtmp");
        default:
            ErrorMsg("Bad Binary op");
        }
    }
    else
        ErrorMsg("Bad Binary type");
}

Value* irEmit(FunctionCallExprNode* funcCall)
{
    auto& TheModule = Module::getInstance();
    Function* func = TheModule.getFunction(funcCall->funcName);

    // if(funcCall->funcParamList)
    // TODO move to semantics check
    if(func->arg_size() != funcCall->funcParamList.size() && funcCall->format.empty())
        ErrorMsg("Args length can not march");
    // FIXME we could do the type cast during the function call.
    std::vector<Value*> args;
    if(!funcCall->format.empty())
    {
        Value* str = Constant::getString(funcCall->format);
        str = Builder.CreateInBoundsGEP(str->getType(), str,
                    {ConstantInt::get(Type::getInt32Ty(), 0), 
                    ConstantInt::get(Type::getInt32Ty(), 0)}, "string");
        args.push_back(str);
    }
    int idx = 0;
    for(auto& iter : funcCall->funcParamList)
    {
        Value* arg = irEmit(iter.get());

        if(arg->getType()->isIntegerTy() && func->getArgs().at(idx)->getType()->isFloatTy())
            arg = Builder.CreateSIToFP(arg, Type::getFloatTy(), "int2float");
        else if(arg->getType()->isFloatTy() && func->getArgs().at(idx)->getType()->isIntegerTy())
            arg = Builder.CreateFPToSI(arg, Type::getInt32Ty(), "float2int");

        auto isArg = [&](Value* arg) -> bool {
            if(auto load = dynamic_cast<LoadInst*>(arg); load && load->getType()->isPointerTy())
                return true;
            return false;
        };

        if(dynamic_cast<GlobalValue*>(arg) || arg->getType()->isPointerTy() && arg->getType()->getInnerType()->isArrayTy() && !isArg(arg))
            arg = Builder.CreateInBoundsGEP(arg->getType()->getInnerType(), arg,
                    {ConstantInt::get(Type::getInt32Ty(), 0), 
                    ConstantInt::get(Type::getInt32Ty(), 0)}, "arrayptr");
        args.push_back(arg);
        idx++;
    }
    if(funcCall->type != VOID)
        return Builder.CreateCall(func, args, "funcCall");
    else
        return Builder.CreateCall(func, args);
}

Value* irEmit(UnaryExprNode* unaryExpr)
{
    Value* expr = irEmit(unaryExpr->expr.get());
    if(unaryExpr->type == INT)
        switch (unaryExpr->op)
        {
        case UOP_ADD:
            return expr;
        case UOP_SUB:
            return Builder.CreateNeg(expr, "negtmp");
        case UOP_NOT:
            return Builder.CreateNot(expr, "nottmp");
        default:
            ErrorMsg("Bad Unary op");
        }
    else if(unaryExpr->type == FLOAT)
        switch (unaryExpr->op)
        {
        case UOP_ADD:
            return expr;
        case UOP_SUB:
            return Builder.CreateFNeg(expr, "negtmp");
        }
    else
        ErrorMsg("Bad Unary type");
}

Value* irEmit(StmtNode* stmt)
{
    if(auto block = dynamic_cast<BlockNode*>(stmt))
        return irEmit(block);
    else if(auto ifStmt = dynamic_cast<IfStmtNode*>(stmt))
        return irEmit(ifStmt);
    else if(auto assignStmt = dynamic_cast<AssignStmtNode*>(stmt))
        return irEmit(assignStmt);
    else if(auto exprStmt = dynamic_cast<ExprStmtNode*>(stmt))
        return irEmit(exprStmt);
    else if(auto forStmt = dynamic_cast<ForStmtNode*>(stmt))
        return irEmit(forStmt);
    else if(auto whileStmt = dynamic_cast<WhileStmtNode*>(stmt))
        return irEmit(whileStmt);
    else if(auto breakStmt = dynamic_cast<BreakStmtNode*>(stmt))
        return irEmit(breakStmt);
    else if(auto continueStmt = dynamic_cast<ContinueStmtNode*>(stmt))
        return irEmit(continueStmt);
    else if (auto returnStmt = dynamic_cast<ReturnStmtNode*>(stmt))
        return irEmit(returnStmt);
    else if(auto decl = dynamic_cast<DeclNode*>(stmt))
        return irEmit(decl);
    else
        ErrorMsg("Invalid stmt type");
}

Value* irEmit(BlockNode* block)
{
    auto& scope = ValueScope::getCurrentPtr();
    scope = scope->createChild();
    Value* end = nullptr;
    for(auto& item : block->blockItems)
        end = irEmit(item.get());
    scope = ValueScope::exitBlock(scope);
    return end;
}

Function* irEmit(FuncDefNode* funcDef)
{
    std::vector<Type*> argType;
    if(funcDef->funcParamList)
        for(auto& arg : funcDef->funcParamList->funcParam)
            if(arg->type == INT)
            {
                if(arg->dimSize.empty())
                    argType.push_back(Type::getInt32Ty());
                else
                {
                    Type* type = Type::getInt32Ty();
                    for(auto iter = arg->dimSize.rbegin(); iter != arg->dimSize.rend(); iter++)
                        if(*iter)
                            type = ArrayType::get(type, *iter);
                    type = Type::getPointerTy(type);
                    argType.push_back(type);
                }
            }
            else if(arg->type == FLOAT)
            {
                if(arg->dimSize.empty())
                    argType.push_back(Type::getFloatTy());
                else
                {
                    Type* type = Type::getFloatTy();
                    for(auto iter = arg->dimSize.rbegin(); iter != arg->dimSize.rend(); iter++)
                        if(*iter)
                            type = ArrayType::get(type, *iter);
                    type = Type::getPointerTy(type);
                    argType.push_back(type);
                }
            }
            else
                ErrorMsg("Invalid args type");
    FunctionType* FT;
    if(funcDef->funcType == VOID)
        FT = FunctionType::get(Type::getVoidTy(), argType, false);
    else if(funcDef->funcType == INT)
        FT = FunctionType::get(Type::getInt32Ty(), argType, false);
    else if(funcDef->funcType == FLOAT)
        FT = FunctionType::get(Type::getFloatTy(), argType, false);
    else
        ErrorMsg("Invalid ret type");
    Function* F = Function::Create(FT, funcDef->funcName);

    size_t idx = 0;
    if(funcDef->funcParamList)
        for(auto& arg : F->getArgs())
            arg->setName(funcDef->funcParamList->funcParam.at(idx++)->varName);
    BasicBlock* block = BasicBlock::Create("entry", F);
    Builder.SetInsertPoint(block);
    auto& scope = ValueScope::getCurrentPtr();
    scope = scope->createChild();
    idx = 0;
    for(auto& arg : F->getArgs())
    {
        AllocaInst* alloca = CreateEntryBlockAlloca(F, arg->getName() + ".alloca", arg->getType());
        Builder.CreateStore(arg.get(), alloca);
        scope->insertAlloca(arg->getName(), alloca);
    }
    Value* ret = irEmit(funcDef->block.get());

    if(FT->getRet()->isIntegerTy())
        Builder.CreateRet(ConstantInt::get(32, 0));
    else if(FT->getRet()->isFloatTy())
        Builder.CreateRet(ConstantFP::get(0));
    else
        Builder.CreateRetVoid();

    scope = ValueScope::exitBlock(scope);
    return F;
}

Value* irEmit(IfStmtNode* ifStmt)
{
    // PrintMsg("if stmt");
    Function* theFunc = Builder.GetInsertBlock()->getParent();
    if(ifStmt->elseBlock && ifStmt->thenBlock)
    {
        BasicBlock* thenBlock = BasicBlock::Create("then", theFunc);
        BasicBlock* elseBlock = BasicBlock::Create("else");
        BasicBlock* nextBlock = BasicBlock::Create("ifnext");

        BasicBlockTarget::getInstance().setCond(thenBlock, elseBlock);
        Value* cond = irEmit(ifStmt->cond.get());
        BasicBlockTarget::getInstance().exitCond();
        if(cond->getType()->isFloatTy())
            cond = Builder.CreateFCmpUNE(cond, ConstantFP::get(0.0f), "ifcond");
        else
            cond = Builder.CreateICmpNE(cond, ConstantInt::get(cond->getType(), 0), "cond");

        Builder.CreateCondBr(cond, thenBlock, elseBlock);

        Builder.SetInsertPoint(thenBlock);
        Value* then = irEmit(ifStmt->thenBlock.get());
        
        Builder.CreateBr(nextBlock);

        theFunc->getBasicBlockList().push_back(elseBlock);
        elseBlock->setParent(theFunc);
        Builder.SetInsertPoint(elseBlock);
        Value* _else = irEmit(ifStmt->elseBlock.get());
        
        Builder.CreateBr(nextBlock);

        theFunc->getBasicBlockList().push_back(nextBlock);
        nextBlock->setParent(theFunc);
        Builder.SetInsertPoint(nextBlock);
    }
    else if(ifStmt->thenBlock)
    {
        BasicBlock* thenBlock = BasicBlock::Create("then", theFunc);
        BasicBlock* nextBlock = BasicBlock::Create("ifnext");

        BasicBlockTarget::getInstance().setCond(thenBlock, nextBlock);
        Value* cond = irEmit(ifStmt->cond.get());
        BasicBlockTarget::getInstance().exitCond();
        if(cond->getType()->isFloatTy())
            cond = Builder.CreateFCmpUNE(cond, ConstantFP::get(0.0f), "ifcond");
        else
        {
            if(cond->getType() == Type::getInt32Ty())
                cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt32Ty(), 0), "cond");
            else
                cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt1Ty(), 0), "cond");
        }

        Builder.CreateCondBr(cond, thenBlock, nextBlock);

        Builder.SetInsertPoint(thenBlock);
        Value* then = irEmit(ifStmt->thenBlock.get());
        
        Builder.CreateBr(nextBlock);

        theFunc->getBasicBlockList().push_back(nextBlock);
        nextBlock->setParent(theFunc);
        Builder.SetInsertPoint(nextBlock);
    }
    else
    {
        BasicBlock* nextBlock = BasicBlock::Create("ifnext", theFunc);
        BasicBlockTarget::getInstance().setCond(nextBlock, nextBlock);
        Value* cond = irEmit(ifStmt->cond.get());
        BasicBlockTarget::getInstance().exitCond();
        
        Builder.CreateBr(nextBlock);
        Builder.SetInsertPoint(nextBlock);
    }

    return Constant::getZero(Type::getInt32Ty());
}

Value* irEmit(AssignStmtNode* assignStmt)
{
    Value* val = irEmit(assignStmt->expr.get());
    if(assignStmt->lval->type == INT && assignStmt->expr->type == FLOAT)
        val = Builder.CreateFPToSI(val, IntegerType::getInt32Ty(), "float2int");
    else if (assignStmt->lval->type == FLOAT && assignStmt->expr->type == INT)
        val = Builder.CreateSIToFP(val, Type::getFloatTy(), "int2float");

    auto& scope = ValueScope::getCurrentPtr();
    Value* var = scope->getValue(assignStmt->lval->variable);
    if(!assignStmt->lval->dimSize.empty())
    {
        if(AllocaInst* A = dynamic_cast<AllocaInst*>(var))
        {
            if(A->getAllocatedType()->isPointerTy())
                var = getPtrPtr(A, assignStmt->lval->indexExpr);
            else
                var = getElementPtr(A, assignStmt->lval->indexExpr);
        }
        else if(GlobalValue* G = dynamic_cast<GlobalValue*>(var))
            var = getElementPtr(G, assignStmt->lval->indexExpr);
    }
    Builder.CreateStore(val, var);
    return val;
}

Value* irEmit(ExprStmtNode* exprStmt)
{
    return irEmit(exprStmt->expr.get());
}

// FIXME for loop can not decl var at the start.
Value* irEmit(ForStmtNode* forStmt)
{
    Function* theFunc = Builder.GetInsertBlock()->getParent();
    Value* init = irEmit(forStmt->initial.get());
    BasicBlock* loop = BasicBlock::Create("forloop", theFunc);
    BasicBlock* after = BasicBlock::Create("afterloop");
    BasicBlockTarget::getInstance().setLoop(loop, after);
    Builder.CreateBr(loop);

    Builder.SetInsertPoint(loop);
    Value* end = irEmit(forStmt->stmt.get());
    Value* incr = irEmit(forStmt->increment.get());
    Value* cond = irEmit(forStmt->cond.get());
    if(cond->getType() == Type::getInt32Ty())
        cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt32Ty(), 0), "cond");
    else
        cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt1Ty(), 0), "cond");
    Builder.CreateCondBr(cond, loop, after);

    theFunc->getBasicBlockList().push_back(after);
    Builder.SetInsertPoint(after);

    return Constant::getZero(Type::getInt32Ty());
}

Value* irEmit(WhileStmtNode* whileStmt)
{
    // PrintMsg("while stmt");
    Function* theFunc = Builder.GetInsertBlock()->getParent();
    BasicBlock* guard = BasicBlock::Create("guard", theFunc);
    BasicBlock* exit = BasicBlock::Create("exit");
    if(!whileStmt->stmt)
    {
        Builder.CreateBr(guard);
        Builder.SetInsertPoint(guard);
        BasicBlockTarget::getInstance().setCond(guard, exit);
        Value* cond = irEmit(whileStmt->cond.get());
        BasicBlockTarget::getInstance().exitCond();
        if(cond->getType() == Type::getInt32Ty())
            cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt32Ty(), 0), "cond");
        else
            cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt1Ty(), 0), "cond");
        Builder.CreateCondBr(cond, guard, exit);
        theFunc->getBasicBlockList().push_back(exit);
        exit->setParent(theFunc);
        Builder.SetInsertPoint(exit);
        return Constant::getZero(Type::getInt32Ty());
    }
    BasicBlock* preHead = BasicBlock::Create("preHeader", true);
    BasicBlock* header = BasicBlock::Create("header");
    BasicBlock* latch = BasicBlock::Create("latch");
    BasicBlockTarget::getInstance().setLoop(latch, exit);
    Builder.CreateBr(guard);

    Builder.SetInsertPoint(guard);
    BasicBlockTarget::getInstance().setCond(preHead, exit);
    Value* cond = irEmit(whileStmt->cond.get());
    BasicBlockTarget::getInstance().exitCond();
    if(cond->getType() == Type::getInt32Ty())
        cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt32Ty(), 0), "cond");
    else
        cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt1Ty(), 0), "cond");
    Builder.CreateCondBr(cond, preHead, exit);

    theFunc->getBasicBlockList().push_back(preHead);
    preHead->setParent(theFunc);
    Builder.SetInsertPoint(preHead);
    Builder.CreateBr(header);

    theFunc->getBasicBlockList().push_back(header);
    header->setParent(theFunc);
    Builder.SetInsertPoint(header);
    Value* stmt = irEmit(whileStmt->stmt.get());
    Builder.CreateBr(latch);

    theFunc->getBasicBlockList().push_back(latch);
    latch->setParent(theFunc);
    Builder.SetInsertPoint(latch);
    BasicBlockTarget::getInstance().setCond(header, exit);
    cond = irEmit(whileStmt->cond.get());
    BasicBlockTarget::getInstance().exitCond();
    if(cond->getType() == Type::getInt32Ty())
        cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt32Ty(), 0), "cond");
    else
        cond = Builder.CreateICmpNE(cond, ConstantInt::get(Type::getInt1Ty(), 0), "cond");
    Builder.CreateCondBr(cond, header, exit);

    theFunc->getBasicBlockList().push_back(exit);
    exit->setParent(theFunc);
    Builder.SetInsertPoint(exit);
    BasicBlockTarget::getInstance().exitLoop();
    return Constant::getZero(Type::getInt32Ty());
}

Value* irEmit(BreakStmtNode* breakStmt)
{
    // PrintMsg("break stmt");
    Function* theFunc = Builder.GetInsertBlock()->getParent();
    BasicBlock* target = BasicBlockTarget::getInstance().getAfter();
    auto ret = Builder.CreateBr(target);

    auto dead = BasicBlock::Create("dead", theFunc);
    Builder.SetInsertPoint(dead);
    return ret;
}

Value* irEmit(ContinueStmtNode* continueStmt)
{
    // PrintMsg("continue stmt");
    Function* theFunc = Builder.GetInsertBlock()->getParent();
    BasicBlock* target = BasicBlockTarget::getInstance().getEntry();
    auto ret = Builder.CreateBr(target);

    auto dead = BasicBlock::Create("dead", theFunc);
    Builder.SetInsertPoint(dead);
    return ret;
}

Value* irEmit(ReturnStmtNode* returnStmt)
{
    // PrintMsg("return stmt");
    Function* theFunc = Builder.GetInsertBlock()->getParent();
    if(!returnStmt->has_ret_val)
    {
        auto ret = Builder.CreateRetVoid();

        auto dead = BasicBlock::Create("dead", theFunc);
        Builder.SetInsertPoint(dead);
        return ret;
    }
    Value* expr = irEmit(returnStmt->expr.get());

    auto func = Builder.GetInsertBlock()->getParent();
    if(func->getReturnType()->isIntegerTy() && expr->getType()->isFloatTy())
        expr = Builder.CreateFPToSI(expr, Type::getInt32Ty(), "float2int");
    else if(func->getReturnType()->isFloatTy() && expr->getType()->isIntegerTy())
        expr = Builder.CreateSIToFP(expr, Type::getFloatTy(), "int2float");

    auto ret = Builder.CreateRet(expr);

    auto dead = BasicBlock::Create("dead", theFunc);
    Builder.SetInsertPoint(dead);
    return ret;
}

Value* privateInit(Constant* initial, Type* type, std::string name, bool is_constant)
{
    GlobalValue* V = GlobalValue::Create(type, is_constant, initial, name);
    return V;
}

bool getNumberArray(std::vector<std::unique_ptr<ExprNode>>& initial, std::vector<NumberNode*>& constInital)
{
    if(initial.empty())
        return true;
    constInital.clear();
    for(auto& expr : initial)
        if(auto number = dynamic_cast<NumberNode*>(expr.get()))
            constInital.push_back(number);
        else
            return false;
    return true;
}

Constant* getConstantArray(std::vector<NumberNode*> constInitial, ArrayType* type)
{
    if(constInitial.empty())
    {
        return ConstantArray::getZero(type);
    }
    if(constInitial.size() == type->getSize())
    {
        Type* _type = type->getInnerType();
        std::vector<Constant*> arr;
        for(auto iter : constInitial)
        {
            if(_type->isIntegerTy())
                arr.push_back(ConstantInt::get(_type, iter->immediate.getInt()));
            else
                arr.push_back(ConstantFP::get(iter->immediate.getFloat()));
        }
        return ConstantArray::get(type, arr);
    }
    size_t step = constInitial.size() / type->getSize();
    std::vector<Constant*> arr;
    for(auto iter = constInitial.begin(); iter != constInitial.end(); iter += step)
    {
        auto subInit = std::vector<NumberNode*>(iter, iter + step);
        auto _type = dynamic_cast<ArrayType*>(type->getInnerType());
        arr.push_back(getConstantArray(subInit, _type));
    }
    return ConstantArray::get(type, arr);
}

Value* irEmit(DeclNode* decl) 
{
    if(auto constDecl = dynamic_cast<ConstDeclNode*>(decl))
        return irEmit(constDecl);
    else if(auto varDecl = dynamic_cast<VarDeclNode*>(decl))
        return irEmit(varDecl);
}

Value* irEmit(ConstDeclNode* constDecl) 
{
    Function* theFunction = Builder.GetInsertBlock()->getParent();
    AstType _type = constDecl->type;

    Value* ret;
    auto& scope = ValueScope::getCurrentPtr();
    for(auto& def : constDecl->constDef)
    {
        auto dim = def->dimSize.size();
        const std::string& name = def->name;
        Type* type = AstType2IrType(_type);
        AllocaInst* alloca = CreateEntryBlockAlloca(theFunction, name, type, def->dimSize);
        Value* initVal;
        if(!dim)
        {
            if(def->constInitVal)
            {
                initVal = irEmit(def->constInitVal->initItem.at(0).get());
                if(!isSameType(initVal->getType(), type))
                {
                    if(initVal->getType()->isIntegerTy())
                        initVal = Builder.CreateSIToFP(initVal, type, "int2float");
                    else
                        initVal = Builder.CreateFPToSI(initVal, type, "float2int");
                }
                Builder.CreateStore(initVal, alloca);
            }
        }
        else
        {
            Constant* initial;
            std::vector<NumberNode*> constInit;
            if(getNumberArray(def->constInitVal->initItem, constInit))
                initial = getConstantArray(constInit, dynamic_cast<ArrayType*>(type));
            else
                ErrorMsg("We can not use the variable to initial the constant.");
            std::string name = theFunction->getName() + "." 
                                + Builder.GetInsertBlock()->getName() + "." + def->name;
            ret = privateInit(initial, type, name, true);
            scope->insertAlloca(name, ret);
            return ret;
        }
        scope->insertAlloca(name, alloca);
    }
    return ret;
}

Value* irEmit(VarDeclNode* varDecl) 
{
    // PrintMsg("decl stmt");
    Function* theFunction = Builder.GetInsertBlock()->getParent();
    AstType _type = varDecl->type;

    auto& scope = ValueScope::getCurrentPtr();
    for(auto& def : varDecl->varDef)
    {
        auto dim = def->dimSize.size();
        const std::string& name = def->name;
        Type* type = AstType2IrType(_type);
        AllocaInst* alloca = CreateEntryBlockAlloca(theFunction, name, type, def->dimSize);
        Value* gv = nullptr;
        Value* initVal;
        if(!dim)
        {
            if(def->varInitVal)
            {
                initVal = irEmit(def->varInitVal->initItem.at(0).get());
                if(!isSameType(initVal->getType(), type))
                {
                    if(initVal->getType()->isIntegerTy())
                        initVal = Builder.CreateSIToFP(initVal, type, "int2float");
                    else
                        initVal = Builder.CreateFPToSI(initVal, type, "float2int");
                }
                Builder.CreateStore(initVal, alloca);
            }
        }
        else
        {
            Constant* initial;
            std::vector<NumberNode*> constInit;
            // type->print(outs());
            if(def->varInitVal && theFunction->getName() == "main" && getNumberArray(def->varInitVal->initItem, constInit))
            {
                initial = getConstantArray(constInit, dynamic_cast<ArrayType*>(type));
                alloca->getBasicBlock()->getInstructions().remove(alloca);
                std::string name = theFunction->getName()
                        + "." + Builder.GetInsertBlock()->getName() + "." + def->name;
                gv = privateInit(initial, type, name, false);
            }
            else if(def->varInitVal)
            {
                Value* gpr = Builder.CreateInBoundsGEP(alloca->getAllocatedType(), alloca, 
                    {ConstantInt::get(Type::getInt32Ty(), 0)}, "gpr");
                emitInitial(alloca, def->varInitVal->initItem, dynamic_cast<ArrayType*>(alloca->getAllocatedType()));
            }
        }
        if(gv)
            scope->insertAlloca(name, gv);
        else
            scope->insertAlloca(name, alloca);
    }
    return Constant::getZero(Type::getInt32Ty());
}


// FIXME global array
GlobalValue* createGlobal(VarDefNode* def, AstType type)
{
    Type* _type = AstType2IrType(type);
    if(!def->dimSize.empty())
    {
        for(auto riter = def->dimSize.rbegin(); riter != def->dimSize.rend(); riter++)
            _type = ArrayType::get(_type, *riter);
        Constant* initial = Constant::getZero(_type);
        bool var = false;
        if(def->varInitVal)
        {
            std::vector<NumberNode*> constInit;
            if(getNumberArray(def->varInitVal->initItem, constInit))
                initial = getConstantArray(constInit, dynamic_cast<ArrayType*>(_type));
            else
                var = true;
        }
        GlobalValue* V = GlobalValue::Create(_type, false, initial, def->name);
        if(var)
            InitFuncList.push_back(createGlobalInit(V, def->varInitVal->initItem));
        return V;
    }
    Constant* initial;
    bool var = false;
    if(type == INT)
        initial = ConstantInt::get(32, 0);
    else
        initial = ConstantFP::get(0.0f);
    if(def->varInitVal)
    {
        if(auto number = dynamic_cast<NumberNode*>(def->varInitVal->initItem.at(0).get()))
        {
            if(type == INT)
                initial = ConstantInt::get(_type, number->immediate.getInt());
            else
                initial = ConstantFP::get(number->immediate.getFloat());
        }
        else
            var = true;
    }
    GlobalValue* V = GlobalValue::Create(_type, false, initial, def->name);
    if(var)
        InitFuncList.push_back(createGlobalInit(V, def->varInitVal->initItem));
    return V;
}

// FIXME global array
GlobalValue* createGlobal(ConstDefNode* def, AstType type)
{
    Type* _type = AstType2IrType(type);
    if(!def->dimSize.empty())
    {
        for(auto riter = def->dimSize.rbegin(); riter != def->dimSize.rend(); riter++)
            _type = ArrayType::get(_type, *riter);
        Constant* initial = Constant::getZero(_type);
        if(def->constInitVal)
        {
            std::vector<NumberNode*> constInit;
            if(getNumberArray(def->constInitVal->initItem, constInit))
                initial = getConstantArray(constInit, dynamic_cast<ArrayType*>(_type));
            else
                ErrorMsg("We can not use the variable to initial the constant.");
        }
        GlobalValue* V = GlobalValue::Create(_type, true, initial, def->name);
        return V;
    }
    Constant* initial;
    if(type == INT)
        initial = ConstantInt::get(32, 0);
    else
        initial = ConstantFP::get(0.0f);
    if(def->constInitVal)
    {
        if(auto number = dynamic_cast<NumberNode*>(def->constInitVal->initItem.at(0).get()))
        {
            if(type == INT)
                initial = ConstantInt::get(_type, number->immediate.getInt());
            else
                initial = ConstantFP::get(number->immediate.getFloat());
        }
        else
            ErrorMsg("We can not use the variable to initial the constant.");
    }
    GlobalValue* V = GlobalValue::Create(_type, true, initial, def->name);
    return V;
}

Module* emitIR(CompUnitNode* compUnit)
{
    initRuntime();

    auto& scope = ValueScope::getCurrentPtr();
    for(auto& item : compUnit->element)
    {
        if(auto decl = dynamic_cast<DeclNode*>(item.get()))
        {
            if(auto constDecl = dynamic_cast<ConstDeclNode*>(decl))
            {
                AstType type = constDecl->type;
                for(auto& def : constDecl->constDef)
                {
                    GlobalValue* V = createGlobal(def.get(), type);
                    scope->insertAlloca(def->name, V);
                }
            }
            else if(auto varDecl = dynamic_cast<VarDeclNode*>(decl))
            {
                AstType type = varDecl->type;
                for(auto& def : varDecl->varDef)
                {
                    GlobalValue* V = createGlobal(def.get(), type);
                    scope->insertAlloca(def->name, V);
                }
            }
        }
        else if(auto funcDef = dynamic_cast<FuncDefNode*>(item.get()))
        {
            auto func = irEmit(funcDef);

            SimplifyCFG::removeDeadBB(func);
        }
    }
    globalInit();
    return std::addressof(Module::getInstance());
}

}

}