#include "sysy/Target/Reg.h"
#include "sysy/Target/Module.h"
#include "sysy/Target/Function.h"
#include "sysy/Target/Block.h"
#include "sysy/Target/Instruction.h"
#include "sysy/Support/common.h"
#include "sysy/IR/IRDump.h"
#include "sysy/Support/debug.h"
#include <cmath>
#include <map>



namespace sysy 
{
namespace mc 
{
constexpr bool FLOAT = true;


int Block::num = 0;

using Kind = ir::Instruction::InstrKind;
using CondType = Instruction::CondType;

std::map<ir::Instruction::InstrKind, Instruction::CondType> cond_map = {
    {Kind::ICmpEQ, CondType::EQ}, {Kind::ICmpNE, CondType::NE}, {Kind::ICmpSGE, CondType::GE}, {Kind::ICmpSGT, CondType::GT}, {Kind::ICmpSLE, CondType::LE}, {Kind::ICmpSLT, CondType::LT},
    {Kind::FCmpUEQ, CondType::EQ}, {Kind::FCmpUNE, CondType::NE}, {Kind::FCmpUGE, CondType::GE}, {Kind::FCmpUGT, CondType::GT}, {Kind::FCmpULE, CondType::LE}, {Kind::FCmpULT, CondType::LT}
};

static bool cmp(const std::pair<ir::Value*, int>& a, const std::pair<ir::Value*, int>& b) {
	// 把2的幂次排到后面 
    bool a_is_pow2 = !(a.second & (a.second-1));
    bool b_is_pow2 = !(b.second & (b.second-1));
    if (!a_is_pow2 && b_is_pow2)
        return true;
    else if (a_is_pow2 && b_is_pow2)
    	return a.second < b.second;
    else
        return false;
}


void Block::instSelection(ir::BasicBlock* block) {
    // B(father->block_map[block]->name);
    auto ir_insts = block->getInstructions();
    for (auto i = ir_insts.begin(); i != ir_insts.end(); ++i) {
        // if (father->block_map[block]->name == "set.copy.entry__8")
            // B((*i)->getSignature());
        if (auto binary = dynamic_cast<ir::BinaryInstrcution*>(*i)) {
            ir::Value* ir_lhs = binary->getLhs();
            ir::Value* ir_rhs = binary->getRhs();
            if (binary->getKind() == ir::Instruction::Add) {
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_rhs, this);
                    int rhs = constant->getValue();

                    if (father->analyseImm(rhs))
                        instructions.push_back(new BinaryImm(BinaryImm::ADD, dst, lhs, rhs));
                    else if (father->analyseImm(-rhs))
                        instructions.push_back(new BinaryImm(BinaryImm::SUB, dst, lhs, -rhs));
                    else {
                        Reg* rhs_reg = father->get_reg(rhs, this);
                        instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, lhs, rhs_reg));
                    }

                } else if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    int rhs = constant->getValue();
                    if (father->analyseImm(rhs))
                        instructions.push_back(new BinaryImm(BinaryImm::ADD, dst, lhs, rhs));
                    else if (father->analyseImm(-rhs))
                        instructions.push_back(new BinaryImm(BinaryImm::SUB, dst, lhs, -rhs));
                    else {
                        Reg* rhs_reg = father->get_reg(rhs, this);
                        instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, lhs, rhs_reg));
                    }
                    // instructions.push_back(new BinaryImm(BinaryImm::ADD, dst, lhs, rhs));
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, lhs, rhs));
                }
            } 
            else if (binary->getKind() == ir::Instruction::Sub) {
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    int lhs = constant->getValue();
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    if (father->analyseImm(lhs))
                        instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, rhs, lhs));
                    else if (father->analyseImm(-lhs)) {
                        Reg* tmp = new Reg(father->reg_num++);
                        instructions.push_back(new BinaryImm(BinaryImm::RSB, tmp, rhs, 0));
                        instructions.push_back(new BinaryImm(BinaryImm::SUB, dst, tmp, -lhs));
                    }
                    else {
                        Reg* lhs_reg = father->get_reg(lhs, this);
                        instructions.push_back(new BinaryReg(BinaryReg::SUB, dst, lhs_reg, rhs));
                    }
                } else if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    int rhs = constant->getValue();

                    if (father->analyseImm(rhs))
                        instructions.push_back(new BinaryImm(BinaryImm::SUB, dst, lhs, rhs));
                    else if (father->analyseImm(-rhs))
                        instructions.push_back(new BinaryImm(BinaryImm::ADD, dst, lhs, -rhs));
                    else {
                        Reg* rhs_reg = father->get_reg(rhs, this);
                        instructions.push_back(new BinaryReg(BinaryReg::SUB, dst, lhs, rhs_reg));
                    }
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::SUB, dst, lhs, rhs));
                }
            } 
            else if (binary->getKind() == ir::Instruction::Mul) {
                Reg* dst = father->get_reg(binary, this);
                Reg* lhs;
                Reg* rhs;
                int imm;
                bool exist_const = false;


                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_lhs)) {
                    lhs = father->get_reg(ir_rhs, this);
                    imm = constant->getValue();
                    exist_const = true;
                } 
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    lhs = father->get_reg(ir_lhs, this);
                    imm = constant->getValue();
                    exist_const = true;
                }
                if (exist_const) {
                    int imm_abs = imm > 0 ? imm : -imm;
                    if ( imm == -1 ) 
                        instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, dst, 0));
                    else if ( !(imm_abs & (imm_abs-1)) ) { // 如果乘以2的幂次
                        // B(imm_abs);
                        // B(imm);
                        // R(binary->getSignature());
                        int lowbit = std::log2(imm_abs);
                        ShiftInstr* shift = new ShiftInstr(dst, lhs, new Shift(Shift::LSL, lowbit));
                        instructions.push_back(shift);
                        if (imm < 0)
                            instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, dst, 0));
                    }
                    else if ( !((imm_abs+1) & imm_abs) ) { // 如果乘以2的幂次减一
                        int lowbit = std::log2(imm_abs+1);
                        Shift* shift = new Shift(Shift::LSL, lowbit);
                        if (imm > 0) {
                            BinaryReg* rsb = new BinaryReg(BinaryReg::RSB, dst, lhs, lhs);
                            rsb->setShift(shift);
                            instructions.push_back(rsb);
                        }
                        else {
                            BinaryReg* sub = new BinaryReg(BinaryReg::SUB, dst, lhs, lhs);
                            sub->setShift(shift);
                            instructions.push_back(sub);
                        }
                    }
                    else if ( !((imm_abs-1) & imm_abs-2) ) { // 如果乘以2的幂次加一
                        int lowbit = std::log2(imm_abs-1);
                        Shift* shift = new Shift(Shift::LSL, lowbit);
                        BinaryReg* add = new BinaryReg(BinaryReg::ADD, dst, lhs, lhs);
                        add->setShift(shift);
                        instructions.push_back(add);
                        if (imm < 0)
                            instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, dst, 0));
                    }
                    else {
                        rhs = father->get_reg(imm, this);
                        instructions.push_back(new BinaryReg(BinaryReg::MUL, dst, lhs, rhs));
                    }  
                }           
                else {
                    lhs = father->get_reg(ir_lhs, this);
                    rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::MUL, dst, lhs, rhs));
                }
            } 
            else if (binary->getKind() == ir::Instruction::SDiv) {

                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_lhs)) {
                    std::pair<int, ir::Value*> div_expr = {constant->getValue(), ir_rhs};
                    if (cv_div_map.count(div_expr)) {
                        father->value_map[binary] = cv_div_map[div_expr];
                        continue;
                    }

                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(constant->getValue(), this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::SDIV, dst, lhs, rhs));
                    cv_div_map.insert({div_expr, dst});
                } 
                else if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    std::pair<ir::Value*, int> div_expr = {ir_lhs, constant->getValue()};
                    if (vc_div_map.count(div_expr)) {
                        father->value_map[binary] = vc_div_map[div_expr];
                        continue;
                    }                    

                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    int imm = constant->getValue();

                    int imm_abs = imm > 0 ? imm : -imm;
                    if ( imm == -1 ) 
                        instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, lhs, 0));
                    else if ( !(imm_abs & (imm_abs-1)) ) { // 如果除以2的幂次
                        int lowbit = std::log2(imm_abs);
                        Reg* sign_reg = new Reg(father->reg_num++);
                        Reg* new_div = new Reg(father->reg_num++);
                        ShiftInstr* get_sign = new ShiftInstr(sign_reg, lhs, new Shift(Shift::ASR, 31));
                        BinaryReg* add_offset = new BinaryReg(BinaryReg::ADD, new_div, lhs, sign_reg);
                        add_offset->setShift(new Shift(Shift::LSR, 32-lowbit));

                        instructions.push_back(get_sign);
                        instructions.push_back(add_offset);

                        ShiftInstr* shift = new ShiftInstr(dst, new_div, new Shift(Shift::ASR, lowbit));                        
                        instructions.push_back(shift);
                        if (imm < 0)
                            instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, dst, 0));
                    }
                    else {
                        Reg* rhs = father->get_reg(imm, this);
                        instructions.push_back(new BinaryReg(BinaryReg::SDIV, dst, lhs, rhs));
                    }
                    vc_div_map.insert({div_expr, dst});
                } 
                else {
                    std::pair<ir::Value*, ir::Value*> div_expr = {ir_lhs, ir_rhs};
                    if (vv_div_map.count(div_expr)) {
                        father->value_map[binary] = vv_div_map[div_expr];
                        continue;
                    }            

                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::SDIV, dst, lhs, rhs));
                    vv_div_map.insert({div_expr, dst});
                }
            } 
            else if (binary->getKind() == ir::Instruction::SRem) {
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(constant->getValue(), this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    Reg* div_tmp = new Reg(father->reg_num++);
                    std::pair<int, ir::Value*> div_expr = {constant->getValue(), ir_rhs};
                    if (cv_div_map.count(div_expr))
                        div_tmp = cv_div_map[div_expr];
                    else {
                        instructions.push_back(new BinaryReg(BinaryReg::SDIV, div_tmp, lhs, rhs));
                        cv_div_map.insert({div_expr, div_tmp});
                    }
                    instructions.push_back(new ComplexMul(ComplexMul::MLS, dst, div_tmp, rhs, lhs));
                } else if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    int imm = constant->getValue();

                    int imm_abs = imm > 0 ? imm : -imm;
                    if ( imm == 1 ) 
                        father->value_map[binary] = lhs;
                    else if ( imm == -1 ) 
                        instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, lhs, 0));
                    else if ( !(imm_abs & (imm_abs-1)) ) { // 如果mod 2的幂次
                        int lowbit = std::log2(imm_abs);
                        Reg* new_mod = new Reg(father->reg_num++);
                        if (lowbit == 1) {
                            BinaryReg* add_offset = new BinaryReg(BinaryReg::ADD, new_mod, lhs, lhs);
                            add_offset->setShift(new Shift(Shift::LSR, 32-lowbit));
                            instructions.push_back(add_offset);
                        }
                        else {
                            Reg* sign_reg = new Reg(father->reg_num++);
                            ShiftInstr* get_sign = new ShiftInstr(sign_reg, lhs, new Shift(Shift::ASR, 31));
                            BinaryReg* add_offset = new BinaryReg(BinaryReg::ADD, new_mod, lhs, sign_reg);
                            add_offset->setShift(new Shift(Shift::LSR, 32-lowbit));
                            instructions.push_back(get_sign);                        
                            instructions.push_back(add_offset);                        
                        }                        

                        Reg* tmp = new Reg(father->reg_num++);
                        if (imm_abs <= 256)
                            instructions.push_back(new Bic(tmp, new_mod, imm_abs-1));
                        else {
                            ShiftInstr* shift_left = new ShiftInstr(tmp, new_mod, new Shift(Shift::LSR, lowbit));
                            ShiftInstr* shift_right = new ShiftInstr(tmp, tmp, new Shift(Shift::LSL, lowbit));
                            instructions.push_back(shift_left);
                            instructions.push_back(shift_right);
                        }
                        instructions.push_back(new BinaryReg(BinaryReg::SUB, dst, lhs, tmp));

                        // if (imm < 0)
                            // instructions.push_back(new BinaryImm(BinaryImm::RSB, dst, dst, 0));
                    }
                    else {
                        R("here");
                        R((*i)->getSignature());
                        // R(imm);

                        Reg* rhs = father->get_reg(imm, this);

                        Reg* div_tmp = new Reg(father->reg_num++);
                        std::pair<ir::Value*, int> div_expr = {ir_lhs, constant->getValue()};
                        if (vc_div_map.count(div_expr)) {
                            // G("here");
                            div_tmp = vc_div_map[div_expr];
                        }
                        else {
                            instructions.push_back(new BinaryReg(BinaryReg::SDIV, div_tmp, lhs, rhs));
                            vc_div_map.insert({div_expr, div_tmp});
                        }
                        instructions.push_back(new ComplexMul(ComplexMul::MLS, dst, div_tmp, rhs, lhs));
                    }                    
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    Reg* div_tmp = new Reg(father->reg_num++);
                    std::pair<ir::Value*, ir::Value*> div_expr = {ir_lhs, ir_rhs};
                    if (vv_div_map.count(div_expr))
                        div_tmp = vv_div_map[div_expr];
                    else {
                        instructions.push_back(new BinaryReg(BinaryReg::SDIV, div_tmp, lhs, rhs));
                        vv_div_map.insert({div_expr, div_tmp});
                    }
                    instructions.push_back(new ComplexMul(ComplexMul::MLS, dst, div_tmp, rhs, lhs));
                }
            } 
            else if (binary->getKind() == ir::Instruction::FAdd) {
                if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_rhs, this);
                    Reg* rhs = father->get_reg(constant->getValue(), this);

                    instructions.push_back(new BinaryReg(BinaryReg::VADD, dst, lhs, rhs));    
                } else if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(constant->getValue(), this);

                    instructions.push_back(new BinaryReg(BinaryReg::VADD, dst, lhs, rhs));
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::VADD, dst, lhs, rhs));
                }                
            } 
            else if (binary->getKind() == ir::Instruction::FSub) {
                if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(constant->getValue(), this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    instructions.push_back(new BinaryReg(BinaryReg::VSUB, dst, lhs, rhs));    
                } else if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(constant->getValue(), this);

                    instructions.push_back(new BinaryReg(BinaryReg::VSUB, dst, lhs, rhs));
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new BinaryReg(BinaryReg::VSUB, dst, lhs, rhs));
                }                
            } 
            else if (binary->getKind() == ir::Instruction::FMul) {
                if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(constant->getValue(), this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    instructions.push_back(new BinaryReg(BinaryReg::VMUL, dst, lhs, rhs));    
                } else if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(constant->getValue(), this);

                    instructions.push_back(new BinaryReg(BinaryReg::VMUL, dst, lhs, rhs));
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    instructions.push_back(new BinaryReg(BinaryReg::VMUL, dst, lhs, rhs));
                }                   
            } 
            else if (binary->getKind() == ir::Instruction::FDiv) {
                if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_lhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(constant->getValue(), this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    instructions.push_back(new BinaryReg(BinaryReg::VDIV, dst, lhs, rhs));    
                } else if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(constant->getValue(), this);

                    instructions.push_back(new BinaryReg(BinaryReg::VDIV, dst, lhs, rhs));
                } else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    instructions.push_back(new BinaryReg(BinaryReg::VDIV, dst, lhs, rhs));
                }       
            } 
            else if (binary->getKind() == ir::Instruction::ICmpEQ ||
                binary->getKind() == ir::Instruction::ICmpNE ||
                binary->getKind() == ir::Instruction::ICmpSLT ||
                binary->getKind() == ir::Instruction::ICmpSGT ||
                binary->getKind() == ir::Instruction::ICmpSLE ||
                binary->getKind() == ir::Instruction::ICmpSGE) {
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_lhs)) {
                    if (ir::ConstantInt* constant2 = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                        Reg* dst = father->get_reg(binary, this);
                        Reg* lhs = father->get_reg(constant->getValue(), this);
                        Reg* rhs = father->get_reg(constant2->getValue(), this);
                        instructions.push_back(new MoveImm(dst, 0));
                        instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs));
                        MoveImm* mov = new MoveImm(dst, 1);
                        if (cond_map[binary->getKind()] != Instruction::AL) {
                            mov->setCond(cond_map[binary->getKind()]);
                            mov->use_reg.push_back(dst);
                        }                    
                        // mov->setCond(cond_map[binary->getKind()]);
                        instructions.push_back(mov);    
                        continue;
                    }

                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_rhs, this);
                    int rhs = constant->getValue();

                    instructions.push_back(new MoveImm(dst, 0));
                    if (father->analyseImm(rhs)) 
                        instructions.push_back(new CompareImm(CompareImm::CMP, lhs, rhs));
                    else if (father->analyseImm(-rhs)) 
                        instructions.push_back(new CompareImm(CompareImm::CMN, lhs, -rhs));
                    else {
                        Reg* rhs_reg = father->get_reg(rhs, this);
                        instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs_reg));
                    }
                    MoveImm* mov = new MoveImm(dst, 1);
                    if (cond_map[binary->getKind()] != Instruction::AL) {
                        mov->setCond(cond_map[binary->getKind()]);
                        mov->use_reg.push_back(dst);
                    }
                    instructions.push_back(mov);
                    // assert(0);
                } 
                else if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    int rhs = constant->getValue();

                    instructions.push_back(new MoveImm(dst, 0));
                    if (father->analyseImm(rhs)) 
                        instructions.push_back(new CompareImm(CompareImm::CMP, lhs, rhs));
                    else if (father->analyseImm(-rhs)) 
                        instructions.push_back(new CompareImm(CompareImm::CMN, lhs, -rhs));
                    else {
                        Reg* rhs_reg = father->get_reg(rhs, this);
                        instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs_reg));
                    }
                    MoveImm* mov = new MoveImm(dst, 1);
                    if (cond_map[binary->getKind()] != Instruction::AL) {
                        mov->setCond(cond_map[binary->getKind()]);
                        mov->use_reg.push_back(dst);
                    }                    
                    // mov->setCond(cond_map[binary->getKind()]);
                    instructions.push_back(mov);
                    // Reg* dst = father->get_reg(binary, this);
                    // Reg* lhs = father->get_reg(ir_lhs, this);
                    // Reg* rhs = father->get_reg(constant->getValue(), this);
                    // instructions.push_back(new MoveImm(MoveImm::MOV, dst, 0));
                    // instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs));
                    // MoveImm* mov = new MoveImm(MoveImm::MOV, dst, 1);
                    // mov->setCond(cond_map[binary->getKind()]);
                    // instructions.push_back(mov);
                } 
                else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    instructions.push_back(new MoveImm(dst, 0));
                    instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs));
                    MoveImm* mov = new MoveImm(dst, 1);
                    if (cond_map[binary->getKind()] != Instruction::AL) {
                        mov->setCond(cond_map[binary->getKind()]);
                        mov->use_reg.push_back(dst);
                    }                    
                    // mov->setCond(cond_map[binary->getKind()]);
                    instructions.push_back(mov);              
                }                
            }
            else if (binary->getKind() == ir::Instruction::FCmpUEQ ||
                binary->getKind() == ir::Instruction::FCmpUNE ||
                binary->getKind() == ir::Instruction::FCmpULT ||
                binary->getKind() == ir::Instruction::FCmpUGT ||
                binary->getKind() == ir::Instruction::FCmpULE ||
                binary->getKind() == ir::Instruction::FCmpUGE) {
                if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_lhs)) {
                    if (ir::ConstantFP* constant2 = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                        Reg* dst = father->get_reg(binary, this);
                        Reg* lhs = father->get_reg(constant->getValue(), this);
                        Reg* rhs = father->get_reg(constant2->getValue(), this);

                        instructions.push_back(new MoveImm(dst, 0));
                        instructions.push_back(new CompareReg(CompareReg::VCMP, lhs, rhs));

                        MoveImm* mov = new MoveImm(dst, 1);
                        if (cond_map[binary->getKind()] != Instruction::AL) {
                            mov->setCond(cond_map[binary->getKind()]);
                            mov->use_reg.push_back(dst);
                        }
                        instructions.push_back(mov);

                        continue;
                    }
                    // R("here00");
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(constant->getValue(), this);
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    // R("here01");
                    instructions.push_back(new MoveImm(dst, 0));
                    instructions.push_back(new CompareReg(CompareReg::VCMP, lhs, rhs));

                    MoveImm* mov = new MoveImm(dst, 1);
                    if (cond_map[binary->getKind()] != Instruction::AL) {
                        mov->setCond(cond_map[binary->getKind()]);
                        mov->use_reg.push_back(dst);
                    }
                    instructions.push_back(mov);
                }           
                else if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(constant->getValue(), this);

                    instructions.push_back(new MoveImm(dst, 0));
                    instructions.push_back(new CompareReg(CompareReg::VCMP, lhs, rhs));

                    MoveImm* mov = new MoveImm(dst, 1);
                    if (cond_map[binary->getKind()] != Instruction::AL) {
                        mov->setCond(cond_map[binary->getKind()]);
                        mov->use_reg.push_back(dst);
                    }                    
                    instructions.push_back(mov);
                }       
                else {
                    Reg* dst = father->get_reg(binary, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    Reg* rhs = father->get_reg(ir_rhs, this);

                    instructions.push_back(new MoveImm(dst, 0));
                    instructions.push_back(new CompareReg(CompareReg::VCMP, lhs, rhs));

                    MoveImm* mov = new MoveImm(dst, 1);
                    if (cond_map[binary->getKind()] != Instruction::AL) {
                        mov->setCond(cond_map[binary->getKind()]);
                        mov->use_reg.push_back(dst);
                    }
                    instructions.push_back(mov);
                }               
            }
            else {
                assert(0);
            }
        } 
        else if (auto unary = dynamic_cast<ir::UnaryInstruction*>(*i)) {
            ir::Value* ir_src = unary->getValue();
            if (unary->getKind() == ir::UnaryInstruction::SIToFP) {
                Reg* dst = father->get_reg(unary, this);
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_src)) {
                    Reg* src = father->get_reg(constant->getValue(), this);
                    Reg* tmp = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                    instructions.push_back(new MoveReg(MoveReg::SVMOV, tmp, src));
                    instructions.push_back(new Convert(Convert::STOV, dst, tmp));
                }
                else {
                    Reg* src = father->get_reg(ir_src, this);
                    Reg* tmp = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                    instructions.push_back(new MoveReg(MoveReg::SVMOV, tmp, src));
                    instructions.push_back(new Convert(Convert::STOV, dst, tmp));
                }
                father->value_map[unary] = dst;
            }
            else if (unary->getKind() == ir::UnaryInstruction::FPToSI) {
                Reg* dst = father->get_reg(unary, this);
                if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(ir_src)) {
                    Reg* src = father->get_reg(constant->getValue(), this);
                    Reg* tmp = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                    instructions.push_back(new Convert(Convert::VTOS, tmp, src));
                    instructions.push_back(new MoveReg(MoveReg::SVMOV, dst, tmp));
                }
                else {
                    Reg* src = father->get_reg(ir_src, this);
                    Reg* tmp = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                    instructions.push_back(new Convert(Convert::VTOS, tmp, src));
                    instructions.push_back(new MoveReg(MoveReg::SVMOV, dst, tmp));
                }
                father->value_map[unary] = dst;
            }
            else {
                assert(0);
            }
        } 
        else if (auto call = dynamic_cast<ir::CallInstruction*>(*i)) {
            if (call->getCallee()->getName() == "putfloat") {
                for (auto arg : call->getArgs()) {
                    if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(arg)) {
                        Reg* src = father->get_reg(constant->getValue(), this);
                        Reg* dst = new Reg(0, Reg::PHY, Reg::VREG);
                        instructions.push_back(new MoveReg(MoveReg::VVMOV, dst, src));
                        instructions.push_back(new Call(father->father->func_map[call->getCallee()]));
                    }
                    else {
                        Reg* src = father->get_reg(arg, this);
                        Reg* dst = new Reg(0, Reg::PHY, Reg::VREG);
                        instructions.push_back(new MoveReg(MoveReg::VVMOV, dst, src));
                        instructions.push_back(new Call(father->father->func_map[call->getCallee()]));
                    }
                }
                continue;
            }
            else if (call->getCallee()->getName() == "getfloat") {
                instructions.push_back(new Call(father->father->func_map[call->getCallee()]));
                Reg* new_reg = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                instructions.push_back(new MoveReg(MoveReg::VVMOV, new_reg, new Reg(0, Reg::PHY, Reg::VREG)));
                father->value_map[call] = new_reg;
                
                continue;
            }    
            else if (call->getCallee()->getName() == "starttime") {
                auto line = Line::getInstance();
                int lineno = line.start;
                instructions.push_back(new MoveImm(new Reg(0, Reg::PHY), lineno));
                instructions.push_back(new Call(father->father->func_map[call->getCallee()]));

                continue;
            }
            else if (call->getCallee()->getName() == "stoptime") {
                auto line = Line::getInstance();
                int lineno = line.end;
                instructions.push_back(new MoveImm(new Reg(0, Reg::PHY), lineno));
                instructions.push_back(new Call(father->father->func_map[call->getCallee()]));

                continue;
            }

            if (call->getCallee()->getName() == "__thread_create") 
                father->father->create_threads = true;


            auto type = dynamic_cast<ir::FunctionType*>(call->getCallee()->getType());
            if (!type->getRet()->isVoidTy()) {
                // B("I'm here\n");
                auto it = father->rev_arg_idx.find(0);
                if (it != father->rev_arg_idx.end()) {
                    auto arg0 = father->value_map.find(it->second);
                    if (arg0 == father->value_map.end()) {}
                    else if (arg0 != father->value_map.end() && arg0->second->type == Reg::VIRTUAL) {}
                    else {
                        // B("I'm here2\n");
                        if (arg0->first->getType()->isFloatTy()) {
                            // B("III");
                            Reg* new_reg = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                            father->value_map[arg0->first] = new_reg;
                            instructions.push_back(new MoveReg(MoveReg::SVMOV, new_reg, new Reg(0, Reg::PHY)));
                        }
                        else {
                            // B("JJJ");
                            Reg* new_reg = new Reg(father->reg_num++, Reg::VIRTUAL);
                            father->value_map[arg0->first] = new_reg;
                            instructions.push_back(new MoveReg(MoveReg::MOV, new_reg, new Reg(0, Reg::PHY)));
                        }
                    }
                }
            }

            // B("between call\n");
            // for (auto i : instructions)
            //     B(i->gen_asm());
            // std::cout << "\n";

            int idx = 0;
            std::vector<Instruction*> push_list;

            for (auto arg : call->getArgs()) {
                auto it = father->rev_arg_idx.find(idx);
                if (it != father->rev_arg_idx.end()) {
                    // if (it->second == arg) {}
                    // else {
                        auto arg_idx = father->value_map.find(it->second);
                        if (arg_idx == father->value_map.end()) {}
                        else if (arg_idx != father->value_map.end() && arg_idx->second->type == Reg::VIRTUAL) {}
                        else if (arg_idx->first->getType()->isFloatTy()) {
                            // B("hereeeee");
                            assert(0);
                            Reg* new_reg = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                            father->value_map[arg_idx->first] = new_reg;
                            instructions.push_back(new MoveReg(MoveReg::SVMOV, new_reg, new Reg(idx, Reg::PHY)));
                        }
                        else {
                            // B("hhhhhhhhh");
                            Reg* new_reg = new Reg(father->reg_num++, Reg::VIRTUAL);
                            father->value_map[arg_idx->first] = new_reg;
                            instructions.push_back(new MoveReg(MoveReg::MOV, new_reg, new Reg(idx, Reg::PHY)));
                        }
                    // }
                }
            

                if (idx <= 3)
                    father->rev_arg_idx[idx] = arg;
                
                if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(arg)) {
                    if (idx <= 3)  {
                        Reg* dst = new Reg(idx, Reg::PHY);
                        instructions.push_back(new MoveImm(dst, constant->getValue()));
                    }
                    else {
                        Reg* dst = father->get_reg(constant->getValue(), this);
                        push_list.push_back(new Push(Push::SPUSH, {dst}));
                    }
                }
                else if (ir::ConstantFP* constant = dynamic_cast<ir::ConstantFP*>(arg)) {
                    if (idx <= 3)  {
                        Reg* src = father->get_reg(constant->getValue(), this);
                        Reg* dst = new Reg(idx, Reg::PHY);
                        instructions.push_back(new MoveReg(MoveReg::SVMOV, dst, src));
                    }
                    else {
                        Reg* dst = father->get_reg(constant->getValue(), this);
                        push_list.push_back(new Push(Push::VPUSH, {dst}));
                    }                                        
                }
                else if (auto gep = dynamic_cast<ir::GEPInst*>(arg)) {
                    Reg* gep_reg = new Reg(father->reg_num++);
                    if (father->gep_map.count(gep)) {
                        Reg* sp = new Reg(13, Reg::PHY);
                        // may wrong
                        auto add_inst = new BinaryImm(BinaryImm::ADD, gep_reg, sp, 0);
                        father->gep_list[add_inst] = father->gep_map[gep];
                        instructions.push_back(add_inst);
                    }                
                    else if (father->gep_with_constant.count(gep)) {
                        Reg* lhs = father->get_reg(gep, this);
                        int offset = father->gep_with_constant[gep];

                        if (Function::analyseImm(offset))
                            instructions.push_back(new BinaryImm(BinaryImm::ADD, gep_reg, lhs, offset));
                        else if (Function::analyseImm(-offset))
                            instructions.push_back(new BinaryImm(BinaryImm::SUB, gep_reg, lhs, -offset));
                        else {
                            Reg* offset_reg = father->get_reg(offset, this);
                            instructions.push_back(new BinaryReg(BinaryReg::ADD, gep_reg, lhs, offset_reg));
                        }
                    }
                    else if (father->gep_with_shift.count(gep)) {
                        Reg* lhs = father->get_reg(gep, this);
                        Reg* rhs = father->gep_with_shift[gep].first;
                        int bits = father->gep_with_shift[gep].second;
                        
                        auto add_inst = new BinaryReg(BinaryReg::ADD, gep_reg, lhs, rhs);
                        add_inst->setShift(new Shift(Shift::LSL, bits));
                        instructions.push_back(add_inst);
                    }
                    else if (father->gep_with_reg.count(gep)) {
                        Reg* lhs = father->get_reg(gep, this);
                        Reg* rhs = father->gep_with_reg[gep];

                        instructions.push_back(new BinaryReg(BinaryReg::ADD, gep_reg, lhs, rhs));
                    }
                    else {
                        // B("here");
                        gep_reg = father->get_reg(gep, this);
                    }

                    if (idx <= 3)  {
                        Reg* dst = new Reg(idx, Reg::PHY);
                        instructions.push_back(new MoveReg(MoveReg::MOV, dst, gep_reg));
                    }
                    else {
                        push_list.push_back(new Push(Push::SPUSH, {gep_reg}));
                    }
                }                
                else {
                    if (arg->getType()->isFloatTy()) {
                        if (idx <= 3)  {
                            Reg* dst = new Reg(idx, Reg::PHY);
                            Reg* src = father->get_reg(arg, this);
                            instructions.push_back(new MoveReg(MoveReg::SVMOV, dst, src));
                        }
                        else {
                            Reg* dst = father->get_reg(arg, this);
                            push_list.push_back(new Push(Push::VPUSH, {dst}));
                            // father->stack_size += 4;
                        }
                    }
                    else {
                        if (idx <= 3)  {
                            Reg* dst = new Reg(idx, Reg::PHY);
                            Reg* lhs = father->get_reg(arg, this);
                            instructions.push_back(new MoveReg(MoveReg::MOV, dst, lhs));
                        }
                        else {
                            Reg* dst = father->get_reg(arg, this);
                            push_list.push_back(new Push(Push::SPUSH, {dst}));
                            // father->stack_size += 4;
                        }
                    }
                }

                idx++;
            }


            Reg* sp = new Reg(13, Reg::PHY);
            int offset = idx > 4 ? (idx-4) * 4 : 0; 
            if ( offset & 4) {
                offset += 4;
                push_list.push_back(new BinaryImm(BinaryImm::SUB, sp, sp, 4));
            }

            // B("here");
            if (push_list.size())
                instructions.insert(instructions.end(), push_list.rbegin(), push_list.rend());

            instructions.push_back(new Call(father->father->func_map[call->getCallee()]));



            //TODO: 改为多条add
            if (offset) {
                if (father->analyseImm(offset))
                    instructions.push_back(new BinaryImm(BinaryImm::ADD, sp, sp, offset));
                else {
                    Reg* offset_reg = father->get_reg(offset, this);
                    instructions.push_back(new BinaryReg(BinaryReg::ADD, sp, sp, offset_reg));
                }
            }
            
            if (!type->getRet()->isVoidTy()) {
                father->rev_arg_idx[0] = call;
                if (type->getRet()->isFloatTy()) {
                    Reg* new_reg = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                    instructions.push_back(new MoveReg(MoveReg::SVMOV, new_reg, new Reg(0, Reg::PHY)));
                    father->value_map[call] = new_reg;
                }
                else {
                    Reg* new_reg = new Reg(father->reg_num++, Reg::VIRTUAL);
                    instructions.push_back(new MoveReg(MoveReg::MOV, new_reg, new Reg(0, Reg::PHY)));
                    father->value_map[call] = new_reg;
                }
            }

            // std::cout << "\n";
            // B("after call\n");
            // for (auto i : instructions)
            //     B(i->gen_asm());
            // std::cout << "\n";
        } 
        else if (auto br = dynamic_cast<ir::BrInstruction*>(*i)) {
            ir::Value* ir_cond = br->getCond();
            if (ir_cond == nullptr) {
                // B("here000");
                Block* target = father->block_map[br->getTrueBlock()];
                instructions.push_back(new Branch(target));
            } 
            else if (ir::ConstantInt* constant = dynamic_cast<ir::ConstantInt*>(ir_cond)) {
                // B("0011");
                if (constant->getValue() == 0) {
                    // B("here111");
                    Block* target = father->block_map[br->getFalseBlock()];
                    instructions.push_back(new Branch(target));
                } 
                else {
                    Block* target = father->block_map[br->getTrueBlock()];
                    instructions.push_back(new Branch(target));
                }
            } 
            else if (ir::BinaryInstrcution* cond = dynamic_cast<ir::BinaryInstrcution*>(ir_cond)) {
                Block* true_target = father->block_map[br->getTrueBlock()];
                Block* false_target = father->block_map[br->getFalseBlock()];
                Reg* lhs = father->get_reg(cond->getLhs(), this);
                if (auto constant = dynamic_cast<ir::ConstantInt*>(cond->getRhs())) {
                    int rhs = constant->getValue();
                    if (father->analyseImm(rhs)) 
                        instructions.push_back(new CompareImm(CompareImm::CMP, lhs, rhs));
                    else if (father->analyseImm(-rhs)) 
                        instructions.push_back(new CompareImm(CompareImm::CMN, lhs, -rhs));
                    else {
                        Reg* rhs_reg = father->get_reg(rhs, this);
                        instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs_reg));
                    }
                    // instructions.push_back(new CompareImm(CompareImm::CMP, lhs, rhs));
                }
                else if (auto constant = dynamic_cast<ir::ConstantFP*>(cond->getRhs())) {
                    Reg* rhs = father->get_reg(constant->getValue(), this);
                    instructions.push_back(new CompareReg(CompareReg::VCMP, lhs, rhs));
                }
                else {
                    if (cond->getRhs()->getType()->isFloatTy()) {
                        Reg* rhs = father->get_reg(cond->getRhs(), this);
                        instructions.push_back(new CompareReg(CompareReg::VCMP, lhs, rhs));                    
                    }
                    else {
                        Reg* rhs = father->get_reg(cond->getRhs(), this);
                        instructions.push_back(new CompareReg(CompareReg::CMP, lhs, rhs));
                    }
                }
                Branch* true_br = new Branch(true_target);
                true_br->setCond(cond_map[cond->getKind()]);
                instructions.push_back(true_br);
                instructions.push_back(new Branch(false_target));
            } 
            else {
                assert(0);
            }
        } 
        else if (auto ret = dynamic_cast<ir::RetInstruction*>(*i)) {
            // B("here ret");
            // for (auto i : instructions)
            //     B(i->gen_asm());

            father->return_blocks.insert(this);
            if(ret->isVoidRet()) {
                instructions.push_back(new Return(true));
                break;
            }
            // std::cout << "return instruction\n";
            ir::Value* return_val = ret->getRet();
            if (ir::ConstantInt* ir_src = dynamic_cast<ir::ConstantInt*>(return_val)) {
                int src = ir_src->getValue();
                Reg* dst = new Reg(0, Reg::PHY);

                instructions.push_back(new MoveImm(dst, src));
            }
            else if (ir::ConstantFP* ir_src = dynamic_cast<ir::ConstantFP*>(return_val)) {
                Reg* src = father->get_reg(ir_src->getValue(), this);
                Reg* dst = new Reg(0, Reg::PHY);
                instructions.push_back(new MoveReg(MoveReg::SVMOV, dst, src));
            } 
            else {
                // std::cout << "return value is reg\n";
                if (return_val->getType()->isFloatTy()) {
                    Reg* src = father->get_reg(return_val, this);
                    Reg* dst = new Reg(0, Reg::PHY);
                    instructions.push_back(new MoveReg(MoveReg::SVMOV, dst, src));
                }
                else {
                    Reg* src = father->get_reg(return_val, this);
                    Reg* dst = new Reg(0, Reg::PHY);
                    instructions.push_back(new MoveReg(MoveReg::MOV, dst, src));
                }
            }

            instructions.push_back(new Return(false));
            break;
        } 
        else if (auto store = dynamic_cast<ir::StoreInstruction*>(*i)) {
            ir::Value* ir_src = store->getValue();
            ir::Value* ir_dst = store->getPtr();

            if (auto gep = dynamic_cast<ir::GEPInst*>(ir_dst)) {
                // B("here");
                if (father->gep_map.count(gep)) {
                    // R("here");
                    Reg* src = father->get_reg(ir_src, this);
                    Reg* sp = new Reg(13, Reg::PHY);
                    auto store_inst = new StoreImm();
                    if (ir_src->getType()->isFloatTy()) 
                        store_inst = new StoreImm(StoreImm::VSTORE, sp, src, father->gep_map[gep]);
                    else 
                        store_inst = new StoreImm(StoreImm::SSTORE, sp, src, father->gep_map[gep]);
                    father->gep_list[store_inst] = father->gep_map[gep];
                    instructions.push_back(store_inst);                    
                }
                else if (father->gep_with_constant.count(gep)) {
                    Reg* dst = father->get_reg(gep, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    int offset = father->gep_with_constant[gep]; 

                    if (offset < 4096) {
                        auto store_inst = new StoreImm();
                        if (ir_src->getType()->isFloatTy())
                            store_inst = new StoreImm(StoreImm::VSTORE, dst, lhs, offset);
                        else
                            store_inst = new StoreImm(StoreImm::SSTORE, dst, lhs, offset);
                        instructions.push_back(store_inst);
                    }
                    else {
                        Reg* offset_reg = father->get_reg(offset, this);
                        if (ir_src->getType()->isFloatTy()) {
                            Reg* tmp = new Reg(father->reg_num++);
                            instructions.push_back(new BinaryReg(BinaryReg::ADD, tmp, dst, offset_reg));
                            instructions.push_back(new StoreImm(StoreImm::VSTORE, tmp, lhs, 0));
                            father->gep_with_constant.erase(gep);
                            father->value_map[gep] = tmp;
                        }
                        else {   
                            instructions.push_back(new StoreReg(StoreReg::SSTORE, dst, lhs, offset_reg));
                        }
                    }
                }
                else if (father->gep_with_shift.count(gep)) {
                    Reg* dst = father->get_reg(gep, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    Reg* rhs = father->gep_with_shift[gep].first;
                    int bits = father->gep_with_shift[gep].second;
                    if (ir_src->getType()->isFloatTy()) {
                        Reg* tmp = new Reg(father->reg_num++);
                        auto add_inst = new BinaryReg(BinaryReg::ADD, tmp, dst, rhs);
                        add_inst->setShift(new Shift(Shift::LSL, bits));
                        instructions.push_back(add_inst);
                        instructions.push_back(new StoreImm(StoreImm::VSTORE, tmp, lhs, 0));
                        father->gep_with_shift.erase(gep);
                        father->value_map[gep] = tmp;
                    }                        
                    else {
                        auto store_inst = new StoreReg(StoreReg::SSTORE, dst, lhs, rhs);
                        store_inst->setShift(new Shift(Shift::LSL, bits));
                        instructions.push_back(store_inst);
                    }
                }
                else if (father->gep_with_reg.count(gep)) {
                    Reg* dst = father->get_reg(gep, this);
                    Reg* lhs = father->get_reg(ir_src, this);                    
                    Reg* rhs = father->gep_with_reg[gep];

                    if (ir_src->getType()->isFloatTy()) {
                        Reg* tmp = new Reg(father->reg_num++);
                        instructions.push_back(new BinaryReg(BinaryReg::ADD, tmp, dst, rhs));
                        instructions.push_back(new StoreImm(StoreImm::VSTORE, tmp, lhs, 0));
                        father->gep_with_reg.erase(gep);
                        father->value_map[gep] = tmp;
                    }
                    else {
                        instructions.push_back(new StoreReg(StoreReg::SSTORE, dst, lhs, rhs));
                    }
                }
                else {
                    // B("here");
                    Reg* dst = father->get_reg(gep, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    auto store_inst = new StoreImm();
                    if (ir_src->getType()->isFloatTy()) 
                        store_inst = new StoreImm(StoreImm::VSTORE, dst, lhs, 0);
                    else
                        store_inst = new StoreImm(StoreImm::SSTORE, dst, lhs, 0);                    
                    instructions.push_back(store_inst);
                    // assert(0);
                }
                continue;
            }


            if (auto alloc = dynamic_cast<ir::AllocaInst*>(ir_dst)) {
                if (alloc->getAllocatedType()->isPointerTy()) {
                    // src可能存在寄存器里，也可能存在栈里，需注意
                    Reg* src = father->get_reg(ir_src, this);
                    father->store_alloc_map[alloc] = src;
                    continue;
                }
            }

            if (auto constant = dynamic_cast<ir::ConstantInt*>(ir_src)) {
                Reg* src = father->get_reg(constant->getValue(), this);
                Reg* dst = father->get_reg(ir_dst, this);
                // std::cout << "store constant\n";
                // std::cout << src->id << " " << dst->id << "\n";
                instructions.push_back(new StoreImm(StoreImm::SSTORE, dst, src, 0));
            } 
            else if (auto constant = dynamic_cast<ir::ConstantFP*>(ir_src)) {
                Reg* src = father->get_reg(constant->getValue(), this);
                Reg* dst = father->get_reg(ir_dst, this);
                instructions.push_back(new StoreImm(StoreImm::VSTORE, dst, src, 0));
            }
            else {
                if (ir_src->getType()->isFloatTy()) {
                    Reg* src = father->get_reg(ir_src, this);
                    Reg* dst = father->get_reg(ir_dst, this);
                    instructions.push_back(new StoreImm(StoreImm::VSTORE, dst, src, 0));
                }
                else {
                    Reg* src = father->get_reg(ir_src, this);
                    Reg* dst = father->get_reg(ir_dst, this);
                    instructions.push_back(new StoreImm(StoreImm::SSTORE, dst, src, 0));
                }
            }
        } 
        else if (auto load = dynamic_cast<ir::LoadInstruction*>(*i)) {
            ir::Value* ir_src = load->getPtr();

            if (auto gep = dynamic_cast<ir::GEPInst*>(ir_src)) {

                if (father->gep_map.count(gep)) {
                    // R("here");
                    Reg* sp = new Reg(13, Reg::PHY);
                    Reg* dst = father->get_reg(load, this);
                    auto load_inst = new LoadImm();
                    if (load->getType()->isFloatTy()) 
                        load_inst = new LoadImm(LoadImm::VLOAD, dst, sp, father->gep_map[gep]);
                    else 
                        load_inst = new LoadImm(LoadImm::SLOAD, dst, sp, father->gep_map[gep]);
                    father->gep_list[load_inst] = father->gep_map[gep];
                    instructions.push_back(load_inst);                    
                }                
                else if (father->gep_with_constant.count(gep)) {
                    Reg* dst = father->get_reg(load, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    int offset = father->gep_with_constant[gep];

                    if (offset < 4096) {
                        auto load_inst = new LoadImm();
                        if (load->getType()->isFloatTy()) 
                            load_inst = new LoadImm(LoadImm::VLOAD, dst, lhs, offset);
                        else
                            load_inst = new LoadImm(LoadImm::SLOAD, dst, lhs, offset);
                        instructions.push_back(load_inst);
                    }
                    else {
                        Reg* offset_reg = father->get_reg(offset, this);
                        if (load->getType()->isFloatTy()) {
                            Reg* tmp = new Reg(father->reg_num++);
                            instructions.push_back(new BinaryReg(BinaryReg::ADD, tmp, lhs, offset_reg));
                            instructions.push_back(new LoadImm(LoadImm::VLOAD, dst, tmp, 0));
                            father->gep_with_constant.erase(gep);
                            father->value_map[gep] = tmp;                            
                        }
                        else {   
                            instructions.push_back(new LoadReg(LoadReg::SLOAD, dst, lhs, offset_reg));
                        }
                    }
                }
                else if (father->gep_with_shift.count(gep)) {
                    Reg* dst = father->get_reg(load, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    Reg* rhs = father->gep_with_shift[gep].first;
                    int bits = father->gep_with_shift[gep].second;
                    
                    if (load->getType()->isFloatTy()) {
                        Reg* tmp = new Reg(father->reg_num++);
                        auto add_inst = new BinaryReg(BinaryReg::ADD, tmp, lhs, rhs);
                        add_inst->setShift(new Shift(Shift::LSL, bits));
                        instructions.push_back(add_inst);
                        instructions.push_back(new LoadImm(LoadImm::VLOAD, dst, tmp, 0));
                        father->gep_with_shift.erase(gep);
                        father->value_map[gep] = tmp;
                    }
                    else {
                        auto load_inst = new LoadReg(LoadReg::SLOAD, dst, lhs, rhs);
                        load_inst->setShift(new Shift(Shift::LSL, bits));
                        instructions.push_back(load_inst);
                    }
                }
                else if (father->gep_with_reg.count(gep)) {
                    Reg* dst = father->get_reg(load, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    Reg* rhs = father->gep_with_reg[gep];

                    if (load->getType()->isFloatTy()) {
                        Reg* tmp = new Reg(father->reg_num++);
                        instructions.push_back(new BinaryReg(BinaryReg::ADD, tmp, lhs, rhs));
                        instructions.push_back(new LoadImm(LoadImm::VLOAD, dst, tmp, 0));
                        father->gep_with_reg.erase(gep);
                        father->value_map[gep] = tmp;
                    }
                    else {
                        instructions.push_back(new LoadReg(LoadReg::SLOAD, dst, lhs, rhs));
                    }
                }
                else {
                    // B("here");
                    Reg* dst = father->get_reg(load, this);
                    Reg* lhs = father->get_reg(ir_src, this);
                    auto store_inst = new LoadImm();
                    if (load->getType()->isFloatTy()) 
                        store_inst = new LoadImm(LoadImm::VLOAD, dst, lhs, 0);
                    else
                        store_inst = new LoadImm(LoadImm::SLOAD, dst, lhs, 0);                    
                    instructions.push_back(store_inst);
                    // assert(0);
                }
                continue;                
            }


            if (auto alloc = dynamic_cast<ir::AllocaInst*>(ir_src)) {
                auto it = father->store_alloc_map.find(alloc);
                if (it != father->store_alloc_map.end()) {
                    father->value_map[load] = it->second;
                    continue;
                }
            }

            Reg* src = father->get_reg(ir_src, this);
            Reg* dst = father->get_reg(load, this);
            if (load->getType()->isFloatTy())
                instructions.push_back(new LoadImm(LoadImm::VLOAD, dst, src, 0));
            else
                instructions.push_back(new LoadImm(LoadImm::SLOAD, dst, src, 0));
        } 
        else if (auto gep = dynamic_cast<ir::GEPInstruction*>(*i)) {
            // R(sum);
            // R(pairs.size());

            ir::Value* start = gep->getPtr();
            Reg* base = father->get_reg(start, this);
            // B("in");
            // if (father->block_map[block]->name == "then__2")
            //     G((*i)->getSignature());
            int sum = 0;
            std::vector<std::pair<ir::Value*, int>> pairs;
            ir::Type* type = start->getType()->getInnerType();

            for(ir::Value* idx : gep->getIndex()) {
                if (auto constant = dynamic_cast<ir::ConstantInt*>(idx)) {
                    sum += constant->getValue() * type->getAllocatedSize();
                }
                else if (type->getAllocatedSize()) {
                    // G(type->getAllocatedSize());
                    pairs.push_back({idx, type->getAllocatedSize()});
                }
                type = type->getInnerType(); 
                
            }
            // if (father->block_map[block]->name == "then__2") {
            //     G(sum);
            //     G(pairs.size());
            // }

            // start可能是局部数组、全局数组以及另一条gep
            // 如果是另一条gep



            Reg* new_base;
            if (auto another_gep = dynamic_cast<ir::GEPInstruction*>(start)) {
                if (!sum && pairs.empty()) {
                    // R("here!");
                    if (father->gep_with_constant.count(another_gep))
                        father->gep_with_constant[gep] = father->gep_with_constant[another_gep];
                    else if (father->gep_with_shift.count(another_gep))
                        father->gep_with_shift[gep] = father->gep_with_shift[another_gep];
                    else if (father->gep_with_reg.count(another_gep)) {
                        father->gep_with_reg[gep] = father->gep_with_reg[another_gep];  
                    }
                    father->value_map[gep] = base;
                    continue;                  
                }
                else if (sum && pairs.empty()) {
                    // R("here");
                    // B("here");
                    // B((*i)->getSignature());
                    // B(sum);
                    new_base = new Reg(father->reg_num++);
                    if (father->gep_with_constant.count(another_gep)) {
                        father->gep_with_constant[gep] = father->gep_with_constant[another_gep] + sum;
                        new_base = base;
                    }
                    else {
                        if (Function::analyseImm(sum))
                            instructions.push_back(new BinaryImm(BinaryImm::ADD, new_base, base, sum));
                        else if (Function::analyseImm(-sum))
                            instructions.push_back(new BinaryImm(BinaryImm::SUB, new_base, base, -sum));
                        else {
                            Reg* sum_reg = father->get_reg(sum, this);
                            instructions.push_back(new BinaryReg(BinaryReg::ADD, new_base, base, sum_reg));
                        }


                        if (father->gep_with_shift.count(another_gep)) 
                            father->gep_with_shift[gep] = father->gep_with_shift[another_gep];
                        else if (father->gep_with_reg.count(another_gep)) 
                            father->gep_with_reg[gep] = father->gep_with_reg[another_gep]; 
                    }
                    father->value_map[gep] = new_base;
                    continue;
                }
                else if (!sum && pairs.size()) {
                    new_base = new Reg(father->reg_num++);
                    if (father->gep_with_constant.count(another_gep)) {
                        int offset = father->gep_with_constant[another_gep];
                        if (Function::analyseImm(offset))
                            instructions.push_back(new BinaryImm(BinaryImm::ADD, new_base, base, offset));
                        else if (Function::analyseImm(-offset))
                            instructions.push_back(new BinaryImm(BinaryImm::SUB, new_base, base, -offset));
                        else {
                            Reg* offset_reg = father->get_reg(offset, this);
                            instructions.push_back(new BinaryReg(BinaryReg::ADD, new_base, base, offset_reg));
                        }
                    }
                    else if (father->gep_with_shift.count(another_gep)) {
                        Reg* rhs = father->gep_with_shift[another_gep].first;
                        int bits = father->gep_with_shift[another_gep].second;
                        auto add_inst = new BinaryReg(BinaryReg::ADD, new_base, base, rhs);
                        add_inst->setShift(new Shift(Shift::LSL, bits));

                        instructions.push_back(add_inst);
                    }
                    else if (father->gep_with_reg.count(another_gep)) {
                        Reg* rhs = father->gep_with_reg[another_gep];
                        auto add_inst = new BinaryReg(BinaryReg::ADD, new_base, base, rhs);

                        instructions.push_back(add_inst);
                    }
                    else
                        new_base = base;
                }
                else 
                    assert(0);
            }
            else
                new_base = base;



            if (pairs.empty()) {
                // G("here");
                // G((*i)->getSignature());
                if (father->alloc_address.count(start)) {
                    if (sum == 0) 
                        father->alloc_address[gep] = father->alloc_address[start];

                    Reg* dst = father->get_reg(gep, this);
                    Reg* sp = new Reg(13, Reg::PHY);
                    
                    if (father->father->argc == 6) {
                        // assert(father->alloc_address[start]-sum < 4096);
                        int offset = father->stack_size - (father->alloc_address[start]-sum);
                        if (offset > 4096) {
                            Reg* offset_reg = new Reg(father->reg_num++);
                            auto mov2reg = new MoveImm(offset_reg, father->stack_size - (father->alloc_address[start]-sum));
                            instructions.push_back(mov2reg);
                            instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, sp, offset_reg));
                            father->gep_list[mov2reg] = father->alloc_address[start]-sum;
                            father->gep_map[gep] = father->alloc_address[start]-sum;                            
                        }
                        else {
                            auto add_inst = new BinaryImm(BinaryImm::ADD, dst, sp, offset);
                            instructions.push_back(add_inst);
                            father->gep_list[add_inst] = father->alloc_address[start]-sum;
                            father->gep_map[gep] = father->alloc_address[start]-sum;
                        }
                    }
                    else {
                        Reg* offset_reg = new Reg(father->reg_num++);
                        auto mov2reg = new MoveImm(offset_reg, father->stack_size - (father->alloc_address[start]-sum));
                        instructions.push_back(mov2reg);
                        instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, sp, offset_reg));
                        father->gep_list[mov2reg] = father->alloc_address[start]-sum;
                        father->gep_map[gep] = father->alloc_address[start]-sum;
                    }

                    // B(i->getSignature());
                    // R("out1");
                    continue;
                }
            }


            Reg* last;
            if (sum != 0) {
                // 如果只含常数，则推迟到load/store阶段
                if (!pairs.size()) {
                    father->gep_with_constant[gep] = sum;
                    father->value_map[gep] = base;
                    // R("out2");
                    continue;
                }
                
                
                last = new Reg(father->reg_num++);
                if (Function::analyseImm(sum))
                    instructions.push_back(new BinaryImm(BinaryImm::ADD, last, new_base, sum));
                else if (Function::analyseImm(-sum))
                    instructions.push_back(new BinaryImm(BinaryImm::SUB, last, new_base, -sum));
                else {
                    Reg* sum_reg = father->get_reg(sum, this);
                    instructions.push_back(new BinaryReg(BinaryReg::ADD, last, new_base, sum_reg));
                }
            }
            else
                last = new_base;

            // std::sort(pairs.begin(), pairs.end(), cmp);
            // for (auto i : pairs)
            //     std::cout << i.second << " ";
            // std::cout << "\n";


            for (auto it = pairs.begin(); it != pairs.end(); ++it) {
                Reg* lhs = father->get_reg((*it).first, this);
                int offset = (*it).second;
                auto factor = std::make_pair((*it).first, offset);
                // 如果是最后一维，可以被并入load/store
                if (std::next(it) == pairs.end()) {
                    if( !(offset & (offset-1)) ) {
                        int lowbit = std::log2(offset);
                        father->gep_with_shift[gep] = std::make_pair(lhs, lowbit);
                        // R("out3");
                    }
                    // TODO:if find
                    else {
                        // R("here!");              
                        Reg* tmp = new Reg(father->reg_num++);
                        Reg* rhs = father->get_reg(offset, this);
                        instructions.push_back(new BinaryReg(BinaryReg::MUL, tmp, lhs, rhs));
                        gep_factor_map.insert({factor, tmp});
                        father->gep_with_reg[gep] = tmp;
                        // R("out4");
                    }
                    break;
                }

                Reg* dst = new Reg(father->reg_num++);
                if (!(offset & (offset-1))) {
                    int lowbit = std::log2(offset);
                    Shift* shift = new Shift(Shift::LSL, lowbit);
                    BinaryReg* add = new BinaryReg(BinaryReg::ADD, dst, last, lhs);
                    add->setShift(shift);
                    instructions.push_back(add);
                }
                else if (gep_factor_map.count(factor)) {
                    Reg* rhs = gep_factor_map[factor];
                    instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, last, rhs));
                }
                else {
                    Reg* tmp = new Reg(father->reg_num++);
                    Reg* rhs = father->get_reg(offset, this);
                    instructions.push_back(new BinaryReg(BinaryReg::MUL, tmp, lhs, rhs));
                    gep_factor_map.insert({factor, tmp});                    
                    instructions.push_back(new BinaryReg(BinaryReg::ADD, dst, last, tmp));
                }

                // Reg* rhs = father->get_reg(pair.second, this);

                // instructions.push_back(new ComplexMul(ComplexMul::MLA, dst, lhs, rhs, last));
                last = dst;
            }
            // Reg* dst = father->get_reg(gep, this);
            // instructions.push_back(new MoveReg(MoveReg::MOV, dst, last));
            father->value_map[gep] = last;


            // std::cout << "\nafter gep:\n";
            // for (auto i : instructions) {
            //     std::cout << i->gen_asm();
            // }
            // std::cout << "\n\n";
        } 
        else if (auto alloc = dynamic_cast<ir::AllocaInstruction*>(*i)) {
            if (alloc->getAllocatedType()->isPointerTy()) 
                continue;

            int size = alloc->getAllocatedType()->getAllocatedSize();
            Reg* sp = new Reg(13, Reg::PHY);

            Reg* offset_reg = new Reg(father->reg_num++, Reg::VIRTUAL);
            Reg* alloc_reg = new Reg(father->reg_num++, Reg::VIRTUAL);

            // if (father->father->argc == 6) {
            //     // B("here");
            //     auto add_inst = new BinaryImm(BinaryImm::ADD, alloc_reg, sp, 0);
            //     father->alloc_list.push_back(add_inst);

            //     father->stack_size += size;
            //     father->alloc_offset[add_inst] = father->stack_size;
            //     father->alloc_address[alloc] = father->stack_size;
            //     assert(father->alloc_address[alloc] < 4096);
            //     father->value_map[alloc] = alloc_reg;
            // }
            // else {
                auto move_inst = new MoveImm(offset_reg, 0);
                father->alloc_list.push_back(move_inst);
                auto add_inst = new BinaryReg(BinaryReg::ADD, alloc_reg, sp, offset_reg);
                father->alloc_list.push_back(add_inst);

                father->stack_size += size;
                father->alloc_offset[move_inst] = father->stack_size;
                father->alloc_address[alloc] = father->stack_size;
                father->value_map[alloc] = alloc_reg;
            // }
        }
        else if (auto move = dynamic_cast<ir::MoveInstruction*>(*i)) {
            std::map<ir::Value*, int> in_degree;
            std::map<ir::Value*, int> out_degree;
            std::set<std::pair<ir::Value*, ir::Value*>> sides;
            // 建图
            while (auto cur = dynamic_cast<ir::MoveInstruction*>(*i)) {
                ir::Value* ir_rhs = cur->getIncomingVal();
                ir::Value* ir_lhs = cur->getTarget();
                if (auto constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
                    int rhs = constant->getValue();
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    instructions.push_back(new MoveImm(lhs, rhs));
                }
                else if (auto constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
                    Reg* rhs = father->get_reg(constant->getValue(), this);
                    Reg* lhs = father->get_reg(ir_lhs, this);
                    instructions.push_back(new MoveReg(MoveReg::VVMOV, lhs, rhs));
                }
                else {
                    sides.insert({ir_rhs, ir_lhs});

                    in_degree[ir_lhs] = in_degree.count(ir_lhs) ? in_degree[ir_lhs]+1 : 1;
                    out_degree[ir_rhs] = out_degree.count(ir_rhs) ? out_degree[ir_rhs]+1 : 1;
                }

                auto next_it = std::next(i);
                if (next_it == ir_insts.end()) 
                    break;
                else if (auto cur = dynamic_cast<ir::MoveInstruction*>(*next_it)) 
                    ++i;
                else 
                    break;
            }



            std::map<ir::Value*, Reg*> copy;
            // 找出度为0的点，找不到就拆一个
            while (sides.size()) {
                // B(sides.size());
                bool flag = false;
                for (auto it = sides.begin(); it != sides.end();) {
                    if (!out_degree[(*it).second]) {
                        Reg* rhs = copy.count((*it).first) ? copy[(*it).first] : father->get_reg((*it).first, this);
                        Reg* lhs = father->get_reg((*it).second, this);
                        if (rhs->kind == Reg::SREG)
                            instructions.push_back(new MoveReg(MoveReg::MOV, lhs, rhs));
                        else
                            instructions.push_back(new MoveReg(MoveReg::VVMOV, lhs, rhs));
                        out_degree[(*it).first]--;
                        in_degree[(*it).second]--;
                        it = sides.erase(it);
                        flag = true;
                        continue;
                    }
                    else
                        ++it;
                }

                if (!flag) {
                    auto side = sides.begin();
                    ir::Value* ir_rhs = (*side).first;
                    ir::Value* ir_lhs = (*side).second;
                    Reg* rhs = father->get_reg(ir_rhs, this);
                    Reg* lhs = father->get_reg(ir_lhs, this);                    
                    if ((*side).second->getType()->isFloatTy()) {
                        Reg* copy_reg = new Reg(father->vreg_num++, Reg::VIRTUAL, Reg::VREG);
                        instructions.push_back(new MoveReg(MoveReg::VVMOV, copy_reg, lhs));
                        copy.insert({ir_lhs, copy_reg});
                        instructions.push_back(new MoveReg(MoveReg::VVMOV, lhs, rhs));
                    }
                    else {
                        Reg* copy_reg = new Reg(father->reg_num++, Reg::VIRTUAL, Reg::SREG);                    
                        instructions.push_back(new MoveReg(MoveReg::MOV, copy_reg, lhs));
                        copy.insert({ir_lhs, copy_reg});
                        instructions.push_back(new MoveReg(MoveReg::MOV, lhs, rhs));
                    }


                    out_degree[ir_rhs]--;
                    in_degree[ir_lhs]--;
                    sides.erase(side);
                }
            }

            // ir::Value* ir_rhs = move->getIncomingVal();
            // if (auto constant = dynamic_cast<ir::ConstantInt*>(ir_rhs)) {
            //     int rhs = constant->getValue();
            //     Reg* lhs = father->get_reg(move->getTarget(), this);
                
            //     instructions.push_back(new MoveImm(lhs, rhs));
            // }
            // else if (auto constant = dynamic_cast<ir::ConstantFP*>(ir_rhs)) {
            //     Reg* rhs = father->get_reg(constant->getValue(), this);
            //     Reg* lhs = father->get_reg(move->getTarget(), this);
            //     instructions.push_back(new MoveReg(MoveReg::VVMOV, lhs, rhs));
            // }
            // else {
            //     if (ir_rhs->getType()->isFloatTy()) {
            //         Reg* rhs = father->get_reg(move->getIncomingVal(), this);
            //         Reg* lhs = father->get_reg(move->getTarget(), this);
            //         instructions.push_back(new MoveReg(MoveReg::VVMOV, lhs, rhs));
            //     }   
            //     else {
            //         Reg* rhs = father->get_reg(move->getIncomingVal(), this);
            //         Reg* lhs = father->get_reg(move->getTarget(), this);
            //         instructions.push_back(new MoveReg(MoveReg::MOV, lhs, rhs));
            //     }             
            // }
        }
    }
}

void Block::insertFront(std::vector<Instruction*> insts) {
    auto it = instructions.begin();
    while (auto i = dynamic_cast<Push*>(*it))
        ++it;

    instructions.insert(it, insts.begin(), insts.end());
}

void Block::insertBack(std::vector<Instruction*> insts) {
    bool flag = false;

    auto it = instructions.end();

    while (--it != instructions.begin()) {
        if (auto ret = dynamic_cast<Return*>(*it)) 
            continue;
        else if (auto pop = dynamic_cast<Pop*>(*it))
            continue;
        else {
            instructions.insert(++it, insts.begin(), insts.end());
            flag = true;
            break;
        }
    }

    if (!flag) {
        if (it == instructions.begin()) {
            if (auto ret = dynamic_cast<Return*>(*it)) 
                instructions.insert(it, insts.begin(), insts.end());
            else if (auto pop = dynamic_cast<Pop*>(*it))
                instructions.insert(it, insts.begin(), insts.end());
            else
                instructions.insert(++it, insts.begin(), insts.end());
        }
        else
            instructions.insert(++it, insts.begin(), insts.end());
    }

}

void Block::setSuccessor(std::vector<ir::BasicBlock*> v) {
    for (auto bb : v)
        successor.insert({father->block_map[bb], 1});
}

void Block::setPredecessor(std::vector<ir::BasicBlock*> v) {
    for (auto bb : v) 
        predecessor.insert({father->block_map[bb], 1});
}

std::string Block::gen_asm() {
    std::string code = "";
    code += name + ":\n";
    // std::cout << "start instructions ASM generate\n";
    for (auto i : instructions) {
        code += i->gen_asm();
    }

    // std::cout << "finish instructions ASM generate\n";

    return code;
}

}
}
