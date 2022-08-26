#ifndef SYSY_VALUESCOPE_H
#define SYSY_VALUESCOPE_H
#include "sysy/Frontend/Semantics.h"
#include "sysy/IR/Value.h"

namespace sysy
{

namespace ir
{
class ValueScope : public Scope
{
protected:
    ValueScope* father;
    std::map<std::string, Value*> namedValues;
public:
    ValueScope(ValueScope* _father) : father(_father) {}
    ValueScope() : father(nullptr) {}
    ValueScope* getFather() override;
    Value* getValue(std::string varName);
    void insertAlloca(std::string name, Value* value);
    ValueScope* createChild();

    static ValueScope* exitBlock(ValueScope* ptr);

    static ValueScope*& getCurrentPtr()
    {
        static ValueScope* ptr = new ValueScope;
        return ptr;
    }
};
} // namesacpe ir

} // namespace sysy


#endif