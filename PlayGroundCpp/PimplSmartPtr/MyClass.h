#ifndef MYCLASS_H
#define MYCLASS_H


#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <string>


class MyClass
{
public:
	MyClass(const std::string & inName);

	~MyClass();

	const std::string & name() const;

private:
	MyClass(const MyClass &);
	MyClass& operator=(const MyClass &);

	struct Impl;
	boost::scoped_ptr<Impl> mImpl;
    //boost::shared_ptr<Impl> mImpl;
    //std::auto_ptr<Impl> mImpl;
};


#endif MYCLASS_H
