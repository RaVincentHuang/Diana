#ifndef SYSY_IR_FUNCTION_H
#define SYSY_IR_FUNCTION_H

#include "sysy/IR/Type.h"
#include "sysy/IR/Value.h"
#include "sysy/IR/User.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/GlobalValue.h"
#include <vector>
#include <set>

namespace sysy
{
namespace ir
{
class Loop;
class CallInstruction;
class SCEV;
class Function : public Value
{
public:
    class Arg : public Value
    {
    public:
        Arg(Type* _type) : Value(_type, "") {}
        Arg(Type* _type, std::string _name) : Value(_type, _name) {}

        static Arg* make_arg(Type* _type, std::string _name)
        {
            return new Arg(_type, _name);
        }

        void setName(std::string _name) { name = _name; }

        std::string getSignature() override;
    };
private:
    bool innerFunction = false;
    bool sideEffect = true;
    bool recursive = false;
    using ArgVec = std::vector<std::unique_ptr<Arg>>;
    ArgVec args;
    Type* retType;

    using FuncIter = std::list<Function*>::iterator;
    FuncIter funcIter;

    std::set<Function*> caller, callee;
    std::vector<CallInstruction*> callSite;

    std::set<GlobalValue*> storeGV, loadGV;

    std::list<BasicBlock*> basicBlock;

    std::unordered_map<BasicBlock*, Loop*> bbToLoop;
    std::vector<Loop*> topLoops;
    std::vector<Loop*> allLoops;

    BasicBlock* exitBlock = nullptr;
public:
    Function(FunctionType* _type, std::string _name);

    Function(FunctionType* _type);

    static Function* Create(FunctionType* type, const std::string& name)
    {
        return new Function(type, name);
    }

    // static Function* Create(FunctionType* type, const std::string& name)
    // {
    //     return new Function(type, name);
    // }

    static Function* Create(FunctionType* type)
    {
        return new Function(type);
    }

    std::string getSignature() override;

    ArgVec& getArgs() { return args; }

    Type* getReturnType() { return retType; }

    std::set<Function*>& getCaller() { return caller; }
    std::set<Function*>& getCallee() { return callee; }
    std::vector<CallInstruction*>& getCallSite() { return callSite; }

    std::set<GlobalValue*>& getStoreGV() { return storeGV; }
    std::set<GlobalValue*>& getLoadGV() { return loadGV; }

    BasicBlock* getEntryBlock() { return *basicBlock.begin(); }

    BasicBlock* getExitBlock() { return exitBlock; } // Just in ADCE
    void setExitBlock(BasicBlock* exit) { exitBlock = exit; }

    std::list<BasicBlock*>& getBasicBlockList() { return basicBlock; }

    size_t arg_size() { return args.size(); } 

    bool isDeclare() { return basicBlock.empty(); }

    bool isParallelZeon() { return innerFunction; }

    bool hasSideEffect() { return sideEffect; } // TODO  

    void noSideEffect() { sideEffect = false; }

    void clearLinkOfBBToLoop() { bbToLoop.clear(); }
    void linkBBToLoop(BasicBlock* bb, Loop* loop) { bbToLoop[bb] = loop; };
    Loop* getLoopOfBB(BasicBlock* bb) { if(bbToLoop.count(bb)) return bbToLoop[bb]; else return nullptr; }
    std::vector<Loop*>& getTopLoops();
    std::vector<Loop*>& getAllLoops();

    void setRecursive() { recursive = true; }
    bool isRecursive() { return recursive; }
};

} // namespace ir

} // namespace sysy


#endif