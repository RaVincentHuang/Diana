#include "sysy/IR/Instruction.h"
#include "sysy/Target/Instruction.h"
#include "sysy/Target/Reg.h"
#include "sysy/Target/Block.h"
#include "sysy/Target/Function.h"

namespace sysy 
{
namespace mc 
{
std::map<Inst::InstrType, std::string> Inst::type_map = {
    {BinaryReg, "BinaryReg"},
    {BinaryImm, "BinaryImm"},
    {ComplexMul, "ComplexMul"},
    {MoveReg, "MoveReg"},
    {ShiftInstr, "ShiftInstr"},
    {MoveImm, "MoveImm"},
    {CompareReg, "CompareReg"},
    {CompareImm, "CompareImm"},
    {Branch, "Branch"},
    {Push, "Push"},
    {Pop, "Pop"},
    {Load, "Load"},
    {LoadGlobal, "LoadGlobal"},
    {Store, "Store"},
    {Call, "Call"},
    {Return, "Return"},
    {Convert, "Convert"}
};

std::map<Shift::Type, std::string> Shift::type_map = {
    {LSL, "lsl"}, {LSR, "lsr"}, {ASR, "asr"}, {ROR, "ror"}
};

std::map<BinaryReg::Type, std::string> BinaryReg::type_map = {
    {ADD, "add"}, {SUB, "sub"}, {RSB, "rsb"}, {MUL, "mul"}, {SDIV, "sdiv"}, 
    {VADD, "vadd"}, {VSUB, "vsub"}, {VMUL, "vmul"}, {VDIV, "vdiv"}
};

std::map<BinaryImm::Type, std::string> BinaryImm::type_map = {
    {ADD, "add"}, {SUB, "sub"}, {RSB, "rsb"}
};

std::map<ComplexMul::Type, std::string> ComplexMul::type_map = {
    {MLA, "mla"}, {MLS, "mls"}
};

std::map<MoveReg::Type, std::string> MoveReg::type_map = {
    {MOV, "mov"}, {MVN, "mvn"}, {VVMOV, "vmov"}, {SVMOV, "vmov"}
};

// std::map<MoveImm::Type, std::string> MoveImm::typeMap = {
//     {MOV, "mov"}, {MVN, "mvn"}, {MOVT, "movt"}, {MOVW, "movw"}
// };

std::map<CompareReg::Type, std::string> CompareReg::type_map = {
    {CMP, "cmp"}, {CMN, "cmn"}, {VCMP, "vcmp"}
};

std::map<CompareImm::Type, std::string> CompareImm::type_map = {
    {CMP, "cmp"}, {CMN, "cmn"}
};

std::map<Push::Type, std::string> Push::type_map = {
    {SPUSH, "push"}, {VPUSH, "vpush"}
};

std::map<Pop::Type, std::string> Pop::type_map = {
    {SPOP, "pop"}, {VPOP, "vpop"}
};

std::map<LoadImm::Type, std::string> LoadImm::type_map = {
    {SLOAD, "ldr"}, {VLOAD, "vldr"}
};

std::map<LoadReg::Type, std::string> LoadReg::type_map = {
    {SLOAD, "ldr"}, {VLOAD, "vldr"}
};

std::map<StoreImm::Type, std::string> StoreImm::type_map = {
    {SSTORE, "str"}, {VSTORE, "vstr"}
};

std::map<StoreReg::Type, std::string> StoreReg::type_map = {
    {SSTORE, "str"}, {VSTORE, "vstr"}
};

std::map<Instruction::CondType, std::string> Instruction::condMap = {
    {AL, ""}, {EQ, "eq"}, {NE, "ne"}, {GE, "ge"}, {GT, "gt"}, {LE, "le"}, {LT, "lt"}
};

// std::map<ir::Instruction::InstrKind, Instruction::CondType> Instruction::ir_cond_map = {
//     {ICmpEQ, EQ}, {ICmpNE, NE}, {ICmpSGE, GE}, {ICmpSGT, GT}, {ICmpSLE, LE}, {ICmpSLT, LT}
// };

Call::Call(Function* _callee) : callee(_callee), Inst(Inst::Call) {
    use_reg = {};
    def_reg = {};
    if (_callee->name == "putfloat")
        use_reg.push_back(new Reg(0, Reg::PHY, Reg::VREG));
    // else
    if (_callee->name == "starttime" || _callee->name == "stoptime")
        use_reg.push_back(new Reg(0, Reg::PHY, Reg::SREG));    

    for (int i = 0; i < std::min(_callee->arg_num, 4); i++)
        use_reg.push_back(new Reg(i, Reg::PHY));
    
    if (_callee->name == "getfloat")
        def_reg.push_back(new Reg(0, Reg::PHY, Reg::VREG));        
    // else { 
    for (int i = 0; i < 4; i++)
        def_reg.push_back(new Reg(i, Reg::PHY));
    def_reg.push_back(new Reg(12, Reg::PHY));
    def_reg.push_back(new Reg(14, Reg::PHY));
    // }
}


std::string BinaryReg::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond];
    if (type == VADD || type == VSUB || type == VMUL || type == VDIV)
        code += ".f32";
    code += "\t" + dst->getSignature() + ",\t";
    code += lhs->getSignature() + ",\t" + rhs->getSignature();
    if (shift && shift->imm)
        code += ",\t" + Shift::type_map[shift->type] + " #" + std::to_string(shift->imm);
    code += "\n";

    return code;
}   

std::string BinaryImm::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond] + "\t";
    code += dst->getSignature() + ",\t";
    code += lhs->getSignature() + ", #" + std::to_string(rhs);
    code += "\n";

    return code;
}

std::string ComplexMul::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond] + "\t";
    code += dst->getSignature() + ",\t";
    code += lhs->getSignature() + ", ";
    code += rhs->getSignature() + ", " + extra->getSignature();
    code += "\n";

    return code;
}

std::string MoveReg::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond]; 
    if (type == VVMOV) 
        code += ".f32";
    code += " " + dst->getSignature() + ",\t" + lhs->getSignature();
    code += "\n";

    return code;
}   

std::string ShiftInstr::gen_asm() {
    std::string code = "\t";
    code += "mov" + condMap[cond] + "\t"; 
    code += dst->getSignature() + ",\t" + lhs->getSignature() + ",\t";
    code += Shift::type_map[shift->type] + " #" + std::to_string(shift->imm);
    code += "\n";

    return code;
}

std::string MoveImm::gen_asm() {
    std::string code = "\t";
    int imm = lhs;
    if (Function::analyseImm(imm)) {
        code += "mov" + condMap[cond] + "\t";    
        code += dst->getSignature() + ", #" + std::to_string(imm);
    }
    else if (Function::analyseImm(~imm)) {
        code += "mvn" + condMap[cond] + "\t";    
        code += dst->getSignature() + ", #" + std::to_string(~imm);        
    }
    else {
        uint t = (uint)imm;
        code += "movw" + condMap[cond] + "\t";    
        code += dst->getSignature() + ", #" + std::to_string(t & 0xffff);   
        if (t >> 16) {
            code += "\n\tmovt" + condMap[cond] + "\t";    
            code += dst->getSignature() + ", #" + std::to_string(t >> 16);   
        }
    }
    code += "\n";

    return code;
}

std::string CompareReg::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond];
    if (type == VCMP)
        code += ".f32";
    code += " " + lhs->getSignature() + ",\t" + rhs->getSignature();
    code += "\n";


    if (type == VCMP)
        code += "\tvmrs\tAPSR_nzcv, fpscr\n";


    return code;
}

std::string CompareImm::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond] + "\t";
    code += lhs->getSignature() + ", #" + std::to_string(rhs);
    code += "\n";

    return code;
}

std::string Branch::gen_asm() {
    std::string code = "\t";
    code += "b" + condMap[cond] + "\t";
    code += target->name;
    code += "\n";

    return code;
}

std::string Push::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + "\t{";
    for (auto& reg : regs) {
        code += reg->getSignature();
        if (&reg != &regs.back())
            code += ", ";
    }
    code += "}\n";

    return code;
}

std::string Pop::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + "\t{";
    for (auto& reg : regs) {
        code += reg->getSignature();
        if (&reg != &regs.back())
            code += ", ";
    }
    code += "}\n";

    return code;
}

std::string LoadImm::gen_asm() {
    std::string code = "\t";

    code += type_map[type] + condMap[cond] + "\t";
    code += dst->getSignature() + ", [";
    code += lhs->getSignature() + ", #";
    code += std::to_string(offset) + "]";
    code += "\n";

    return code;
}

std::string LoadReg::gen_asm() {
    std::string code = "\t";

    code += type_map[type] + condMap[cond] + "\t";
    code += dst->getSignature() + ", [";
    code += lhs->getSignature() + ", " + rhs->getSignature();
    if (shift && shift->imm)
        code += ", " + Shift::type_map[shift->type] + " #" + std::to_string(shift->imm);
    code += "]\n";

    return code;
}

std::string StoreImm::gen_asm() {
    std::string code = "\t";
    code += type_map[type] + condMap[cond] + "\t";
    code += lhs->getSignature() + ", [";
    code += dst->getSignature() + ", #";
    code += std::to_string(offset) + "]";
    code += "\n";

    return code;
} 

std::string StoreReg::gen_asm() {
    std::string code = "\t";

    code += type_map[type] + condMap[cond] + "\t";
    code += lhs->getSignature() + ", [";
    code += dst->getSignature() + ", " + rhs->getSignature();
    if (shift && shift->imm)
        code += ", " + Shift::type_map[shift->type] + " #" + std::to_string(shift->imm);
    code += "]\n";

    return code;
}

std::string LoadGlobal::gen_asm() {
    std::string code = "";
    code += "\tmovw" + condMap[cond] + "\t";
    code += dst->getSignature() + ", :lower16: " + name + "\n";
    code += "\tmovt" + condMap[cond] + "\t";
    code += dst->getSignature() + ", :upper16: " + name + "\n";

    return code;
}

std::string Call::gen_asm() {
    std::string code = "\t";
    code += "bl\t";
    if (callee->name == "starttime") 
        code += "_sysy_starttime";
    else if (callee->name == "stoptime") 
        code += "_sysy_stoptime";
    else
        code += callee->name;

    code += "\n";
    
    return code;
}

std::string Return::gen_asm() {
    std::string code = "\t";
    code += "bx\tlr\n";

    return code;
}

std::string LoadFP::gen_asm() {
    std::string code = "\t";
    code += "vldr" + condMap[cond] + "\t";
    code += dst->getSignature() + ", " + name;
    code += "\n";

    return code;
}

std::string Convert::gen_asm() {
    std::string code = "\t";
    if (type == Convert::STOV)
        code += "vcvt" + condMap[cond] + ".f32.s32\t";
    else
        code += "vcvt" + condMap[cond] + ".s32.f32\t";
    code += " " + dst->getSignature() + ", " + src->getSignature();
    code += "\n";

    return code;
}

std::string Bic::gen_asm() {
    std::string code = "\t";
    code += "bic\t" + dst->getSignature() + ",\t";
    code += lhs->getSignature() + ", #" + std::to_string(imm);
    code += "\n";

    return code;
}


} // namespace mc 
} // namespace sysy