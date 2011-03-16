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


template<class T>
static Variant MakeVariant(typename TypeWrapper<T>::CONSTREFTYPE inValue)
{
    Variant var;
    var.setValue<T>(inValue);
    return var;
}


/**
 * DynamicObject allows you to add member variables at runtime.
 *
 * The variable type is used as key. Therefore only one object of any type can be added.
 * Use a container type if you need to store multiple values of one type.
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
    template<class T>
    void set(typename TypeWrapper<T>::CONSTREFTYPE inValue)
    {
        mMembers.insert(std::make_pair(&typeid(typename TypeWrapper<T>::TYPE),
                                       MakeVariant<T>(inValue)));
    }


    template<class T>
    bool has() const
    {
        const std::type_info * typeInfo = &typeid(typename TypeWrapper<T>::TYPE);
        return mMembers.find(typeInfo) != mMembers.end();
    }

    template<class T>
    typename TypeWrapper<T>::CONSTREFTYPE get() const
    {
        const std::type_info * typeInfo = &typeid(typename TypeWrapper<T>::TYPE);
        Members::const_iterator it = mMembers.find(typeInfo);
        if (it == mMembers.end())
        {
            throw std::logic_error(std::string("Dynamic object does not contain objects of type: ") + typeid(typename TypeWrapper<T>::TYPE).name());
        }
        return it->second.getValue<T>();
    }

    template<class T>
    typename TypeWrapper<T>::REFTYPE get()
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
    obj.set<int>(3);
    obj.set<std::string>("Hello");
    obj.set<bool>(false);
    std::cout << "int: "     << obj.get<int>() << std::endl;
    std::cout << "string: " << obj.get<std::string>() << std::endl;
    std::cout << "bool: " << obj.get<bool>() << std::endl;

    // This would throw
    try
    {
        obj.get< std::vector<int> >();
    }
    catch (const std::exception & exc)
    {
        std::cout << "Hah! " << exc.what() << std::endl;
    }

    std::cout << "Has char? Answer: " << (obj.has<char>() ? "Yes" : "No") << "!" << std::endl;
    std::cout << "Has int? Answer: " << (obj.has<int>() ? "Yes" : "No") << "!" << std::endl;
    std::cout << "Has Foo? Answer: " << (obj.has<Foo>() ? "Yes" : "No") << "!" << std::endl;
    return 0;
}

