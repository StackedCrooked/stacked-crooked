#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H


#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <typeindex>


namespace Playground {


#define PP_DefineItemFactoryException(NAME, INFO) \
    struct NAME : std::runtime_error { \
        NAME(const std::string & inDetail) : std::runtime_error(INFO), mDetail(inDetail) { } \
        virtual ~NAME() throw() {} \
        virtual const char* what() const throw() { return mDetail.c_str(); } \
        const std::string & detail() const { return mDetail; } \
    private:\
        std::string mDetail; \
    }


/**
 * ItemFactory is a simple string-based object factory.
 * @note Requires ItemType to be default-constructible.
 *
 * @example
 *  // Define a simple class hierarchy
 *  struct Base { virtual ~Base() {} };
 *  struct A : Base {};
 *  struct B : Base {};
 *
 *  // Create the factory for this class hierarchy.
 *  ItemFactory<Base> factory;
 *  factory.registerType<A>("a");
 *  factory.registerType<B>("b");
 *  std::unique_ptr<Base> a = factory.create("a");
 *  A & a = dynamic_cast<A&>(*a);
 */
template<typename ItemType>
struct ItemFactory
{
    static_assert(std::is_default_constructible<ItemType>::value, "ItemType is not default constructible.");

    // Define exceptions.
    PP_DefineItemFactoryException(NotRegistered, "No create function.");
    PP_DefineItemFactoryException(AlreadyRegistered, "Already registered.");

    template<typename T>
    void registerType(const std::string & inName)
    {
        if (mCreateFunctions.find(inName) != mCreateFunctions.end())
        {
            throw AlreadyRegistered(inName);
        }

        CreateFunction cf = [](){ return ItemPtr(new T); };
        mCreateFunctions.insert(std::make_pair(inName, cf));
    }

    std::unique_ptr<ItemType> create(const std::string & inName)
    {
        auto it = mCreateFunctions.find(inName);
        if (it == mCreateFunctions.end())
        {
            throw NotRegistered(std::move(inName));
        }

        return it->second();
    }

private:
    typedef std::unique_ptr<ItemType> ItemPtr;
    typedef std::function<ItemPtr()> CreateFunction;
    typedef std::map<std::string, CreateFunction> CreateFunctions;
    CreateFunctions mCreateFunctions;
};


/**
 * ItemObtainer stores at most one instance of each registered item type.
 * The items are lazily created (or reused) when calling obtainItem.
 * @note Requires ItemType to be default-constructible.
 *
 * @example
 *  ItemObtainer<Base> obtainer;
 *  obtainer.registerType<A>("a");
 *  obtainer.registerType<B>("b");
 *  Base & base = obtainer.obtainItem("a");
 *  std::cout << "A value: " << dynamic_cast<A&>(base).mValue << std::endl;
 */
template<typename ItemType>
struct ItemObtainer
{
    static_assert(std::is_default_constructible<ItemType>::value, "ItemType is not default constructible.");

    template<typename T>
    void registerType(const std::string & inName)
    {
        mFactory.registerType<T>(inName);
    }

    ItemType & obtainItem(const std::string & inName)
    {
        auto it = mItems.find(inName);
        if (it != mItems.end())
        {
            return *it->second;
        }

        return *mItems.insert(std::make_pair(inName, mFactory.create(inName))).first->second;
    }

private:
    ItemFactory<ItemType> mFactory;
    typedef std::unique_ptr<ItemType> ItemPtr;
    typedef std::map<std::string, ItemPtr> Items;
    Items mItems;
};


//
// Example
//
struct Base
{
    virtual ~Base() {}

    std::ostream & print(std::ostream & os) const
    {
        return os << static_cast<void*>(const_cast<Base*>(this));
    }
};


struct A : Base
{
    A() : mValue("a")
    {
        std::cout << "A()" << std::endl;
    }

    ~A()
    {
        std::cout << "~A()" << std::endl;
    }

    std::string mValue;
};


struct B : Base
{
    B() : mValue("b")
    {
        std::cout << "B()" << std::endl;
    }

    ~B()
    {
        std::cout << "~B()" << std::endl;
    }

    std::string mValue;
};


} // namespace Playground


int main()
{
    using namespace Playground;

    {
        std::cout << "Testing factory: " << std::endl;
        {
            ItemFactory<Base> factory;
            factory.registerType<A>("a");
            factory.registerType<B>("b");

            factory.create("a");
            factory.create("a");
            factory.create("b");
            factory.create("b");
        }
        std::cout << std::endl << "Done testing factory." << std::endl << std::endl;
    }


    {
        std::cout << "Testing obtainer: " << std::endl;
        {
            ItemObtainer<Base> obtainer;
            obtainer.registerType<A>("a");
            obtainer.registerType<B>("b");
            Base & baseA = obtainer.obtainItem("a");
            std::cout << "A value: " << dynamic_cast<A&>(baseA).mValue << std::endl;
            obtainer.obtainItem("a");
            obtainer.obtainItem("b");
            obtainer.obtainItem("b");
        }
        std::cout << std::endl << "Done testing obtainer." << std::endl << std::endl;
    }
}


#endif // ITEMFACTORY_H
