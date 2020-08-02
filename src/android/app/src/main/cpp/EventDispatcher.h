#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <string>

namespace cc
{

class CustomEvent
{
public:
    std::string name;
    union {
        void* ptrVal;
        long longVal;
        int intVal;
        short shortVal;
        char charVal;
        bool boolVal;
    } args[10];

    CustomEvent(){};
    virtual ~CustomEvent(){};
};

class EventDispatcher
{
public:
    using CustomEventListener = std::function<void(const CustomEvent&)>;
    static uint32_t addCustomEventListener(const std::string& eventName, const CustomEventListener& listener);
};
    
} // end of namespace cc
