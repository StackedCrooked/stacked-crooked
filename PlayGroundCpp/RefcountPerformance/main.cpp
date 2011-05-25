#include "Poco/Stopwatch.h"
#include <boost/intrusive_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_set.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <ctime>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include "Poco/RefCountedObject.h"


class RefcountedObject : boost::noncopyable
{
public:
    RefcountedObject() : mRefCount(1) { }

    friend void intrusive_ptr_add_ref(RefcountedObject *);
    friend void intrusive_ptr_release(RefcountedObject *);

protected:
    ~RefcountedObject() {}

    std::size_t mRefCount;
};

void intrusive_ptr_add_ref(RefcountedObject * obj) { ++obj->mRefCount; }
void intrusive_ptr_release(RefcountedObject * obj) { if (--obj->mRefCount == 0) delete obj; }


class MyClass : boost::noncopyable
{
public:
    MyClass() : mIdentifier(sInstanceCount++) {}

    bool operator<(const MyClass & rhs) {
        return mIdentifier < rhs.mIdentifier;
    }

private:
    std::size_t mIdentifier;
    static std::size_t sInstanceCount;
};

std::size_t MyClass::sInstanceCount = 0;


class MyClassWithRefcount : public RefcountedObject
{
public:
    MyClassWithRefcount() :
        mIdentifier(++sInstanceCount)
    {
    }

    bool operator<(const MyClassWithRefcount & rhs)
    {
        return mIdentifier < rhs.mIdentifier;
    }

    std::size_t mIdentifier;
    static std::size_t sInstanceCount;
};

std::size_t MyClassWithRefcount::sInstanceCount = 0;


template<class Container>
time_t TestCopy(const Container & inContainer, std::size_t n) {
    Poco::Stopwatch stopwatch;
    stopwatch.start();
    for (std::size_t idx = 0; idx < n; ++idx)
    {
        volatile Container copy = inContainer;
    }
    return static_cast<time_t>(0.5 + (double(stopwatch.elapsed()) / 1000.0));
}


void TestSet(std::size_t n) {
    std::cout << std::endl << "Set" << std::endl;

    {
        std::set<boost::shared_ptr<MyClass> > theSet;
        for (std::size_t idx = 0; idx < 256; ++idx)
        {
            boost::shared_ptr<MyClass> obj(new MyClass);
            theSet.insert(obj);
        }
        std::cout << "shared_ptr: " << TestCopy(theSet, n) << "ms" << std::endl;
    }

    {
        std::set< boost::intrusive_ptr<MyClassWithRefcount> > theSet;
        for (std::size_t idx = 0; idx < 256; ++idx)
        {
            theSet.insert(new MyClassWithRefcount);
        }
        std::cout << "intrusive_ptr: " << TestCopy(theSet, n) << "ms" << std::endl;
    }
}


void TestVector(std::size_t n) {
    std::cout << std::endl << "Vector" << std::endl;

    {
        std::vector<boost::shared_ptr<MyClass> > theVector;
        for (std::size_t idx = 0; idx < 256; ++idx)
        {
            boost::shared_ptr<MyClass> obj(new MyClass);
            theVector.push_back(obj);
        }
        std::cout << "shared_ptr: " << TestCopy(theVector, n) << "ms" << std::endl;
    }

    {
        std::vector< boost::intrusive_ptr<MyClassWithRefcount> > theVector;
        for (std::size_t idx = 0; idx < 256; ++idx)
        {
            theVector.push_back(new MyClassWithRefcount);
        }
        std::cout << "intrusive_ptr: " << TestCopy(theVector, n) << "ms" << std::endl;
    }
}


int main(int argc, char ** argv) {
    std::size_t n = 10000;
    if (argc == 2)
    {
        n = boost::lexical_cast<std::size_t>(argv[1]);
    }
    TestVector(n);
    TestSet(n);
    return 0;
}

