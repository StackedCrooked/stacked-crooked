#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <typeinfo>
#include <boost/shared_ptr.hpp>


template <typename T>
struct TypeWrapper
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<const T>
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<const T&>
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<T&>
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


class Variant
{
public:
    Variant() { }

    template<class T>
    Variant(T inValue) :
        mImpl(new VariantImpl<typename TypeWrapper<T>::TYPE>(inValue))
    {
    }

    template<class T>
    typename TypeWrapper<T>::REFTYPE getValue()
    {
        return dynamic_cast<VariantImpl<typename TypeWrapper<T>::TYPE>&>(*mImpl.get()).mValue;
    }
    template<class T>
    typename TypeWrapper<T>::CONSTREFTYPE getValue() const
    {
        const VariantImpl<typename TypeWrapper<T>::TYPE> * impl = dynamic_cast<const VariantImpl<typename TypeWrapper<T>::TYPE>*>(mImpl.get());
        if (!impl)
        {
            std::string msg = "Types don't match. Own type: " + mImpl->getClassName();
            msg += ". Requested type: ";
            msg += typeid(typename TypeWrapper<T>::TYPE).name();
            msg += ".";
            throw std::logic_error(msg);
        }
        return impl->mValue;
    }

    template<class T>
    void setValue(typename TypeWrapper<T>::CONSTREFTYPE inValue)
    {
        mImpl.reset(new VariantImpl<typename TypeWrapper<T>::TYPE>(inValue));
    }

private:
    struct AbstractVariantImpl
    {
        virtual const std::string & getClassName() const = 0;
        virtual ~AbstractVariantImpl() {}
    };

    template<class T>
    struct VariantImpl : public AbstractVariantImpl
    {
        VariantImpl(T inValue) :
            mValue(inValue),
            mClassName(typeid(T).name())
        {
        }

        ~VariantImpl() {}

        virtual const std::string & getClassName() const
        {
            return mClassName;
        }

        T mValue;
        std::string mClassName;
    };

    boost::shared_ptr<AbstractVariantImpl> mImpl;
};


/**
 * DynamicObject is a class that allows you to add "member variables" at runtime.
 *
 * These "member variables" are unnamed. Their type-id is used as identifier.
 * Therefore only one object of any type can be added. You can use a container
 * type or a wrapper class if you need to store multiple objects of the same type.
 *
 * Usage example:
 *   DynamicObject obj;
 *   obj.set<int>(3); // add an int
 *   obj.set<std::string>("Hello"); // add a string object
 *   int & var = obj.get<int>(); // get the int as ref
 *   const std::string & msg = obj.get<std::string>(); // get the string as const ref
 *
 * This can be used to get/set private implementation details from within a .cpp file.
 * Other classes that have this knowledge can then access this data from their .cpp file.
 */
class DynamicObject
{
public:

    /**
     * Add a new dynamic member or overwrite an existing one.
     */
    template<class T>
    void setDynamicMember(typename TypeWrapper<T>::CONSTREFTYPE inValue)
    {
        mMembers[&typeid(typename TypeWrapper<T>::TYPE)] = Variant(inValue);
    }

    /**
     * Check if a dynamic member of a certain type exists.
     */
    template<class T>
    bool hasDynamicMember() const
    {
        const std::type_info * typeInfo = &typeid(typename TypeWrapper<T>::TYPE);
        return mMembers.find(typeInfo) != mMembers.end();
    }

    /**
     * Get reference
     */
    template<class T>
    typename TypeWrapper<T>::CONSTREFTYPE getDynamicMember() const
    {
        const std::type_info * typeInfo = &typeid(typename TypeWrapper<T>::TYPE);
        Members::const_iterator it = mMembers.find(typeInfo);
        if (it == mMembers.end())
        {
            throw std::logic_error(std::string("Dynamic object does not contain objects of type: ") + typeid(typename TypeWrapper<T>::TYPE).name());
        }
        return it->second.getValue<T>();
    }

    /**
     * Get const reference
     */
    template<class T>
    typename TypeWrapper<T>::REFTYPE getDynamicMember()
    {
        const std::type_info * typeInfo = &typeid(typename TypeWrapper<T>::TYPE);
        Members::iterator it = mMembers.find(typeInfo);
        if (it == mMembers.end())
        {
            throw std::logic_error(std::string("Dynamic object does not contain objects of type: ") + typeid(typename TypeWrapper<T>::TYPE).name() + std::string("."));
        }
        return it->second.getValue<T>();
    }

private:
    typedef std::map<const std::type_info*, Variant> Members;
    Members mMembers;
};


struct Foo {};


int main()
{
    DynamicObject obj;
    obj.setDynamicMember<int>(3);

    // Get the int as reference
    int & theInt = obj.getDynamicMember<int>();
    std::cout << "int: " << theInt << std::endl;

    // Update the int via the ref
    theInt = 5;

    // "const int &", "cont int" and "const int &" are all considered the same type
    std::cout << "int&: " << obj.getDynamicMember<int &>() << std::endl;
    std::cout << "const int: " << obj.getDynamicMember<const int>() << std::endl;
    std::cout << "const int &: " << obj.getDynamicMember<const int &>() << std::endl;

    // Store a string and bool
    obj.setDynamicMember<std::string>("Hello");
    obj.setDynamicMember<bool>(false);
    std::cout << "string: " << obj.getDynamicMember<std::string>() << std::endl;
    std::cout << "bool: " << obj.getDynamicMember<bool>() << std::endl;

    // This would throw
    try
    {
        obj.getDynamicMember< std::vector<int> >();
    }
    catch (const std::exception & exc)
    {
        std::cout << "Hah! " << exc.what() << std::endl;
    }

    std::cout << "Has char? Answer: " << (obj.hasDynamicMember<char>() ? "Yes" : "No") << "!" << std::endl;
    std::cout << "Has int? Answer: " << (obj.hasDynamicMember<int>() ? "Yes" : "No") << "!" << std::endl;
    std::cout << "Has Foo? Answer: " << (obj.hasDynamicMember<Foo>() ? "Yes" : "No") << "!" << std::endl;
    return 0;
}

