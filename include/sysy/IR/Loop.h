#ifndef SYSY_IR_LOOP_H
#define SYSY_IR_LOOP_H
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/PassManager.h"
#include "sysy/Support/iterator_range.h"
#include <vector>
#include <unordered_set>
namespace sysy
{

namespace ir
{

class SCEV
{
private:
    std::vector<Value*> scevBody;
    std::set<BinaryInst*> instructionsHasBeenCaculated;
public:
    SCEV() {};
    SCEV(Value* initial, Value* step);
    SCEV(Value* initial, const SCEV& innerSCEV);
    SCEV(const std::vector<Value*>& vec);
    SCEV(Value* initial, Value* step, std::set<BinaryInst*>&& binarys);

    friend SCEV operator+(Value* lhs, const SCEV& rhs);
    friend SCEV operator+(const SCEV& lhs, const SCEV& rhs);

    friend SCEV operator-(const SCEV& lhs, Value* rhs);
    friend SCEV operator-(const SCEV& lhs, const SCEV& rhs);

    friend SCEV operator*(Value* lhs, const SCEV& rhs);
    // friend SCEV operator*(const SCEV& lhs, const SCEV& rhs);

    Value*& at(unsigned i) { return scevBody.at(i); }

    size_t size() { return scevBody.size(); }

    Value* at(unsigned i) const { return scevBody.at(i); }

    size_t size() const { return scevBody.size(); }

    void getItemChains(unsigned i, std::vector<BinaryInst*>& instrChain);

    std::string getSignature();

    void clear();
};
    
class Loop
{
private:
    Function* parent;

    int loopDepth;
    Loop* parentLoop;
    BasicBlock* loopHeader;
    std::vector<Loop*> subLoops;
    std::vector<BasicBlock*> blocks;

    std::unordered_set<BasicBlock*> exitingBlocks; 
    std::unordered_set<BasicBlock*> exitBlocks;
    BasicBlock* latchBlock;
    BasicBlock* preHeader;

    std::unordered_map<Instruction*, SCEV> SCEVCheck;

    PhiInst* indPhi;
    Instruction* indCondVar;
    Value* indEnd;
    Instruction::InstrKind icmpKind;
    int tripCount;
public:
    Loop(BasicBlock* header);

    Function* getParent() { return parent; }

	void setLoopDepth(int depth) { loopDepth = depth; }
	int getLoopDepth() { return loopDepth; }

    Loop* getParentLoop() { return parentLoop; }
    void setParentLoop(Loop* parent) { parentLoop = parent; }
    std::vector<Loop*>& getSubLoops() { return subLoops; }
    
    std::vector<BasicBlock*>& getLoopBasicBlocks() { return blocks; }
    std::unordered_set<BasicBlock*> BBSet;

    BasicBlock* getHeader() { return loopHeader; }

    std::unordered_set<BasicBlock*>& getExitingBlocks() { return exitingBlocks; }
    std::unordered_set<BasicBlock*>& getExitBlocks() { return exitBlocks; }

    BasicBlock* getLatchBlock() { return latchBlock; }
    void setLatchBlock(BasicBlock* latch) { latchBlock = latch; }

    BasicBlock* getPreHeader() { return preHeader; }
    void setPreHeader(BasicBlock* _preHeader) { preHeader = _preHeader; }

    SCEV& getSCEV(Instruction* instr);

    bool hasSCEV(Instruction* instr);

    void registerSCEV(Instruction* instr, SCEV scev);

    void cleanSCEV() { SCEVCheck.clear(); }

    using scev_iterator = std::unordered_map<Instruction*, SCEV>::iterator; 
    iterator_range<scev_iterator> getSCEV() { return make_range(SCEVCheck.begin(), SCEVCheck.end()); }

    PhiInst* getIndexPhi() { return indPhi; }
    void setIndexPhi(PhiInst* phi) { indPhi = phi; }
    
    Value* getIndexEnd() { return indEnd; }
    void setIndexEnd(Value* end) { indEnd = end; }

    Instruction* getIndexCondInstr() { return indCondVar; }
    void setIndexCondInstr(Instruction* instr) { indCondVar = instr; }

    int getTripCount() { return tripCount; }
    void setTripCount(int c) { tripCount = c; }

    void setICmpKind(Instruction::InstrKind kind) { icmpKind = kind; }

    Instruction::InstrKind getICmpKind() { return icmpKind; }

    bool isLoopInvariant(Value* val);
};

using LoopPass = Pass<Loop*>;
using LoopPassManager = PassManager<Loop*>;

} // namespace ir



} // namespace sysy


#endif