#ifndef SYSY_TARGET_REG_H
#define SYSY_TARGET_REG_H
#include <map>
#include <string>
namespace sysy 
{
namespace mc 
{

class Reg {
public:
    enum Type { VIRTUAL, PHY };
    enum Kind { SREG, VREG };
    Type type;
    Kind kind;
    int id;
    int cost = -1;

    static std::map<Type, std::string> type_map;
    static std::map<Kind, std::string> kind_map;
    // static std::map<int, std::string> idMap;


    bool operator <(const Reg &other) {
        return id < other.id;
    }
    bool operator ==(const Reg &other) {
        return id == other.id && type == other.type;
    }


    Reg() : type(Reg::VIRTUAL), id(-1), kind(Reg::SREG) {}
    Reg(int _id) : type(Reg::VIRTUAL), id(_id), kind(Reg::SREG) {}
    Reg(int _id, Type _type) : type(_type), id(_id), kind(Reg::SREG) {}
    Reg(int _id, Type _type, Kind _kind) : type(_type), id(_id), kind(_kind) {} 
    std::string getSignature();
};

struct StackObject {
    int size, position;
};

struct RegAllocStat {
    int spill_cnt, callee_save_used;
    bool succeed,ifonly=false;
};

bool allocable(int reg_id);

} // namespace mc 
} // namespace sysy


#endif