#ifndef ITEMOBTAINER_H
#define ITEMOBTAINER_H


#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
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


#define ITEMOBTAINER_EXCEPTION(NAME, INFO) \
    struct NAME : std::runtime_error { \
        NAME() : std::runtime_error(INFO) { } \
        virtual ~NAME() throw() {} \
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
 *  factory.registerItem<A>("a");
 *  factory.registerItem<B>("b");
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
struct ItemFactory
{
    ITEMOBTAINER_EXCEPTION(NotRegistered, "No create function.");
    ITEMOBTAINER_EXCEPTION(AlreadyRegistered, "Already registered.");

    template<typename SubType>
    void registerItem(const std::string & inName)
    {
        static_assert(std::is_base_of<BaseType, SubType>::value, "Provided type is not a subtype of base type.");
        nonstd::type_index itemIndex = GetId<SubType>();
        registerItemType(inName, itemIndex);
        registerCreateFunction<SubType>(itemIndex);
    }

    template<typename SubType>
    std::unique_ptr<BaseType> createItem()
    {
        return createItemImpl(GetId<SubType>());
    }

    std::unique_ptr<BaseType> createItem(const std::string & inItemName)
    {
        return createItemImpl(getItemId(inItemName));
    }

protected:
    template<typename T>
    static nonstd::type_index GetId()
    {
        return nonstd::type_index(typeid(T));
    }

    void registerItemType(const std::string & inName, const nonstd::type_index & inTypeIndex)
    {
        if (mItemTypes.find(inName) != mItemTypes.end())
        {
            throw AlreadyRegistered();
        }

        mItemTypes.insert(std::make_pair(inName, inTypeIndex));
    }

    template<typename SubType>
    void registerCreateFunction(const nonstd::type_index & inTypeIndex)
    {
        auto it = mCreateFunctions.find(inTypeIndex);
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

        mCreateFunctions.insert(std::make_pair(inTypeIndex, std::bind(&Create::create)));
    }

    std::unique_ptr<BaseType> createItemImpl(const nonstd::type_index & inTypeIndex)
    {
        auto it = mCreateFunctions.find(inTypeIndex);
        if (it == mCreateFunctions.end())
        {
            throw NotRegistered();
        }
        return it->second();
    }

    nonstd::type_index getItemId(const std::string & inName) const
    {
        auto it = mItemTypes.find(inName);
        if (it == mItemTypes.end())
        {
            throw NotRegistered();
        }
        return it->second;
    }

private:
    typedef std::function<std::unique_ptr<BaseType>()> CreateFunction;
    typedef std::map<std::string, nonstd::type_index> ItemTypes;
    ItemTypes mItemTypes;

    typedef std::map<nonstd::type_index, CreateFunction> CreateFunctions;
    CreateFunctions mCreateFunctions;
};


/**
 * ItemObtainer is a lazy container.
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
 *  ItemObtainer<Base> obtainer;
 *  obtainer.register<A>("a");
 *  obtainer.obtainItem("a"); // creates and returns a new instance of type A
 *  obtainer.obtainItem("a"); // returns previously created instance of type A
 *  obtainer.obtainItem<A>(); // returns previously created instance of type A
 */
template<typename BaseType>
struct ItemObtainer : ItemFactory<BaseType>
{
public:
    BaseType & obtainItem(const std::string & inName)
    {
        return obtainItemImpl(this->getItemId(inName));
    }

    template<typename SubType>
    SubType & obtainItem()
    {
        nonstd::type_index id = this->template GetId<SubType>();
        BaseType & base = obtainItemImpl(id);
        return dynamic_cast<SubType&>(base);
    }

protected:
    BaseType & obtainItemImpl(const nonstd::type_index & inTypeIndex)
    {
        auto it = mItems.find(inTypeIndex);
        if (it == mItems.end())
        {
            std::shared_ptr<BaseType> basePtr(this->createItemImpl(inTypeIndex).release());
            mItems.insert(std::make_pair(inTypeIndex, basePtr));
            return *basePtr;
        }
        return *it->second;
    }

private:
    typedef std::shared_ptr<BaseType> ItemPtr;
    typedef std::map<nonstd::type_index, ItemPtr> Items;
    Items mItems;
};


struct Base
{
    virtual ~Base() {}
};


struct A : Base {};
struct B : Base {};
struct C : Base {};


int main()
{
    ItemObtainer<Base> obtainer;
    obtainer.registerItem<A>("a");
    obtainer.registerItem<B>("b");
    obtainer.registerItem<C>("c");

    A & a = obtainer.obtainItem<A>();
    Base & base_a = obtainer.obtainItem("a");
    assert(&a == &base_a);
}


#endif // ITEMOBTAINER_H
