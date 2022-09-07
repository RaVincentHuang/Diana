//===----------------Semantics.h - Semantic module -*- C++ -*--------------===//
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
#ifndef SYSY_SEMANTICS_H
#define SYSY_SEMANTICS_H

#include "sysy/Frontend/Ast.h"
#include "sysy/Frontend/AstType.h"
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>


namespace sysy
{

using ast::AstType;
struct
{
    std::string name;
    AstType type;
} storedRuntime[] = {
    { "getint", AstType::INT},
    { "getch", AstType::INT},
    { "getfloat", AstType::FLOAT},
    { "getarray", AstType::INT},
    { "getfarray", AstType::INT},
    { "putint", AstType::VOID},
    { "putch", AstType::VOID},
    { "putfloat", AstType::VOID},
    { "putarray", AstType::VOID},
    { "putfarray", AstType::VOID},
    { "putf", AstType::VOID},
    { "starttime", AstType::VOID},
    { "stoptime", AstType::VOID}
};


class Scope
{
public:
    virtual Scope* getFather() = 0;
};


class AstScope : public Scope
{
    // TODO Use the Decl class to replace the AstType to do the semantics check. 
protected:
    AstScope* father;
    std::unordered_map<std::string, ast::AstType> declType;
    std::unordered_map<std::string, std::vector<int>> dimSize;
    std::unordered_map<std::string, ast::InitValNode*> constInitial;
    std::unordered_set<std::string> constVar;
public:
    AstScope(AstScope* _father) : father(_father) {}
    AstScope() : father(nullptr) {}
    AstScope* getFather() override;
    ast::AstType getType(std::string varName);
    std::vector<int> getDim(std::string varName);
    void insertDim(std::string name, std::vector<int> dimSize);
    void insertVariable(std::string name, ast::AstType type);
    AstScope* createChild();
    void insertInitial(std::string varName, ast::InitValNode* initial);
    ast::InitValNode* getInitial(std::string varName);

    bool isConst(std::string name);
    void setCosnt(std::string name);

    static AstScope* exitBlock(AstScope* ptr);

    static AstScope*& getCurrentPtr()
    {
        static AstScope* ptr = new AstScope;
        return ptr;
    }
};

} // namespace sysy

#endif