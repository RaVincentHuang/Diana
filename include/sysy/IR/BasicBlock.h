#ifndef SYSY_IR_BASICBLOCK_H
#define SYSY_IR_BASICBLOCK_H

#include "sysy/IR/Value.h"
#include "sysy/IR/Instruction.h"
#include <set>

namespace sysy
{
namespace ir
{

class Function;

class BasicBlock : public Value
{
private:
    Function* father;
    std::list<Instruction*> instructions;
    std::unordered_map<const Instruction*, int> rank;

    std::vector<BasicBlock*> successor;
    std::vector<BasicBlock*> predecessor;

    std::set<BasicBlock*> dom;
    BasicBlock* idom = nullptr;
    std::vector<BasicBlock*> domTreeEdge;
    std::vector<BasicBlock*> domFrontier;
    int domLevel;

    std::set<BasicBlock*> pdom;
    BasicBlock* ipdom = nullptr;
    std::vector<BasicBlock*> pdomTreeEdge;
    std::vector<BasicBlock*> pdomFrontier;
    int pdomLevel;
    // std::vector<BasicBlock*> CDGEdge;

    bool preHeader;

public:
    BasicBlock(const std::string name, bool isPreHeader) : Value(Type::getLabelTy(), name), preHeader(isPreHeader) {}

    BasicBlock(const std::string name, Function* func, bool isPreHeader) : Value(Type::getLabelTy(), name), father(func), preHeader(isPreHeader) {}

    std::list<Instruction*>& getInstructions() { return instructions; }
    using iterator = std::list<Instruction*>::iterator;
    using reverse_iterator = std::list<Instruction*>::reverse_iterator;

    iterator begin() { return instructions.begin(); }
    iterator end() { return instructions.end(); }
    reverse_iterator rbegin() { return instructions.rbegin(); }
    reverse_iterator rend() { return instructions.rend(); }

    void setIdom(BasicBlock* bb) { idom = bb; }
    BasicBlock* getIdom() { return idom; }
    void setDomLevel(int level) { domLevel = level; }
    int getDomLevel() { return domLevel; }
    std::set<BasicBlock*>& getDoms() { return dom; }
    std::vector<BasicBlock*>& getDomTree() { return domTreeEdge; } 
    std::vector<BasicBlock*>& getDomFrontier() { return domFrontier; }

    std::vector<BasicBlock*>& getSuccessor() { return successor; }
    std::vector<BasicBlock*>& getPredecessor() { return predecessor; }


    void setIpdom(BasicBlock* bb) { ipdom = bb; }
    BasicBlock* getIpdom() { return ipdom; }
    void setPdomLevel(int level) { pdomLevel = level; }
    int getPdomLevel() { return pdomLevel; }
    std::set<BasicBlock*>& getPdoms() { return pdom; }
    std::vector<BasicBlock*>& getPdomTree() { return pdomTreeEdge; } 
    std::vector<BasicBlock*>& getPdomFrontier() { return pdomFrontier; } 


    BasicBlock::iterator insertInstr(iterator iter, Instruction* instr) { return instructions.insert(iter, instr); }
    void insertBack(Instruction* instr) { instructions.push_back(instr); }
    void insertBeforeTerminal(Instruction* instr);
    void insertFront(Instruction* instr) { instructions.push_front(instr); }
    void insertAfterPhi(Instruction* instr);
    void setRank(Instruction* instr, int _rank);
    void rankReorder();
    // int getRank(Instruction* instr) { return rank[instr]; }
    int getRank(const Instruction* instr) { return rank[instr]; }
    static BasicBlock* Create(const std::string& name, bool isPreHeader = false);
    static BasicBlock* Create(const std::string& name, Function* theFunction, bool isPreHeader = false);

    static void BasicBlockDfs(BasicBlock* bb, std::function<bool(BasicBlock*)> func);
    static void BasicBlockDfs(BasicBlock* pre, BasicBlock* bb, std::function<bool(BasicBlock*, BasicBlock*)> func);
    static void BasicBlockDfsInv(BasicBlock* bb, std::function<bool(BasicBlock*)> func);
    static void BasicBlockDfsInv(BasicBlock* pre, BasicBlock* bb, std::function<bool(BasicBlock*, BasicBlock*)> func);
    static void BasicBlockDfsPost(BasicBlock* bb, std::function<bool(BasicBlock*)> cond, std::function<void(BasicBlock*)> action);
    static void DomTreeDfsPost(BasicBlock* bb, std::function<bool(BasicBlock*)> cond, std::function<void(BasicBlock*)> action);

    std::string getSignature() override;

    Function* getParent() { return father; }
    void setParent(Function* func) { father = func; }

    bool isPreHeader() { return preHeader; }
};
} // namespace ir
    
} // namespace sysy


#endif