#ifndef ClassOBTAINER_H
#define ClassOBTAINER_H


#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/typeof/std/utility.hpp>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <typeinfo>


// Workaround for type_index which is not yet supported on GCC 4.5
// Once we have upgraded to GCC 4.7+ we can replace "nonstd::type_index" with "std::type_index".
namespace nonstd {


struct type_index
{
    type_index(const std::type_info & inInfo) :
        mInfo(&inInfo)
    {
    }

    friend bool operator<(const type_index & lhs, const type_index & rhs)
    {
        return lhs.mInfo->before(*rhs.mInfo);
    }

    friend bool operator==(const type_index & lhs, const type_index & rhs)
    {
        return lhs.mInfo == rhs.mInfo;
    }

    const std::type_info * mInfo;
};


} // namespace nonstd


#define ClassOBTAINER_EXCEPTION(NAME, INFO) \
    struct NAME : std::runtime_error { \
        NAME() : std::runtime_error(INFO) { } \
        virtual ~NAME() throw() {} \
    }


// Use local functor instead of lambda because lambda currently messes up my editor's indention logic.


template<typename BaseType, typename SubType>
struct ConcreteCreator;


template<typename BaseType>
struct Creator
{
    virtual ~Creator() {}

    template<typename SubType>
    ConcreteCreator<BaseType, SubType> & downcast();

    template<typename ...Args>
    std::unique_ptr<BaseType> call(Args ...args)
    {
        auto t = std::make_tuple(args...);
        return callImpl(static_cast<void*>(&t));
    }

    virtual std::unique_ptr<BaseType> callImpl(void *) = 0;
};


template<typename BaseType>
template<typename SubType>
ConcreteCreator<BaseType, SubType> & Creator<BaseType>::downcast()
{
    return dynamic_cast<ConcreteCreator<BaseType, SubType>&>(*this);
}


/**
 * ClassFactory is a object factory that allows you to create objects using string ids.
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
 *  ClassFactory<Base> factory;
 *
 *  // Register types A as "a" and B as "b".
 *  factory.registerClass<A>("a");
 *  factory.registerClass<B>("b");
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
template<typename BaseType>
struct ClassFactory
{
    ClassOBTAINER_EXCEPTION(NotRegistered, "No create function.");
    ClassOBTAINER_EXCEPTION(AlreadyRegistered, "Already registered.");

    template<typename SubType>
    void registerClass(const std::string & inName)
    {
        static_assert(std::is_base_of<BaseType, SubType>::value, "Provided type is not a subtype of base type.");
        nonstd::type_index id = GetId<SubType>();
        registerClassType(inName, id);
        registerCreateFunction<SubType>(id);
    }

    template<typename SubType, typename ...Args>
    std::unique_ptr<BaseType> createClass(Args ...args)
    {
        return createClassImpl(GetId<SubType>(), args...);
    }

    template<typename ...Args>
    std::unique_ptr<BaseType> createClass(const std::string & inClassName, Args ...args)
    {
        return createClassImpl(getClassId(inClassName), args...);
    }

protected:
    template<typename T>
    static nonstd::type_index GetId()
    {
        return nonstd::type_index(typeid(T));
    }

    void registerClassType(const std::string & inName, const nonstd::type_index & inTypeIndex)
    {
        if (mClassTypes.find(inName) != mClassTypes.end())
        {
            throw AlreadyRegistered();
        }

        mClassTypes.insert(std::make_pair(inName, inTypeIndex));
    }

    template<typename SubType>
    void registerCreateFunction(const nonstd::type_index & inTypeIndex)
    {
        auto it = mCreateFunctions.find(inTypeIndex);
        if (it != mCreateFunctions.end())
        {
            throw AlreadyRegistered();
        }

        CreatorPtr ptr(new ConcreteCreator<BaseType, SubType>());
        mCreateFunctions.insert(std::make_pair(inTypeIndex, ptr));
    }

    template<typename ...Args>
    std::unique_ptr<BaseType> createClassImpl(const nonstd::type_index & inTypeIndex, Args ...args)
    {
        auto it = mCreateFunctions.find(inTypeIndex);
        if (it == mCreateFunctions.end())
        {
            throw NotRegistered();
        }

        CreatorPtr ptr = it->second;
        return ptr->call(args...);
    }

    nonstd::type_index getClassId(const std::string & inName) const
    {
        auto it = mClassTypes.find(inName);
        if (it == mClassTypes.end())
        {
            throw NotRegistered();
        }
        return it->second;
    }

private:
    typedef std::map<std::string, nonstd::type_index> ClassTypes;
    ClassTypes mClassTypes;

    typedef std::shared_ptr<Creator<BaseType> > CreatorPtr;
    typedef std::map<nonstd::type_index, CreatorPtr> CreateFunctions;
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
    A()
    {
    }
};


struct B : Base
{
    B(int n) : Base(), _n(n)
    {
        std::cout << "B: " << n << std::endl;
    }

    int _n;
};


template<>
struct ConcreteCreator<Base, A> : Creator<Base>
{
    virtual std::unique_ptr<Base> callImpl(void *)
    {
        return std::unique_ptr<Base>(new A);
    }
};


template<>
struct ConcreteCreator<Base, B> : Creator<Base>
{
    virtual std::unique_ptr<Base> callImpl(void * p)
    {
        std::tuple<int> & t = *reinterpret_cast<std::tuple<int>*>(p);
        return std::unique_ptr<Base>(new B(std::get<0>(t)));
    }
};


int main()
{
    ClassFactory<Base> factory;

    factory.registerClass<A>("a");
    std::unique_ptr<Base> base_a = factory.createClass("a");
    A & a = dynamic_cast<A&>(*base_a);
    std::cout << "A: " << &a << std::endl;

    factory.registerClass<B>("b");
    std::unique_ptr<Base> base_b = factory.createClass("b", 2); // calls B(2)
    B & b = dynamic_cast<B&>(*base_b);
    std::cout << "B: " << &b << std::endl;
}


#endif // ClassOBTAINER_H
