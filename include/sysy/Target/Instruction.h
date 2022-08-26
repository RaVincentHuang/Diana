#ifndef SYSY_TARGET_INSTRUCTION_H
#define SYSY_TARGET_INSTRUCTION_H

#include "sysy/Target/Reg.h"
#include <string>
#include <map>
#include <vector>

namespace sysy 
{
namespace mc 
{

class Function;
class Block;

class Shift {
public:
    enum Type { LSL, LSR, ASR, ROR };
    Type type;
    int imm;

    static std::map<Type, std::string> type_map;

    Shift() : type(LSL), imm(0) {}
    Shift(Type _type, int _imm) : type(_type), imm(_imm) {}
};

class Instruction {
public:
    enum InstrType {
        BinaryReg,
        BinaryImm,
        ComplexMul,
        MoveReg,
        ShiftInstr,
        MoveImm,
        CompareReg,
        CompareImm,
        Branch,
        Push,
        Pop,
        Load,
        Store,
        LoadGlobal,
        Call,
        Return,
        LoadFP,
        Convert,
        Bic,
        None
    };
    InstrType instr_type;

    enum CondType { AL, EQ, NE, GE, GT, LE, LT};
    CondType cond;

    bool is_float = false;

    std::vector<Reg*> def_reg;
    std::vector<Reg*> use_reg;

    static std::map<InstrType, std::string> type_map;

    bool def(int id) {
      for (auto r : def_reg)
        if (r->kind==Reg::SREG&&r->id == id&&r->type==Reg::VIRTUAL) return true;
      return false;
    }
    bool use(int id) {
      for (auto r : use_reg)
        if (r->kind==Reg::SREG&&r->id == id&&r->type==Reg::VIRTUAL) return true;
      return false;
    }
    bool def_v(int id) {
      for (auto r : def_reg)
        if (r->kind==Reg::VREG&&r->id == id&&r->type==Reg::VIRTUAL) return true;
      return false;
    }
    bool use_v(int id) {
      for (auto r : use_reg)
        if (r->kind==Reg::VREG&&r->id == id&&r->type==Reg::VIRTUAL) return true;
      return false;
    }
    void replace_reg(int before, int after) {
        for (auto i : def_reg)
            if (i->id == before) i->id = after;
        for (auto j : use_reg)
            if (j->id == before) j->id = after;
    }
    static std::map<CondType, std::string> condMap;
    // static std::map<ir::Instruction::InstrKind, Instruction::CondType> ir_cond_map;

    Instruction(InstrType _type, std::vector<Reg*> _def_reg, std::vector<Reg*> _use_reg) 
        : instr_type(_type), cond(AL), def_reg(_def_reg), use_reg(_use_reg) {}

    Instruction(InstrType _type) : instr_type(_type), cond(AL), def_reg({}), use_reg({}) {}

    void setCond(CondType _cond) { cond = _cond; }

    virtual std::string gen_asm() = 0;
};

using Inst = Instruction;

class BinaryReg : public Inst {
public:
    enum Type { ADD, SUB, RSB, MUL, SDIV, VADD, VSUB, VMUL, VDIV };
    Type type;
    Reg* dst,* lhs,* rhs;
    Shift* shift;

    static std::map<Type, std::string> type_map;

    BinaryReg(Type _type, Reg* _dst, Reg* _lhs, Reg* _rhs) 
        : type(_type), dst(_dst), lhs(_lhs), rhs(_rhs), shift(),
        Inst(Inst::BinaryReg, {_dst}, {_lhs, _rhs}) {
        if (_type == VADD || _type == VSUB || _type == VMUL || _type == VDIV)
            is_float = true;        
    }

    void setShift(Shift* _shift) { shift = _shift; }
    virtual std::string gen_asm() override;
};

class BinaryImm : public Inst {
public:
    enum Type { ADD, SUB, RSB };
    Type type;
    Reg* dst,* lhs;
    int rhs;

    static std::map<Type, std::string> type_map;

    BinaryImm(Type _type, Reg* _dst, Reg* _lhs, int _rhs) 
        : type(_type), dst(_dst), lhs(_lhs), rhs(_rhs),
        Inst(Inst::BinaryImm, {_dst}, {_lhs}) {}

    virtual std::string gen_asm() override;
};

class ComplexMul : public Inst {
public:
    enum Type { MLA, MLS };
    Type type;
    Reg* dst,* lhs,* rhs,* extra;

    static std::map<Type, std::string> type_map;

    ComplexMul(Type _type, Reg* _dst, Reg* _lhs, Reg* _rhs, Reg* _extra) 
        : type(_type), dst(_dst), lhs(_lhs), rhs(_rhs), extra(_extra),
        Inst(Inst::ComplexMul, {_dst}, {_lhs, _rhs, _extra}) {}

    virtual std::string gen_asm() override;
};

class MoveReg : public Inst {
public:
    enum Type { MOV, MVN, VVMOV, SVMOV };
    Type type;
    Reg* dst,* lhs;

    static std::map<Type, std::string> type_map;

    MoveReg(Type _type, Reg* _dst, Reg* _lhs) 
        : type(_type), dst(_dst), lhs(_lhs), 
        Inst(Inst::MoveReg, {_dst}, {_lhs}) {
        if (_type == VVMOV || _type == SVMOV)
            is_float = true;
    }
    virtual std::string gen_asm() override;
};

class ShiftInstr : public Inst {
public:
    Reg* dst,* lhs;
    Shift* shift;

    ShiftInstr(Reg* _dst, Reg* _lhs, Shift* _shift) : dst(_dst), lhs(_lhs), shift(_shift), 
        Inst(Inst::ShiftInstr, {_dst}, {_lhs}) {}
    void setShift(Shift* _shift) { shift = _shift; }
    virtual std::string gen_asm() override;
};

class MoveImm : public Inst {
public:
    // enum Type { MOV, MVN };
    // Type type;
    Reg* dst;
    int lhs;

    // static std::map<Type, std::string> type_map;

    MoveImm(Reg* _dst, int _lhs) : dst(_dst), lhs(_lhs), 
        Inst(Inst::MoveImm, {_dst}, {}) {}
    virtual std::string gen_asm() override;
};

class CompareReg : public Inst {
public:
    enum Type { CMP, CMN, VCMP };
    Type type;
    Reg* lhs,* rhs;

    static std::map<Type, std::string> type_map;

    CompareReg(Type _type, Reg* _lhs, Reg* _rhs) : type(_type), lhs(_lhs), rhs(_rhs),
        Inst(Inst::CompareReg, {}, {_lhs, _rhs}) {
        if (_type == VCMP)
            is_float = true;
    }
    virtual std::string gen_asm() override;
};

class CompareImm : public Inst {
public:
    enum Type { CMP, CMN };
    Type type;
    Reg* lhs;
    int rhs;

    static std::map<Type, std::string> type_map;

    CompareImm(Type _type, Reg* _lhs, int _rhs) : type(_type), lhs(_lhs), rhs(_rhs), 
        Inst(Inst::CompareImm, {}, {_lhs}) {}
    virtual std::string gen_asm() override;
};

class Branch : public Inst {
public:
    Block* target;
    
    Branch(Block* _target) : target(_target),
        Inst(Inst::Branch, {}, {}) {}
    virtual std::string gen_asm() override;
};

class Push : public Inst {
public:
    enum Type { SPUSH, VPUSH };
    Type type;
    std::vector<Reg*> regs;

    static std::map<Type, std::string> type_map;

    Push(Type _type, std::vector<Reg*> _regs) : type(_type), regs(_regs),
        Inst(Inst::Push, {}, _regs) {}
    virtual std::string gen_asm() override;
};

class Pop : public Inst {
public:
    enum Type { SPOP, VPOP };
    Type type;
    std::vector<Reg*> regs;

    static std::map<Type, std::string> type_map;

    Pop(Type _type, std::vector<Reg*> _regs) : type(_type), regs(_regs),
        Inst(Inst::Pop, {}, _regs) {}
    virtual std::string gen_asm() override;
};

class LoadImm : public Inst {
public:
    enum Type { SLOAD, VLOAD }; 
    Type type;
    Reg* dst,* lhs;
    int offset;

    static std::map<Type, std::string> type_map;

    LoadImm() : Inst(Inst::None) {}
    LoadImm(Type _type, Reg* _dst, Reg* _lhs, int _offset) 
        : type(_type), dst(_dst), lhs(_lhs), offset(_offset), 
        Inst(Inst::Load, {_dst}, {_lhs}){
        if (_type == VLOAD)
            is_float = true;
    }
    virtual std::string gen_asm() override;
};

class LoadReg : public Inst {
public:
    enum Type { SLOAD, VLOAD }; 
    Type type;
    Reg *dst, *lhs, *rhs;
    Shift* shift;

    static std::map<Type, std::string> type_map;

    LoadReg() : Inst(Inst::None) {}
    LoadReg(Type _type, Reg* _dst, Reg* _lhs, Reg* _rhs) 
        : type(_type), dst(_dst), lhs(_lhs), rhs(_rhs), shift(),
        Inst(Inst::Load, {_dst}, {_lhs, _rhs}){
        if (_type == VLOAD)
            is_float = true;
    }
    void setShift(Shift* _shift) { shift = _shift; }
    virtual std::string gen_asm() override;
};

class StoreImm : public Inst {
public:
    enum Type { SSTORE, VSTORE };
    Type type;
    Reg* dst,* lhs;
    int offset;

    static std::map<Type, std::string> type_map;

    StoreImm() : Inst(Inst::None) {}
    StoreImm(Type _type, Reg* _dst, Reg* _lhs, int _offset) 
        : type(_type), dst(_dst), lhs(_lhs), offset(_offset),
        Inst(Inst::Store, {}, {_dst, _lhs}){
        if (_type == VSTORE)
            is_float = true;
    }
    virtual std::string gen_asm() override;
};

class StoreReg : public Inst {
public:
    enum Type { SSTORE, VSTORE };
    Type type;
    Reg *dst, *lhs, *rhs;
    Shift* shift;

    static std::map<Type, std::string> type_map;

    StoreReg() : Inst(Inst::None) {}
    StoreReg(Type _type, Reg* _dst, Reg* _lhs, Reg* _rhs) 
        : type(_type), dst(_dst), lhs(_lhs), rhs(_rhs), shift(),
        Inst(Inst::Store, {}, {_dst, _lhs, _rhs}){
        if (_type == VSTORE)
            is_float = true;
    }
    void setShift(Shift* _shift) { shift = _shift; }
    virtual std::string gen_asm() override;
};

class LoadGlobal : public Inst {
public: 
    Reg* dst;
    std::string name;

    LoadGlobal(Reg* _dst, std::string _name) : dst(_dst), name(_name),
        Inst(Inst::LoadGlobal, {_dst}, {}) {}
    virtual std::string gen_asm() override;
};

class Call : public Inst {
public:
    Function* callee;

    Call(Function* _callee);

    virtual std::string gen_asm() override;
};

class Return : public Inst {
public:
    bool is_void;

    Return(bool _is_void) : is_void(_is_void), Inst(Inst::Return) {
        def_reg = {};
        if (is_void) 
            use_reg = {};
        else
            use_reg = {new Reg(0, Reg::PHY)};
    }
    virtual std::string gen_asm() override;
};

class LoadFP : public Inst {
public:
    Reg* dst;
    std::string name;

    LoadFP(Reg* _dst, std::string _name) : dst(_dst), name(_name),
        Inst(Inst::LoadFP, {_dst}, {}) {
        is_float = true;
    }
    virtual std::string gen_asm() override;
};

class Convert : public Inst {
public:
    enum Type { STOV, VTOS };
    Type type;
    Reg* dst, *src;

    Convert(Type _type, Reg* _dst, Reg* _src) : type(_type), dst(_dst), src(_src),
        Inst(Inst::Convert, {_dst}, {_src}) {
        is_float = true;
    }
    virtual std::string gen_asm() override;
};

class Bic : public Inst {
public:
    Reg* dst,* lhs;
    int imm;

    Bic(Reg* _dst, Reg* _lhs, int _imm) 
        : dst(_dst), lhs(_lhs), imm(_imm),
        Inst(Inst::Bic, {_dst}, {_lhs}) {}
    virtual std::string gen_asm() override;
};

} // namespace mc 
} // namespace sysy


#endif