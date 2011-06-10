#include <iostream>
#include <string>
#include <typeinfo>

#define TRACE std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
#define TRACE_MIXIN std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
#define TRACE_BASE std::cout << "\n" << __PRETTY_FUNCTION__ << std::endl << std::flush;


template <typename T>
struct TypeWrapper
{
    typedef T TYPE;
};


template <typename T>
struct TypeWrapper<const T>
{
    typedef const T TYPE;
};


template <typename T>
struct TypeWrapper<const T&>
{
    typedef const T& TYPE;
};


template <typename T>
struct TypeWrapper<T&>
{
    typedef T& TYPE;
};



// Savable is a mixin class for saving object state to file.
template<class BaseType>
struct Savable : public BaseType {
    Savable() {
        TRACE_MIXIN
    }

    template<class Arg0>
    Savable(typename TypeWrapper<Arg0>::TYPE arg0) : BaseType(arg0) {
        TRACE_MIXIN
    }

    template<class Arg0, class Arg1>
    Savable(typename TypeWrapper<Arg0>::TYPE arg0, typename TypeWrapper<Arg1>::TYPE arg1) : BaseType(arg0, arg1) {
        TRACE_MIXIN
    }

    void Save(const std::string & inFileName) {}
};

// Loadable is a mixin class for load object state from file.
template<class BaseType>
struct Loadable : public BaseType {
    Loadable() {
        TRACE_MIXIN
    }

    template<class Arg0>
    Loadable(typename TypeWrapper<Arg0>::TYPE arg0) : BaseType(arg0) {
        TRACE_MIXIN
    }

    template<class Arg0, class Arg1>
    Loadable(typename TypeWrapper<Arg0>::TYPE arg0, typename TypeWrapper<Arg1>::TYPE arg1) : BaseType(arg0, arg1) {
        TRACE_MIXIN
    }

    void Load(const std::string & inFileName) {}
};

// Test with a copyable constructor args
namespace CopyLand {

struct MyObject  {}; // copyable

struct ValueBase {
    ValueBase(MyObject inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    MyObject mObject;
    std::string mTest;
};

struct ConstValueBase {
    ConstValueBase(const MyObject inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    const MyObject mObject;
    std::string mTest;
};

struct PtrBase {
    PtrBase(MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    MyObject * mObject;
    std::string mTest;
};

struct ConstPtrBase {
    ConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    const MyObject * mObject;
    std::string mTest;
};

struct RefBase {
    RefBase(MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    MyObject & mObject;
    std::string mTest;
};

struct ConstRefBase {
    ConstRefBase(const MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    const MyObject & mObject;
    std::string mTest;
};

struct LoadableAndSavableValueBase : public Loadable<Savable<ValueBase> > {
    LoadableAndSavableValueBase(MyObject inObject, const std::string & inTest) :
        Loadable<Savable<ValueBase> >(inObject, inTest) {
        TRACE
    }
};

struct LoadableAndSavableConstValueBase : public Loadable<Savable<ConstValueBase> > {
    LoadableAndSavableConstValueBase(const MyObject inObject, const std::string & inTest) :
        Loadable<Savable<ConstValueBase> >(inObject, inTest) {
        TRACE
    }
};

struct LoadableAndSavablePtrBase : public Loadable<Savable<PtrBase> > {
    LoadableAndSavablePtrBase(MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<PtrBase> >(inObject, inTest) {
        TRACE
    }
};

struct LoadableAndSavableRefBase : public Loadable<Savable<RefBase> > {
    LoadableAndSavableRefBase(MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<RefBase> >(inObject, inTest) {
        TRACE
    }
};

struct LoadableAndSavableConstPtrBase : public Loadable<Savable<ConstPtrBase> > {
    LoadableAndSavableConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<ConstPtrBase> >(inObject, inTest) {
        TRACE
    }
};

struct LoadableAndSavableConstRefBase : public Loadable<Savable<ConstRefBase> > {
    LoadableAndSavableConstRefBase(const MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<ConstRefBase> >(inObject, inTest) {
        TRACE
    }
};

void test() {
    MyObject myObject;
    LoadableAndSavableValueBase valueBase(myObject, "Test");
    LoadableAndSavableConstValueBase constValueBase(myObject, "Test");
    LoadableAndSavablePtrBase ptrBase(&myObject, "Test");
    LoadableAndSavableConstPtrBase constPtrBase(&myObject, "Test");
    LoadableAndSavableRefBase refBase(myObject, "Test");
    LoadableAndSavableConstRefBase constRefBase(myObject, "Test");
}

} // namespace CopyLand

namespace NoCopyLand { // Test with a noncopyable Arg0 object

struct MyObject {
    MyObject() {}

    // noncopyable
    MyObject(const MyObject&);
    const MyObject& operator=(const MyObject&);
};

struct PtrBase {
    PtrBase(MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    MyObject * mObject;
    std::string mTest;
};

struct ConstPtrBase {
    ConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    const MyObject * mObject;
    std::string mTest;
};

struct RefBase {
    RefBase(MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    MyObject & mObject;
    std::string mTest;
};

struct ConstRefBase {
    ConstRefBase(const MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) {
        TRACE_BASE
    }
    const MyObject & mObject;
    std::string mTest;
};

struct LoadableAndSavablePtrBase : public Loadable<Savable<PtrBase> > {
    LoadableAndSavablePtrBase(MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<PtrBase> >(inObject, inTest)  {
        TRACE
    }
};

struct LoadableAndSavableRefBase : public Loadable<Savable<RefBase> > {
    LoadableAndSavableRefBase(MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<RefBase> >(inObject, inTest)  {
        TRACE
    }
};

struct LoadableAndSavableConstPtrBase : public Loadable<Savable<ConstPtrBase> > {
    LoadableAndSavableConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<ConstPtrBase> >(inObject, inTest)  {
        TRACE
    }
};

struct LoadableAndSavableConstRefBase : public Loadable<Savable<ConstRefBase> > {
    LoadableAndSavableConstRefBase(const MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<ConstRefBase> >(inObject, inTest)  {
        TRACE
    }
};

// Also include a test with std::type_info.
struct TypeInfo {
    TypeInfo(const std::type_info * inTypeInfo) : mTypeInfo(inTypeInfo) {}

    const std::type_info * mTypeInfo;
};

struct LoadableAndSavableTypeInfo : public Loadable<Savable<TypeInfo> > {
    LoadableAndSavableTypeInfo() : Loadable<Savable<TypeInfo> >(&typeid(this)) {}
};

void test() {
    MyObject myObject;
    LoadableAndSavablePtrBase ptrBase(&myObject, "Test");
    LoadableAndSavableConstPtrBase constPtrBase(&myObject, "Test");
    LoadableAndSavableRefBase refBase(myObject, "Test");
    LoadableAndSavableConstRefBase constRefBase(myObject, "Test");
    LoadableAndSavableTypeInfo typeInfo;
    typeInfo.mTypeInfo.name(); // get the std::type_info
}

} // namespace NoCopyLand

int main() {
    CopyLand::test();
    NoCopyLand::test();
    return 0;
}
