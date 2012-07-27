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
    void registerItem(const std::string & inName)
    {
        static_assert(std::is_base_of<BaseType, SubType>::value, "Provided type is not a subtype of base type.");
        std::type_index id(typeid(SubType));
        auto it = mCreateFunctions.find(inName);
        if (it != mCreateFunctions.end())
        {
            throw AlreadyRegistered();
        }

        // Use local functor instead of lambda because lambda currently messes up my editor's indention logic.
        struct Create
        {
            static std::unique_ptr<BaseType> create()
            {
                return std::unique_ptr<BaseType>(new SubType);
            }
        };

        mCreateFunctions.insert(std::make_pair(inName, std::bind(&Create::create)));
    }

    std::unique_ptr<BaseType> createItem(const std::string & inName)
    {
        auto it = mCreateFunctions.find(inName);
        if (it == mCreateFunctions.end())
        {
            throw NoCreateFunction();
        }
        return it->second();
    }

private:
    typedef std::function<std::unique_ptr<BaseType>()> CreateFunction;
    typedef std::map<std::string, CreateFunction> CreateFunctions;
    CreateFunctions mCreateFunctions;
};


template<typename BaseType>
struct ItemObtainer : ItemFactory<BaseType>
{
    BaseType & obtainItem(const std::string & inName)
    {
        auto it = mItems.find(inName);
        if (it == mItems.end())
        {
            std::unique_ptr<BaseType> basePtr = this->createItem(inName);
            mItems.insert(std::make_pair(inName, std::move(basePtr)));
            return *basePtr;
        }
        return *it->second;
    }

private:
    typedef std::unique_ptr<BaseType> ItemPtr;
    typedef std::map<std::string, ItemPtr> Items;
    Items mItems;
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
    ItemObtainer<ItemBase> f;
    f.registerItem<ItemA>("A");
    f.registerItem<ItemB>("B");
    f.registerItem<ItemC>("C");

    ItemBase & base_a = f.obtainItem("A");
    ItemA & a = dynamic_cast<ItemA&>(base_a);
    (void)a;
}
