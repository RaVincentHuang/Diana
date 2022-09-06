#include "sysy/IR/Module.h"
#include "sysy/IR/Function.h"
#include "sysy/IR/BasicBlock.h"
#include "sysy/IR/Instruction.h"
#include "sysy/IR/Instructions.h"
#include "sysy/IR/Type.h"
#include "sysy/IR/IRDump.h"
#include "sysy/IR/Loop.h"
#include "sysy/Support/strUtils.h"
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace sysy
{
namespace ir
{

// TODO Maybe use the unordered_map could be better.
static std::multiset<std::string> variableCounter;
static std::multiset<std::string> labelCounter;

static std::map<BasicBlock*, std::string> labelName;
static std::map<Instruction*, std::string> variableName;

std::string to_hex(float f)
{
    std::stringstream ss;
    ss << std::hexfloat << f;
    std::string res;
    ss >> res;
    return res; 
}

static std::string checkLabel(BasicBlock* bb, const std::string& label)
{
    auto num = labelCounter.count(label);
    labelCounter.insert(label);
    auto res = (num) ? label + std::to_string(num) : label;
    labelName[bb] = res;
    return res;
}

static std::string getLabel(BasicBlock* bb)
{
    if(!labelName.count(bb))
        return checkLabel(bb, bb->getName());
    return labelName[bb];
}

static std::string checkVariable(Instruction* instr, const std::string& variable)
{
    auto num = variableCounter.count(variable);
    variableCounter.insert(variable);
    auto res = (num) ? variable + std::to_string(num) : variable;
    variableName[instr] = res;
    return res;
}

static std::string getVariable(Instruction* instr)
{
    if(!variableName.count(instr))
        return checkVariable(instr, instr->getName());
    return variableName[instr];
}

static std::string nameIs(Value* val)
{
    std::string res;
    if(auto instr = dynamic_cast<Instruction*>(val))
        res = "%" + getVariable(instr);
    else if(auto c = dynamic_cast<Constant*>(val))
        res = c->getSignature();
    else if(auto G = dynamic_cast<GlobalValue*>(val))
        res = "@" + G->getName();
    else if(auto arg = dynamic_cast<Function::Arg*>(val))
        res = "%" + arg->getName();
    else if(auto undef = dynamic_cast<UndefValue*>(val))
        res = "undef";
    return res;
}

std::string ConstantInt::getSignature() 
{
    int x = value;
    return std::to_string(x);
}

std::string ConstantFP::getSignature() 
{
    return std::to_string(value);
}

std::string ConstantArray::getSignature() 
{
    if(zeroinitializer)
        return " zeroinitializer ";
    std::string str = "[";
    for(auto iter = items.begin(); iter != items.end(); iter++)
    {
        if(iter != items.begin())
            str += ", ";
        str += (*iter)->getType()->getSignature() + " " + (*iter)->getSignature();
    }
    // for(auto item : items)
    //     str += item->getType()->getSignature() + " " + item->getSignature() + ", ";
    str += "]";
    return str;
}

std::string ConstantString::getSignature() 
{
    return "c\"\\22" + str + "\\22\\00\"";
}

static std::string UnaryOps[] = { "neg", "not", "fneg", "fptosi", "sitofp", "bitcast" };
std::string UnaryInstruction::getSignature() 
{
    std::string str = "%" + getVariable(this);
    str += " = " + UnaryOps[getKind() - Neg];
    if(getKind() == BitCast || getKind() == SIToFP || getKind() == FPToSI)
    {
        str += " " + getValue()->getType()->getSignature() + "  " + nameIs(getValue());
        str += " to " + getType()->getSignature();
        return str;
    }
    str += " " + getValue()->getType()->getSignature() + " " + nameIs(getValue());
    return str;
}

std::string StoreInstruction::getSignature() 
{
    std::string str = "store ";
    str += getValue()->getType()->getSignature() + " " + nameIs(getValue()) + ", ";
    str += getPtr()->getType()->getSignature() + " " + nameIs(getPtr()) + ", align 4";
    return str;
}

std::string RetInstruction::getSignature() 
{
    std::string str = "ret";
    if(!getRet())
        return str + " void";
    str += " " + getRet()->getType()->getSignature() + " " + nameIs(getRet());
    return str;
}

std::string LoadInstruction::getSignature() 
{
    std::string str = "%" + getVariable(this);
    str += " = load " + getType()->getSignature() + ", ";
    str += getPtr()->getType()->getSignature() + " " + nameIs(getPtr());
    if(auto store = getStore())
        str += ", store is " + store->getSignature();
    return str;
}

std::string BrInstruction::getSignature() 
{
    std::string str = "br ";
    if(getCond())
        str += getCond()->getType()->getSignature() + " " + nameIs(getCond()) 
            + ", label %" + getLabel(getTrueBlock())
            + ", label %" + getLabel(getFalseBlock());
    else
        str += "label %" + getLabel(getTrueBlock());
    return str;
}

std::string AllocaInstruction::getSignature() 
{
    std::string str = "%" + getVariable(this) + " = alloca ";
    str += getAllocatedType()->getSignature() + ", align 4";
    return str; 
}

static std::string binaryOps[] = { "add", "sub", "mul", "sdiv", "srem", "icmp eq", 
                            "icmp ne", "icmp slt", "icmp sgt", "icmp sle", "icmp sge",
                            "ashr", "lshr", "shl", "fadd", "fsub", "fmul", "fdiv",
                            "frem", "fcmp ueq", "fcmp une", "fcmp ult", "fcmp ugt", 
                            "fcmp ule", "fcmp uge"};
std::string BinaryInstrcution::getSignature() 
{
    std::string str = "%" + getVariable(this);
    str += " = " + binaryOps[getKind()] + " " +  getLhs()->getType()->getSignature();
    str += " " + nameIs(getLhs()) + ",";
    str += " " + nameIs(getRhs());
    return str;
}

std::string GlobalValue::getSignature() 
{
    std::string str = "@" + getName() + " = global ";
    str += getValueType()->getSignature() + " " + initial->getSignature();
    return str;
}

std::string CallInstruction::getSignature() 
{
    std::string str = "";
    if(!getType()->isVoidTy())
        str += "%" + getVariable(this) + " = ";
    str += "call " + getCallee()->getType()->getSignature();
    str += " @" + getCallee()->getName() + "(";
    for(auto iter = getArgs().begin(); iter != getArgs().end(); iter++)
    {
        if(iter != getArgs().begin())
            str += ", ";
        str += (*iter)->getType()->getSignature() + " " + nameIs(*iter);
    }

    str += ")";
    return str;
}

std::string GEPInstruction::getSignature() 
{
    std::string str = "%" + getVariable(this) + " = ";
    str += "getelementptr inbounds " + getPtr()->getType()->getInnerType()->getSignature() + ", ";
    if(!getPtr()->getType()->isPointerTy())
        str += PointerType::get(getPtr()->getType())->getSignature();
    else
        str += getPtr()->getType()->getSignature();
    str += " " + nameIs(getPtr()) + ", ";
    for(auto iter = getIndex().begin(); iter != getIndex().end(); iter++)
    {
        if(iter != getIndex().begin())
            str += ", ";
        str += (*iter)->getType()->getSignature() + " " + nameIs(*iter);
    }
    // for(auto item : getIndex())
    //     str += item->getType()->getSignature() + " " + nameIs(item) + ", ";
    return str;
}

std::string PhiInstruction::getSignature()
{
    std::string str = "%" + getVariable(this) + " = phi " + getType()->getSignature() + " ";
    for(auto iter = getPredecessors().begin(); iter != getPredecessors().end(); ++iter)
    {
        auto val = (*iter).first;
        auto bb = (*iter).second;
        if(iter != getPredecessors().begin())
            str += ", ";
        str += "[" + nameIs(val) + ", " + "%" + getLabel(bb) + "]";
    }
    return str;
}

std::string BasicBlock::getSignature() 
{
    std::string str = getLabel(this) + ":\t\t\t\t";
    if(!predecessor.empty())
    {
        str += "; preds = ";
        for(auto bb : predecessor)
            str += "%" + getLabel(bb) + ", ";
    }
    str += "\n";
    for(auto instr : instructions)
    {
        str += "  " + instr->getSignature() + "\n";
    }
    if(!successor.empty())
    {
        str += "\t\t\t\t; succ = ";
        for(auto bb : successor)
            str += "%" + getLabel(bb) + ", ";
        str += "\n";
    }
    return str;
}

std::string Function::getSignature() 
{
    // variableCounter.clear();
    // labelCounter.clear();
    std::string str = getType()->getSignature() + " @" + getName() + "(";
    for(auto iter = args.begin(); iter != args.end(); iter++)
    {
        if(iter != args.begin())
            str += ", ";
        if(isDeclare())
            str += (*iter)->getType()->getSignature();
        else
            str += (*iter)->getSignature();
    } 
    // for(auto& arg : args)
    //     str += arg->getSignature() + ", ";
    str += ")";
    if(!getBasicBlockList().empty())
    {
        str += " {\n";
        for(auto bb : getBasicBlockList())
        {
            str += bb->getSignature() + "\n";
        }
        str += "}";
    }
    return str;
}

void dumpFunction(Function* func)
{
    // std::cout << "define " + func->getSignature() << std::endl;
}

void dumpBasickBlock(BasicBlock* bb)
{
    // std::cout << bb->getSignature() << std::endl;
}

std::string dumpIR(Module* mod)
{
    std::string str = "";
    for(auto G : mod->__globalValue)
        str += G->getSignature() + "\n\n";
    for(auto func : mod->__function)
        if(func->isDeclare())
            str += "declare " + func->getSignature() + "\n\n";
        else
            str += "define " + func->getSignature() + "\n\n";
    std::cout << str << std::endl;

    return str;
}

std::string MoveInst::getSignature()
{
    return "move " + nameIs(getIncomingVal()) 
        + " to " + nameIs(getTarget());
}


std::string MemPhiInst::getSignature()
{
    std::string str = "%" + getVariable(this) + " = memphi " + getType()->getSignature() + " ";
    for(auto iter = getPredecessors().begin(); iter != getPredecessors().end(); ++iter)
    {
        auto val = (*iter).first;
        auto bb = (*iter).second;
        if(iter != getPredecessors().begin())
            str += ", ";
        str += "[" + nameIs(val) + ", " + "%" + getLabel(bb) + "]";
    }
    return str;
}

std::string DeadInst::getSignature()
{
    if(!getOprands().empty())
        return  "load depend " + getOprands().at(0)->getValue()->getSignature();
    else
        return "dead instr";
}

std::string SCEV::getSignature()
{
    std::string str = "{";
    for(int i = 0; i < size(); i++)
    {
        if(i != 0)
            str += ", +, ";
        str += nameIs(at(i));
    }
    str += "}\n";
    if(!instructionsHasBeenCaculated.empty())
        str += "\t\tRelated Instruction is\n";
    for(auto instr : instructionsHasBeenCaculated)
        str += "\t\t" + instr->getSignature() + "\n";
    return str;
}

const std::string runtime = "runtime/sylib.ll";

void llFileGen(Module* mod)
{

    std::string str = "";
    for(auto G : mod->__globalValue)
        str += G->getSignature() + "\n\n";
    for(auto func : mod->__function)
        if(func->isDeclare())
            str += "declare " + func->getSignature() + "\n\n";
        else
            str += "define " + func->getSignature() + "\n\n";

    std::ofstream ofs;						//定义流对象
    ofs.open("a.ll", std::ios::out);		//以写的方式打开文件
    ofs << str << std::endl;
    ofs.close();
}

} // namespace ir

} // namespace sysy
