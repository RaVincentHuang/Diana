#ifndef SYSY_IR_USE_H
#define SYSY_IR_USE_H


namespace sysy
{
namespace ir
{
class User;
class Value;

class Use
{
private:
    Value* value;
    User* user;
    unsigned operandPos;
public:
    Use(Value* _value = nullptr, User* _user = nullptr, unsigned _operandPos = 0);
    Use(const Use& use);
    Value* getValue();
    User* getUser();
    void setValue(Value* _value );
    void setUser(User* _user);
    unsigned getPos() { return operandPos; }
    void reSetPos(unsigned x) { operandPos = x; }
};

} // namespace ir
    
} // namespace sysy


#endif