#ifndef SYSY_TRANSFORM_PRE_H
#define SYSY_TRANSFORM_PRE_H
#include "sysy/IR/PassManager.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/BasicBlock.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
namespace sysy
{
namespace ir
{

class Occurrence
{
private:
    std::string name;
    int _class;
public:
    Occurrence(std::string _name = "");
    std::string getName() { return name; }
    virtual std::string getSignature() = 0;
};

class RealOccurrence : public Occurrence
{
private:
    Instruction* expr;
public:
    RealOccurrence(Instruction* instr, std::string _name = "");
    Instruction* getExpr();
    std::string getSignature() override;
};

class PhiOccurrence : public Occurrence
{
private:
    BasicBlock* father;
    std::vector<std::pair<Occurrence*, BasicBlock*>> incomingOccurrence;
public:
    size_t size() { return incomingOccurrence.size(); }
    BasicBlock* getDefineBB() { return father; }
    PhiOccurrence(BasicBlock* bb, std::string _name = "");
    void setIncoming(Occurrence* occur, BasicBlock* pred);
    void changeIncoming(size_t index, Occurrence* occur) { incomingOccurrence[index].first = occur; }
    std::vector<std::pair<Occurrence*, BasicBlock*>>& getIncoming();
    Occurrence* getIncoming(size_t index);
    BasicBlock* getBB(size_t index);
    std::string getSignature() override;
};

class UndefOccurrence : public Occurrence
{
public:
    UndefOccurrence() : Occurrence("undef") {}
    std::string getSignature() override { return getName(); }
    static UndefOccurrence* get()
    {
        static UndefOccurrence undef;
        return std::addressof(undef);
    }
};

class CollectOccurrences
{
private:
    Instruction* keyInstr;
    std::unordered_set<BasicBlock*> realOccurrencesBB;
    std::unordered_set<BasicBlock*> phiBB;
    std::unordered_map<BasicBlock*, std::vector<Instruction*>> instructions;
    std::unordered_map<BasicBlock*, PhiOccurrence*> phis;
public:
    std::unordered_map<Instruction*, Occurrence*> occurrenceName;
    std::unordered_set<Occurrence*> allClasses;
    CollectOccurrences(Instruction* key);
    bool isSameExpr(Instruction* expr);
    void addExpr(Instruction* instr);
    size_t phiSize() { return phis.size(); }
    void addPhi(PhiOccurrence* phi, BasicBlock* bb);
    PhiOccurrence* getPhi(BasicBlock* bb) { return phis[bb]; }
    std::vector<Instruction*>& getExprFromBB(BasicBlock* bb);
    std::unordered_set<BasicBlock*>& getRealOccurrencesBB() { return realOccurrencesBB; }
    std::unordered_set<BasicBlock*>& getPhiBB() { return phiBB; }
};

class PRE : public FunctionPass
{
private:
    void buildCollectOccurrences(Function* func);
    void insertPhiInstruction(Function* func);
    void renaming(BasicBlock* entry);
    void rename1(BasicBlock* entry, CollectOccurrences& collect);
    void rename2(BasicBlock* entry, CollectOccurrences& collect);
    void resetDownSafe(PhiOccurrence*, int index);
    void downSafety(CollectOccurrences& collect);
    void willBeAvail(CollectOccurrences& collect);
    void resetCanBeAvail(PhiOccurrence* occ, CollectOccurrences& collect);
    void computeCanBeAvail(CollectOccurrences& collect);
    void resetLater(PhiOccurrence* occ, CollectOccurrences& collect);
    void computeLater(CollectOccurrences& collect);
    void finalize();
    void finalize1(CollectOccurrences& collect);
    void finalize2(CollectOccurrences& collect);
public:
    static bool sameOriginal(Value* x, Value* y);
    PRE();
    void run(Function* func) override;
    std::string getName() override { return "PRE"; }
};
    
} // namespace ir
    
} // namespace sysy



#endif