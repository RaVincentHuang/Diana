#include "sysy/Frontend/Semantics.h"
#include "sysy/Frontend/AstTransform.h"
#include "sysy/Support/Debug.h"

namespace sysy  
{
AstScope* AstScope::getFather() 
{
    return father;
}

ast::AstType AstScope::getType(std::string varName) 
{   
    auto iter = declType.find(varName);
    if(iter != declType.end())
        return iter->second;
    else if(father)
        return father->getType(varName);
    ErrorMsg(varName + ": Variable undefined");
}

void AstScope::insertVariable(std::string name, ast::AstType type) 
{
    if(this->declType.count(name))
        ErrorMsg(name + ": Variable has been defined");
    this->declType[name] = type;
}

void AstScope::insertInitial(std::string varName, ast::InitValNode* initial)
{
    constInitial[varName] = initial;
}

bool AstScope::isConst(std::string name)
{
    if(constVar.count(name))
        return true;
    else if(father)
        return father->isConst(name);
    return false;
}

void AstScope::setCosnt(std::string name)
{
    constVar.insert(name);
}

ast::InitValNode* AstScope::getInitial(std::string varName)
{
    if(auto iter = constInitial.find(varName); iter != constInitial.end())
        return iter->second;
    else if(father)
        return father->getInitial(varName);
    else
        return nullptr;
}
    

std::vector<int> AstScope::getDim(std::string varName)
{
    auto iter = dimSize.find(varName);
    if(iter != dimSize.end())
        return iter->second;
    else if(father)
        return father->getDim(varName);
    ErrorMsg(varName + ": It seems is not the array");
}

void AstScope::insertDim(std::string name, std::vector<int> dimSize)
{
    if(this->dimSize.count(name))
        ErrorMsg(name + ": Variable has been defined");
    this->dimSize[name] = dimSize;
}

AstScope* AstScope::createChild() 
{
    AstScope* child = new AstScope(this);
    return child;
}

AstScope* AstScope::exitBlock(AstScope* ptr)
{
    auto father = ptr->father;
    delete ptr;
    return father;
}

namespace ast
{
void removeDeadStmts(BlockNode* block)
{
    for(auto iter = block->blockItems.begin(); iter != block->blockItems.end(); iter++)
        if((*iter)->getKind() == AstNode::AST_breakStmt || (*iter)->getKind() == AstNode::AST_continueStmt || (*iter)->getKind() == AstNode::AST_returnStmt)
        {
            block->blockItems.erase(iter + 1, block->blockItems.end());
            break;
        }
}

void optimizeAst(CompUnitNode* ast)
{

}

} // namespcae ast

} // namespace sysy
