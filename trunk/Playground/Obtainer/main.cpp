#include <iostream>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>


#define ITEMOBTAINER_DEFINE_EXCEPTION(NAME, INFO) \
    struct NAME : std::runtime_error { \
        NAME() : std::runtime_error(INFO) { } \
        virtual ~NAME() throw() {} \
    }



template<typename BaseType>
struct ItemFactory
{
    ITEMOBTAINER_DEFINE_EXCEPTION(NoCreateFunction, "No create function.");
    ITEMOBTAINER_DEFINE_EXCEPTION(AlreadyRegistered, "Already registered.");

    template<typename SubType>
    void createItem()
    {
        static_assert(std::is_base_of<BaseType, SubType>::value, "Provided type is not a subtype of base type.");
        auto it = mCreateFunctions.find(std::type_index(typeid(SubType)));
        if (it == mCreateFunctions.end())
        {
            throw NoCreateFunction();
        }
        return *it->second;
    }

    template<typename SubType>
    void registerItem()
    {
        static_assert(std::is_base_of<BaseType, SubType>::value, "Provided type is not a subtype of base type.");
        std::type_index id(typeid(SubType));
        auto it = mCreateFunctions.find(id);
        if (it != mCreateFunctions.end())
        {
            throw AlreadyRegistered();
        }
        auto fun = [](){ return std::unique_ptr<BaseType>(new SubType); };
        mCreateFunctions.insert(std::make_pair(id, fun));
    }


    typedef std::function<std::unique_ptr<BaseType>()> CreateFunction;
    typedef std::map<std::type_index, CreateFunction> CreateFunctions;
    CreateFunctions mCreateFunctions;
};


struct ItemBase
{
    virtual ~ItemBase() {}
};

struct ItemA : ItemBase {};
struct ItemB : ItemBase {};
struct ItemC : ItemBase {};


int main()
{
    ItemFactory<ItemBase> f;
    f.registerItem<ItemA>();
    f.registerItem<ItemB>();
    f.registerItem<ItemC>();
}
