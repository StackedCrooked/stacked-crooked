#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
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
    Variant()
    {
    }

    template<class T>
    Variant(typename TypeWrapper<T>::CONSTREFTYPE inValue) :
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

template<typename T>
struct Identity
{
    typedef T type;
};

class TypeInfo
{
public:
    TypeInfo() {}

    // Template constructor of non-template class is only possible
    // if you pass an argument of the template type.
    template<class T>
    TypeInfo(Identity<T> id) :
        mTypeInfoImpl(new TypeInfoImpl(id))
    {
    }

    const char * name() const
    {
        return mTypeInfoImpl->mTypeInfo.name();
    }

private:
    friend bool operator<(const TypeInfo & lhs, const TypeInfo & rhs);

    struct TypeInfoImpl
    {
        template<class T>
        TypeInfoImpl(Identity<T> id) :
            mTypeInfo(typeid(id)) // Copy constructor is avoided here by storing
            // a const reference instead of an object.
        {
        }

        // C++ deliberately specifies that binding a temporary object to a
        // reference to const on the stack lengthens the lifetime of the
        // temporary to the lifetime of the reference itself.
        // See: http://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/
        const std::type_info & mTypeInfo;
    };

    // Make our class copyable by storing a pointer.
    boost::shared_ptr<TypeInfoImpl> mTypeInfoImpl;
};

bool operator<(const TypeInfo & lhs, const TypeInfo & rhs)
{
    return lhs.mTypeInfoImpl->mTypeInfo.before(rhs.mTypeInfoImpl.get()->mTypeInfo);
}

template<class T>
static TypeInfo TypeId()
{
    return TypeInfo(Identity<T>());
}


// A few tests.
TypeInfo TestTypeInfo()
{
    TypeInfo t1 = TypeId<int>();
    std::cout << "t1.name: " << const_cast<TypeInfo&>(t1).name() << std::endl;
    TypeInfo t2 = TypeId<bool>();
    std::cout << "t2.name: " << const_cast<TypeInfo&>(t2).name() << std::endl;
    t2 = t1;
    std::cout << "t2.name is now: " << t2.name() << std::endl;
    return t1;
}

template<class T>
class MyObject
{
public:
    MyObject() : mTypeInfo(&typeid(this)) {}
    const std::type_info * mTypeInfo;
};

void TestWithPointer()
{
    const std::type_info * t1 = &typeid(int);
    MyObject<int> obj1;
    MyObject<int> obj2;
    obj2 = obj1;

}


class DynamicObject
{
public:
    template<class T>
    void set(typename TypeWrapper<T>::CONSTREFTYPE inValue)
    {
        Variant var;
        var.setValue<T>(inValue);
        const std::type_info * typeInfo = &typeid(typename TypeWrapper<T>::TYPE);
        mMembers.insert(std::make_pair(typeInfo, var));
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

private:
    typedef std::map<const std::type_info*, Variant> Members;
    Members mMembers;
};


int main()
{
    DynamicObject obj;
    obj.set<int>(3);
    obj.set<std::string>("Hello");
    obj.set<bool>(false);
    std::cout << "int: "     << obj.get<int>() << std::endl;
    std::cout << "string: " << obj.get<std::string>() << std::endl;
    std::cout << "bool: " << obj.get<bool>() << std::endl;
    return 0;
}

