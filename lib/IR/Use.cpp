#include "sysy/IR/Use.h"

namespace sysy
{
namespace ir
{

Use::Use(Value* _value, User* _user, unsigned _operandPos)
        : value(_value), user(_user), operandPos(_operandPos) {}

Use::Use(const Use& use) : value(use.value), user(use.user), operandPos(use.operandPos) {}

Value* Use::getValue() { return value; }
User* Use::getUser() { return user; }
void Use::setValue(Value* _value ) { value = _value; }
void Use::setUser(User* _user) { user = _user; }

} // namespace ir
    
} // namespace sysy
