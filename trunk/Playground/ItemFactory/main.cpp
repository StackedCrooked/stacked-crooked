#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H


#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
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
 * ItemFactory is a object factory that allows you to create objects using string ids.
 *
 * @example
 *
 *  // Define types
 *  struct Base {
 *      virtual ~Base() {}
 *  };
 *
 *  struct A : Base {};
 *  struct B : Base {};
 *
 *  // Create a factory for objects that inherit "Base".
 *  ItemFactory<Base> factory;
 *
 *  // Register types A as "a" and B as "b".
 *  factory.registerType<A>("a");
 *  factory.registerType<B>("b");
 *
 *  // Create an object and specifying its type as a template argument.
 *  std::unique_ptr<A> a = factory.create<A>();
 *
 *  // Create an object and specify its type using a string identifier.
 *  // In this case an object of the Base type is returned.
 *  std::unique_ptr<Base> b = factory.create("b");
 *
 *  // Use dynamic cast to get an object of derived type.
 *  B & b = dynamic_cast<B&>(*b);
 *
 */
template<typename BaseType_>
struct ItemFactory
{
    PP_DefineItemFactoryException(NotRegistered, "No create function.");
    PP_DefineItemFactoryException(AlreadyRegistered, "Already registered.");

    typedef BaseType_ BaseType;
    typedef std::unique_ptr<BaseType> BasePtr;

    template<typename T>
    void registerType(const std::string & inName)
    {
        if (mCreateFunctions.find(inName) != mCreateFunctions.end())
        {
            throw AlreadyRegistered(inName);
        }

        CreateFunction cf = [](){ return BasePtr(new T); };
        mCreateFunctions.insert(std::make_pair(inName, cf));
    }

    BasePtr create(std::string inName)
    {
        auto it = mCreateFunctions.find(inName);
        if (it == mCreateFunctions.end())
        {
            throw NotRegistered(std::move(inName));
        }

        return it->second();
    }

private:
    typedef std::function<BasePtr()> CreateFunction;
    typedef std::map<std::string, CreateFunction> CreateFunctions;
    CreateFunctions mCreateFunctions;
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

    std::string mValue;
};


struct B : Base
{
    B() : mValue("b")
    {
        std::cout << "B()" << std::endl;
    }

    std::string mValue;
};


} // namespace Playground


int main()
{
    using namespace Playground;
    ItemFactory<Base> factory;
    factory.registerType<A>("a");
    factory.registerType<B>("b");

    std::unique_ptr<Base> base_a = factory.create("a");
    std::unique_ptr<Base> base_b = factory.create("b");

    A & a = dynamic_cast<A&>(*base_a);
    std::cout << "A: " << &a << ", " << a.mValue << std::endl;

    B & b = dynamic_cast<B&>(*base_b);
    std::cout << "B: " << &b << ", " << b.mValue << std::endl;
}


#endif // ITEMFACTORY_H
