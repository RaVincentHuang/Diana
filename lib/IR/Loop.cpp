#include "sysy/IR/Loop.h"
#include "sysy/IR/IRCopy.h"
#include <queue>

namespace sysy
{
namespace ir
{

Loop::Loop(BasicBlock* header) 
    : parent(header->getParent()), parentLoop(nullptr), loopHeader(header), indCondVar(nullptr),
        indEnd(nullptr), indPhi(nullptr), tripCount(0)
{
    blocks.push_back(header);
    BBSet.insert(header);
}


SCEV::SCEV(Value* initial, Value* step)
{
    scevBody.push_back(initial);
    scevBody.push_back(step);
}

SCEV::SCEV(Value* initial, const SCEV& innerSCEV)
{
    scevBody.push_back(initial);

    for(auto iter = innerSCEV.scevBody.begin(); iter!= innerSCEV.scevBody.end(); iter++)

    for(auto val : innerSCEV.scevBody)
        scevBody.push_back(val);

    std::set_union(instructionsHasBeenCaculated.begin(), instructionsHasBeenCaculated.end(),
                    innerSCEV.instructionsHasBeenCaculated.begin(), innerSCEV.instructionsHasBeenCaculated.end(),
                    std::inserter(instructionsHasBeenCaculated, instructionsHasBeenCaculated.begin()));
}

SCEV::SCEV(const std::vector<Value*>& vec)
{
    for(auto val : vec)
        scevBody.push_back(val);
}

SCEV::SCEV(Value* initial, Value* step, std::set<BinaryInst*>&& binarys) 
    : instructionsHasBeenCaculated(binarys)
{
    scevBody.push_back(initial);
    scevBody.push_back(step);
}

using Kind = Instruction::InstrKind;
SCEV operator+(Value* lhs, const SCEV& rhs)
{
    auto add = new BinaryInst(lhs->getType(), Kind::Add, lhs, rhs.at(0), nullptr, "scevtmpadd");
    std::vector<Value*> initialVals;
    initialVals.push_back(add);
    for(int i = 1; i < rhs.size(); i++)
        initialVals.push_back(rhs.at(i));
    auto res = SCEV(initialVals);
    res.instructionsHasBeenCaculated = rhs.instructionsHasBeenCaculated;
    res.instructionsHasBeenCaculated.insert(add);
    return res;
}

SCEV operator+(const SCEV& lhs, const SCEV& rhs)
{
    auto maxSize = std::max(lhs.size(), rhs.size());
    std::vector<BinaryInst*> binaryHasBeenCreated;
    std::vector<Value*> initialVals;
    for(auto i = 0; i < maxSize; i++)
    {
        if(i < lhs.size() && i < rhs.size())
        {
            if(auto clhs = dynamic_cast<ConstantInt*>(lhs.at(i)))
                if(auto crhs = dynamic_cast<ConstantInt*>(rhs.at(i)))
                {
                    initialVals.push_back(ConstantInt::get(32, clhs->getValue() + crhs->getValue()));
                    continue;
                }
            auto add = new BinaryInst(lhs.at(i)->getType(), Kind::Add, lhs.at(i), rhs.at(i), nullptr, "scevtmpadd");
            binaryHasBeenCreated.push_back(add);
            initialVals.push_back(add);
        }
        else if(i < lhs.size())
            initialVals.push_back(lhs.at(i));
        else if(i < rhs.size())
            initialVals.push_back(rhs.at(i));
    }

    auto res = SCEV(initialVals);
    std::set_union(lhs.instructionsHasBeenCaculated.begin(), lhs.instructionsHasBeenCaculated.end(),
                    rhs.instructionsHasBeenCaculated.begin(), rhs.instructionsHasBeenCaculated.end(),
                    std::inserter(res.instructionsHasBeenCaculated, res.instructionsHasBeenCaculated.begin()));
    for(auto binary : binaryHasBeenCreated)
        res.instructionsHasBeenCaculated.insert(binary);

    return res;
}


SCEV operator-(const SCEV& lhs, Value* rhs)
{
    auto sub = new BinaryInst(rhs->getType(), Kind::Sub, lhs.at(0), rhs, nullptr, "scevtmpadd");
    std::vector<Value*> initialVals;
    initialVals.push_back(sub);
    for(int i = 1; i < lhs.size(); i++)
        initialVals.push_back(lhs.at(i));
    auto res = SCEV(initialVals);
    res.instructionsHasBeenCaculated = lhs.instructionsHasBeenCaculated;
    res.instructionsHasBeenCaculated.insert(sub);
    return res;
}

SCEV operator-(const SCEV& lhs, const SCEV& rhs)
{
    auto maxSize = std::max(lhs.size(), rhs.size());
    std::vector<BinaryInst*> binaryHasBeenCreated;
    std::vector<Value*> initialVals;
    for(auto i = 0; i < maxSize; i++)
    {
        if(i < lhs.size() && i < rhs.size())
        {
            if(auto clhs = dynamic_cast<ConstantInt*>(lhs.at(i)))
                if(auto crhs = dynamic_cast<ConstantInt*>(rhs.at(i)))
                {
                    initialVals.push_back(ConstantInt::get(32, clhs->getValue() - crhs->getValue()));
                    continue;
                }
            auto add = new BinaryInst(lhs.at(i)->getType(), Kind::Sub, lhs.at(i), rhs.at(i), nullptr, "scevtmpadd");
            binaryHasBeenCreated.push_back(add);
            initialVals.push_back(add);
        }
        else if(i < lhs.size())
            initialVals.push_back(lhs.at(i));
        else if(i < rhs.size())
            initialVals.push_back(rhs.at(i));
    }

    auto res = SCEV(initialVals);
    std::set_union(lhs.instructionsHasBeenCaculated.begin(), lhs.instructionsHasBeenCaculated.end(),
                    rhs.instructionsHasBeenCaculated.begin(), rhs.instructionsHasBeenCaculated.end(),
                    std::inserter(res.instructionsHasBeenCaculated, res.instructionsHasBeenCaculated.begin()));
    for(auto binary : binaryHasBeenCreated)
        res.instructionsHasBeenCaculated.insert(binary);

    return res;
}

SCEV operator*(Value* lhs, const SCEV& rhs)
{
    std::vector<BinaryInst*> binaryHasBeenCreated;
    std::vector<Value*> initialVals;
    for(auto initem : rhs.scevBody)
    {
        if(auto clhs = dynamic_cast<ConstantInt*>(lhs))
            if(auto crhs = dynamic_cast<ConstantInt*>(initem))
            {
                initialVals.push_back(ConstantInt::get(32, clhs->getValue() * crhs->getValue()));
                continue;
            }
        auto mul = new BinaryInst(lhs->getType(), Kind::Mul, lhs, initem, nullptr, "scevtmpmul");
        binaryHasBeenCreated.push_back(mul);
        initialVals.push_back(mul);
    }

    auto res = SCEV(initialVals);
    res.instructionsHasBeenCaculated = rhs.instructionsHasBeenCaculated;

    for(auto binary : binaryHasBeenCreated)
        res.instructionsHasBeenCaculated.insert(binary);
    
    return res;

}
// TODO DO the SCEV at the higher dimension
// SCEV operator*(const SCEV& lhs, const SCEV& rhs)
// {

// }

void SCEV::getItemChains(unsigned i, std::vector<BinaryInst*>& instrChain)
{
    auto value = scevBody.at(i);
    if(auto instr = dynamic_cast<BinaryInst*>(value); instr 
        && instructionsHasBeenCaculated.count(instr))
    {
        // Post Order
        std::function<void(BinaryInst*)> postTravel = [&](BinaryInst* binary) -> void {
            // PrintMsg(binary->getSignature());
            if(auto lhs = dynamic_cast<BinaryInst*>(binary->getLhs()); lhs
                    && instructionsHasBeenCaculated.count(lhs))
                postTravel(lhs);
        
            
            if(auto rhs = dynamic_cast<BinaryInst*>(binary->getRhs()); rhs
                    && instructionsHasBeenCaculated.count(rhs))
                postTravel(rhs);

            
            instrChain.push_back(binary);
        };

        postTravel(instr);
    }
}

void SCEV::clear()
{
    for(auto bin : instructionsHasBeenCaculated)
    {
        if(!bin->getBasicBlock())
            bin->UnUseAll();
    }
    instructionsHasBeenCaculated.clear();
    scevBody.clear();
}

SCEV& Loop::getSCEV(Instruction* instr)
{
    return SCEVCheck[instr];
}

bool Loop::hasSCEV(Instruction* instr)
{
    return SCEVCheck.count(instr);
}

void Loop::registerSCEV(Instruction* instr, SCEV scev)
{
    SCEVCheck[instr] = scev;
}

bool Loop::isLoopInvariant(Value* val)
{
    if(auto instr = dynamic_cast<Instruction*>(val))
    {
        auto bb = instr->getBasicBlock();
        return !this->BBSet.count(bb);
    }
    return true;
}

} // namespace ir
    
} // namespace sysy
