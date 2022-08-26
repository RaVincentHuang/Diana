#include "sysy/IR/Module.h"
#include "sysy/Target/GlobalObject.h"
#include "sysy/Target/Function.h"
#include <sstream>
#include <iomanip>

namespace sysy 
{
namespace mc 
{

int GlobalObject::id = 0;

void GlobalObject::pushValue(ir::Constant* gv) {
    if (auto constant = dynamic_cast<ir::ConstantInt*>(gv)) {
        values.push_back(constant->getValue());
        size += 4;
    }
    else if (auto constant = dynamic_cast<ir::ConstantFP*>(gv)) {
        values.push_back(Function::getFloatInt(constant->getValue()));
        is_float = true;
        is_zero = false;
        size += 4;
    }
    else if (auto array = dynamic_cast<ir::ConstantArray*>(gv)) {
        for (int i = 0; i < array->size(); i++) 
            pushValue(array->getValue(i));
        
    }
    else if (auto string = dynamic_cast<ir::ConstantString*>(gv)) {
        str_value = string->getStr();
        is_string = true;
    }
    else {
        assert(0);
    }
}


std::string GlobalObject::dec2hex(int i) //将int转成16进制字符串
{
	std::stringstream ioss; //定义字符串流
	std::string s_temp; //存放转化后字符
	ioss << std::setiosflags(std::ios::uppercase) << std::hex << i; //以十六制(大写)形式输出
	//ioss << resetiosflags(ios::uppercase) << hex << i; //以十六制(小写)形式输出//取消大写的设置
	ioss >> s_temp;
	return "0x" + s_temp;
}

std::string GlobalObject::getName() {
    return ".MIN_" + std::to_string(GlobalObject::id++);
}

}
}