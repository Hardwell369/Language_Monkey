#pragma once

#include <iostream>
#include <string>
#include <map>
#include <algorithm>

#include "./object.h"
#include "./errors.h"

namespace monkey{
    struct BuiltinUnit {
        std::string name;
        std::shared_ptr<Builtin> fn;

        BuiltinUnit(std::string name, std::shared_ptr<Builtin> fn) : name(name), fn(fn) {}
    };

    // len 接受一个数组或字符串，返回数组的长度或字符串的长度
    std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> args);

    // first 接受一个数组，返回数组的第一个元素
    std::shared_ptr<Object> first(std::vector<std::shared_ptr<Object>> args);

    // last 接受一个数组，返回数组的最后一个元素
    std::shared_ptr<Object> last(std::vector<std::shared_ptr<Object>> args);

    // rest 接受一个数组，返回一个新数组，新数组包含原数组除第一个元素外的所有元素
    std::shared_ptr<Object> rest(std::vector<std::shared_ptr<Object>> args);

    // push 接受一个数组和一个元素，返回一个新数组，新数组包含原数组的所有元素和新元素
    std::shared_ptr<Object> push(std::vector<std::shared_ptr<Object>> args);

    // print 打印参数
    std::shared_ptr<Object> print(std::vector<std::shared_ptr<Object>> args);

    // str 将参数转换为字符串
    std::shared_ptr<Object> str(std::vector<std::shared_ptr<Object>> args);

    // concat 连接两个字符串或数组
    std::shared_ptr<Object> concat(std::vector<std::shared_ptr<Object>> args);

    // zip 接受两个数组，返回一个字典，字典的键是第一个数组的元素，值是第二个数组的元素
    std::shared_ptr<Object> zip(std::vector<std::shared_ptr<Object>> args);

    // set 接受一个数组, 去重后返回
    std::shared_ptr<Object> set(std::vector<std::shared_ptr<Object>> args);

    // type 返回参数的类型
    std::shared_ptr<Object> type(std::vector<std::shared_ptr<Object>> args);

    // cut 返回字符串的子串, 或数组的子数组
    std::shared_ptr<Object> cut(std::vector<std::shared_ptr<Object>> args);

    // reverse
    std::shared_ptr<Object> reverse(std::vector<std::shared_ptr<Object>> args);

    static std::vector<BuiltinUnit> builtins = {
        BuiltinUnit("len", std::make_shared<Builtin>(len)),
        BuiltinUnit("first", std::make_shared<Builtin>(first)),
        BuiltinUnit("last", std::make_shared<Builtin>(last)),
        BuiltinUnit("rest", std::make_shared<Builtin>(rest)),
        BuiltinUnit("push", std::make_shared<Builtin>(push)),
        BuiltinUnit("print", std::make_shared<Builtin>(print)),
        BuiltinUnit("str", std::make_shared<Builtin>(str)),
        BuiltinUnit("concat", std::make_shared<Builtin>(concat)),
        BuiltinUnit("zip", std::make_shared<Builtin>(zip)),
        BuiltinUnit("set", std::make_shared<Builtin>(set)),
        BuiltinUnit("type", std::make_shared<Builtin>(type)),
        BuiltinUnit("cut", std::make_shared<Builtin>(cut)),
        BuiltinUnit("re", std::make_shared<Builtin>(reverse)),
    };


    inline 
    std::shared_ptr<Builtin> getBuiltin(const std::string& name) {
        for (auto& b : builtins) {
            if (b.name == name) {
                return b.fn;
            }
        }
        return nullptr;
    }
};
