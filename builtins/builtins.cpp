#include "../include/builtins.h"

namespace monkey{
    // len
    std::shared_ptr<Object> len(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(len). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() == "STRING"){
            return std::make_shared<Integer>(static_cast<int64_t>(std::dynamic_pointer_cast<Strin>(args[0])->value.size()));
        } else if(args[0]->type() == "ARRAY"){
            return std::make_shared<Integer>(static_cast<int64_t>(std::dynamic_pointer_cast<Array>(args[0])->elements.size()));
        } else {
            return std::make_shared<Error>("argument to `len` not supported, got " + args[0]->type());
        }
    }

    // first
    std::shared_ptr<Object> first(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(first). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `first` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            if(arr->elements.size() > 0){
                return arr->elements[0];
            } else {
                return nullptr;
            }
        }
    }

    // last
    std::shared_ptr<Object> last(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(last). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `last` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            if(arr->elements.size() > 0){
                return arr->elements[arr->elements.size() - 1];
            } else {
                return nullptr;
            }
        }
    }

    // rest 接受一个数组，返回一个新数组，新数组包含原数组除第一个元素外的所有元素
    std::shared_ptr<Object> rest(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(rest). got=" + std::to_string(args.size()) + ", want=1");
        } else if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `rest` must be ARRAY, got " + args[0]->type());
        } else {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            if(arr->elements.size() > 0){
                std::vector<std::shared_ptr<Object>> newElements;
                for(int i = 1; i < arr->elements.size(); ++i){
                    newElements.push_back(arr->elements[i]);
                }
                return std::make_shared<Array>(newElements);
            } else {
                return nullptr;
            }
        }
    }

    // push 接受一个数组和一个元素，返回一个新数组，新数组包含原数组的所有元素和新元素
    std::shared_ptr<Object> push(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 2){
            return std::make_shared<Error>("wrong number of arguments in builtin function(push). got=" + std::to_string(args.size()) + ", want=2");
        }
        if(args[0]->type() != "ARRAY"){
            return std::make_shared<Error>("argument to `push` must be ARRAY, got " + args[0]->type());
        }
        auto arr = std::dynamic_pointer_cast<Array>(args[0]);
        std::vector<std::shared_ptr<Object>> newElements;
        for(int i = 0; i < arr->elements.size(); ++i){
            newElements.push_back(arr->elements[i]);
        }
        newElements.push_back(args[1]);
        return std::make_shared<Array>(newElements);
    }

    // puts 
    std::shared_ptr<Object> print(std::vector<std::shared_ptr<Object>> args){
        for(auto& arg : args){
            std::cout << arg->inspect() << " ";
        }
        std::cout << std::endl;
        return nullptr;
    }

    // transform integer to string
    std::shared_ptr<Object> str(std::vector<std::shared_ptr<Object>> args){
        if(args.size() != 1){
            return std::make_shared<Error>("wrong number of arguments in builtin function(to_string). got=" + std::to_string(args.size()) + ", want=1");
        }
        if(args[0]->type() != "INTEGER"){
            return std::make_shared<Error>("argument to `str` must be INTEGER, got " + args[0]->type());
        }
        return std::make_shared<Strin>(std::to_string(std::dynamic_pointer_cast<Integer>(args[0])->value));
    }

    // concat two strings or two arrays
    std::shared_ptr<Object> concat(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 2) {
            return std::make_shared<Error>("wrong number of arguments in builtin function(concat). got=" + std::to_string(args.size()) + ", want=2");
        }
        if (args[0]->type() != args[1]->type()) {
            return std::make_shared<Error>("arguments to `concat` must be the same type, got " + args[0]->type() + " and " + args[1]->type());
        }
        if (args[0]->type() == "STRING") {
            return std::make_shared<Strin>(std::dynamic_pointer_cast<Strin>(args[0])->value + std::dynamic_pointer_cast<Strin>(args[1])->value);
        } else if (args[0]->type() == "ARRAY") {
            auto arr1 = std::dynamic_pointer_cast<Array>(args[0]);
            auto arr2 = std::dynamic_pointer_cast<Array>(args[1]);
            std::vector<std::shared_ptr<Object>> newElements;
            newElements.reserve(arr1->elements.size() + arr2->elements.size());
            for (auto& e : arr1->elements) {
                newElements.push_back(e);
            }
            for (auto& e : arr2->elements) {
                newElements.push_back(e);
            }
            return std::make_shared<Array>(newElements);
        } else {
            return std::make_shared<Error>("arguments to `concat` must be STRING or ARRAY, got " + args[0]->type());
        }
    }

    // zip 
    std::shared_ptr<Object> zip(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 2) {
            return std::make_shared<Error>("wrong number of arguments in builtin function(zip). got=" + std::to_string(args.size()) + ", want=2");
        }
        if (args[0]->type() != "ARRAY" || args[1]->type() != "ARRAY") {
            return std::make_shared<Error>("arguments to `zip` must be ARRAY, got " + args[0]->type() + " and " + args[1]->type());
        }
        auto arr1 = std::dynamic_pointer_cast<Array>(args[0]);
        auto arr2 = std::dynamic_pointer_cast<Array>(args[1]);
        std::map<std::shared_ptr<HashKey>, std::shared_ptr<HashPair>> pairs;
        int len = std::min(arr1->elements.size(), arr2->elements.size());
        for (int i = 0; i < len; ++i) {
            auto key = arr1->elements[i];
            auto value = arr2->elements[i];
            auto pair = std::make_shared<HashPair>(key, value);
            if (key->type() != "STRING" & key->type() != "INTEGER" & key->type() != "BOOLEAN") {
                return std::make_shared<Error>("in builtin function `zip`, unusable as hash key: " + key->inspect() + "(" + key->type() + ")");
            }
            if (std::dynamic_pointer_cast<Strin>(key)) {
                auto k = std::dynamic_pointer_cast<Strin>(key);
                pairs[k->hashKey()] = pair;
            } else if (std::dynamic_pointer_cast<Integer>(key)) {
                auto k = std::dynamic_pointer_cast<Integer>(key);
                pairs[k->hashKey()] = pair;
            } else if (std::dynamic_pointer_cast<Boolea>(key)) {
                auto k = std::dynamic_pointer_cast<Boolea>(key);
                pairs[k->hashKey()] = pair;
            }
        }
        return std::make_shared<HashTable>(pairs);
    }

    // set 
    std::shared_ptr<Object> set(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 1) {
            return std::make_shared<Error>("wrong number of arguments in builtin function(set). got=" + std::to_string(args.size()) + ", want=1");
        }
        if (args[0]->type() != "ARRAY") {
            return std::make_shared<Error>("argument to `set` must be ARRAY, got " + args[0]->type());
        }
        auto arr = std::dynamic_pointer_cast<Array>(args[0]);
        std::map<HashKey, std::shared_ptr<Object>> map;
        for (auto& e : arr->elements) {
            if (e->type() != "STRING" & e->type() != "INTEGER" & e->type() != "BOOLEAN") {
                return std::make_shared<Error>("in builtin function `set`, unusable as hash key: " + e->inspect() + "(" + e->type() + ")");
            }
            if (std::dynamic_pointer_cast<Strin>(e)) {
                auto k = std::dynamic_pointer_cast<Strin>(e);
                map[*(k->hashKey())] = e;
            } else if (std::dynamic_pointer_cast<Integer>(e)) {
                auto k = std::dynamic_pointer_cast<Integer>(e);
                map[*(k->hashKey())] = e;
            } else if (std::dynamic_pointer_cast<Boolea>(e)) {
                auto k = std::dynamic_pointer_cast<Boolea>(e);
                map[*(k->hashKey())] = e;
            }
        }
        std::vector<std::shared_ptr<Object>> setElements;
        for (auto& e : map) {
            setElements.push_back(e.second);
        }
        return std::make_shared<Array>(setElements);
    }

    // type
    std::shared_ptr<Object> type(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 1) {
            return std::make_shared<Error>("wrong number of arguments in builtin function `type`. got=" + std::to_string(args.size()) + ", want=1");
        }
        return std::make_shared<Strin>(args[0]->type());
    }

    // cut
    std::shared_ptr<Object> cut(std::vector<std::shared_ptr<Object>> args) {
        auto numArgs = args.size();
        if (numArgs != 2 && numArgs != 3) {
            return std::make_shared<Error>("wrong number of arguments in builtin function `sub`. got=" + std::to_string(numArgs) + ", want=2 or 3");
        }
        if (args[0]->type() != "STRING" && args[0]->type() != "ARRAY") {
            return std::make_shared<Error>("first argument to `sub` must be STRING or ARRAY, got " + args[0]->type());
        }
        if (args[0]->type() == "STRING") {
            auto str = std::dynamic_pointer_cast<Strin>(args[0]);
            int start_pos = 0;
            int end_pos = str->value.size();
            if (args.size() == 2) {
                if (args[1]->type() != "INTEGER") {
                    return std::make_shared<Error>("second argument to `sub` must be INTEGER, got " + args[1]->type());
                }
                auto start = std::dynamic_pointer_cast<Integer>(args[1]);
                if (start->value < 0 || start->value >= str->value.size()) {
                    return std::make_shared<Error>("start index out of range: " + std::to_string(start->value));
                }
                start_pos = start->value;
            }
            if (args.size() == 3) {
                if (args[1]->type() != "INTEGER" || args[2]->type() != "INTEGER") {
                    return std::make_shared<Error>("second and third arguments to `sub` must be INTEGER, got " + args[1]->type() + " and " + args[2]->type());
                }
                auto start = std::dynamic_pointer_cast<Integer>(args[1]);
                auto end = std::dynamic_pointer_cast<Integer>(args[2]);
                if (start->value < 0 || start->value >= str->value.size()) {
                    return std::make_shared<Error>("start index out of range: " + std::to_string(start->value));
                }
                if (end->value < 0 || end->value > str->value.size()) {
                    return std::make_shared<Error>("end index out of range: " + std::to_string(end->value));
                }
                start_pos = start->value;
                end_pos = end->value;
            }
            return std::make_shared<Strin>(str->value.substr(start_pos, end_pos - start_pos));
        }
        if (args[0]->type() == "ARRAY") {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            int start_pos = 0;
            int end_pos = arr->elements.size();
            if (args.size() == 2) {
                if (args[1]->type() != "INTEGER") {
                    return std::make_shared<Error>("second argument to `sub` must be INTEGER, got " + args[1]->type());
                }
                auto start = std::dynamic_pointer_cast<Integer>(args[1]);
                if (start->value < 0 || start->value >= arr->elements.size()) {
                    return std::make_shared<Error>("start index out of range: " + std::to_string(start->value));
                }
                start_pos = start->value;
            }
            if (args.size() == 3) {
                if (args[1]->type() != "INTEGER" || args[2]->type() != "INTEGER") {
                    return std::make_shared<Error>("second and third arguments to `sub` must be INTEGER, got " + args[1]->type() + " and " + args[2]->type());
                }
                auto start = std::dynamic_pointer_cast<Integer>(args[1]);
                if (start->value < 0 || start->value >= arr->elements.size()) {
                    return std::make_shared<Error>("start index out of range: " + std::to_string(start->value));
                }
                auto end = std::dynamic_pointer_cast<Integer>(args[2]);
                if (end->value < 0 || end->value > arr->elements.size()) {
                    return std::make_shared<Error>("end index out of range: " + std::to_string(end->value));
                }
                start_pos = start->value;
                end_pos = end->value;
            }
            std::vector<std::shared_ptr<Object>> newElements;
            for (int i = start_pos; i < end_pos; ++i) {
                newElements.push_back(arr->elements[i]);
            }
            return std::make_shared<Array>(newElements);
        }
        return std::make_shared<Error>("argument to `sub` not supported, got " + args[0]->type());
    }

    // reverse
    std::shared_ptr<Object> reverse(std::vector<std::shared_ptr<Object>> args) {
        if (args.size() != 1) {
            return std::make_shared<Error>("wrong number of arguments in builtin function `reverse`. got=" + std::to_string(args.size()) + ", want=1");
        }
        if (args[0]->type() != "STRING" && args[0]->type() != "ARRAY") {
            return std::make_shared<Error>("argument to `reverse` must be STRING or ARRAY, got " + args[0]->type());
        }
        if (args[0]->type() == "STRING") {
            auto str = std::dynamic_pointer_cast<Strin>(args[0]);
            std::string reversed = str->value;
            std::reverse(reversed.begin(), reversed.end());
            return std::make_shared<Strin>(reversed);
        }
        if (args[0]->type() == "ARRAY") {
            auto arr = std::dynamic_pointer_cast<Array>(args[0]);
            std::vector<std::shared_ptr<Object>> reversed;
            for (int i = arr->elements.size() - 1; i >= 0; --i) {
                reversed.push_back(arr->elements[i]);
            }
            return std::make_shared<Array>(reversed);
        }
        return std::make_shared<Error>("argument to `reverse` not supported, got " + args[0]->type());
    }
};
