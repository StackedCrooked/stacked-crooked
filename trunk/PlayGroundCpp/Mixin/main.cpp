#include <iostream>
#include <string>
#include <typeinfo>

#define TRACE std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
#define TRACE_MIXIN std::cout << __PRETTY_FUNCTION__ << std::endl << std::flush;
#define TRACE_BASE std::cout << "\n" << __PRETTY_FUNCTION__ << std::endl << std::flush;

template<class BaseType>
struct Savable : public BaseType {
    Savable() { TRACE_MIXIN }

    template<class Arg0>
    Savable(Arg0 & arg0) : BaseType(arg0) { TRACE_MIXIN }

    template<class Arg0, class Arg1>
    Savable(Arg0 & arg0, Arg1 & arg1) : BaseType(arg0, arg1) { TRACE_MIXIN }

    void Save(const std::string & inFileName) {}
};

template<class BaseType>
struct Loadable : public BaseType {
    Loadable() { TRACE_MIXIN }

    template<class Arg0>
    Loadable(Arg0 & arg0) : BaseType(arg0) { TRACE_MIXIN }

    template<class Arg0, class Arg1>
    Loadable(Arg0 & arg0, Arg1 & arg1) : BaseType(arg0, arg1) { TRACE_MIXIN }

    void Load(const std::string & inFileName) {}
};

namespace CopyLand { // Test with a copyable Arg0 object

struct MyObject  {};

struct ValueBase {
    ValueBase(MyObject inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    MyObject mObject;
    std::string mTest;
};

struct ConstValueBase {
    ConstValueBase(const MyObject inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    const MyObject mObject;
    std::string mTest;
};

struct PtrBase {
    PtrBase(MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    MyObject * mObject;
    std::string mTest;
};

struct ConstPtrBase {
    ConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    const MyObject * mObject;
    std::string mTest;
};

struct RefBase {
    RefBase(MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    MyObject & mObject;
    std::string mTest;
};

struct ConstRefBase {
    ConstRefBase(const MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    const MyObject & mObject;
    std::string mTest;
};

struct LoadableAndSavableValueBase : public Loadable<Savable<ValueBase> > {
    LoadableAndSavableValueBase(MyObject inObject, const std::string & inTest) :
        Loadable<Savable<ValueBase> >(inObject, inTest) { TRACE }
};

struct LoadableAndSavableConstValueBase : public Loadable<Savable<ConstValueBase> > {
    LoadableAndSavableConstValueBase(const MyObject inObject, const std::string & inTest) :
        Loadable<Savable<ConstValueBase> >(inObject, inTest) { TRACE }
};

struct LoadableAndSavablePtrBase : public Loadable<Savable<PtrBase> > {
    LoadableAndSavablePtrBase(MyObject * inObject, const std::string & inTest) :
         Loadable<Savable<PtrBase> >(inObject, inTest) { TRACE }
};

struct LoadableAndSavableRefBase : public Loadable<Savable<RefBase> > {
    LoadableAndSavableRefBase(MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<RefBase> >(inObject, inTest) { TRACE }
};

struct LoadableAndSavableConstPtrBase : public Loadable<Savable<ConstPtrBase> > {
    LoadableAndSavableConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<ConstPtrBase> >(inObject, inTest) { TRACE }
};

struct LoadableAndSavableConstRefBase : public Loadable<Savable<ConstRefBase> > {
    LoadableAndSavableConstRefBase(const MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<ConstRefBase> >(inObject, inTest) { TRACE }
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
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    MyObject * mObject; std::string mTest;
};

struct ConstPtrBase {
    ConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    const MyObject * mObject; std::string mTest;
};

struct RefBase {
    RefBase(MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    MyObject & mObject; std::string mTest;
};

struct ConstRefBase {
    ConstRefBase(const MyObject & inObject, const std::string & inTest) :
        mObject(inObject), mTest(inTest) { TRACE_BASE }
    const MyObject & mObject; std::string mTest;
};

struct LoadableAndSavablePtrBase : public Loadable<Savable<PtrBase> > {
    LoadableAndSavablePtrBase(MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<PtrBase> >(inObject, inTest)  { TRACE }
};

struct LoadableAndSavableRefBase : public Loadable<Savable<RefBase> > {
    LoadableAndSavableRefBase(MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<RefBase> >(inObject, inTest)  { TRACE }
};

struct LoadableAndSavableConstPtrBase : public Loadable<Savable<ConstPtrBase> > {
    LoadableAndSavableConstPtrBase(const MyObject * inObject, const std::string & inTest) :
        Loadable<Savable<ConstPtrBase> >(inObject, inTest)  { TRACE }
};

struct LoadableAndSavableConstRefBase : public Loadable<Savable<ConstRefBase> > {
    LoadableAndSavableConstRefBase(const MyObject & inObject, const std::string & inTest) :
        Loadable<Savable<ConstRefBase> >(inObject, inTest)  { TRACE }
};

void test() {
    MyObject myObject;
    LoadableAndSavablePtrBase ptrBase(&myObject, "Test");
    LoadableAndSavableConstPtrBase constPtrBase(&myObject, "Test");
    LoadableAndSavableRefBase refBase(myObject, "Test");
    LoadableAndSavableConstRefBase constRefBase(myObject, "Test");
}

} // namespace NoCopyLand

int main() {
    CopyLand::test();
    NoCopyLand::test();
    return 0;
}
