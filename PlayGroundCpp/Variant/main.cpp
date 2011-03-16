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
	Variant() { }

	template<class T>
	Variant(T inValue) :
		mImpl(new VariantImpl<typename TypeWrapper<T>::TYPE>(inValue))
	{
	}

	template<class T>
	typename TypeWrapper<T>::REFTYPE getValue() const
	{
		return dynamic_cast<VariantImpl<typename TypeWrapper<T>::TYPE>&>(*mImpl.get()).mValue;
	}

	template<class T>
	void setValue(typename TypeWrapper<T>::CONSTREFTYPE inValue)
	{
		mImpl.reset(new VariantImpl<typename TypeWrapper<T>::TYPE>(inValue));
	}

private:
	struct AbstractVariantImpl
	{
		virtual ~AbstractVariantImpl() {}
	};

	template<class T>
	struct VariantImpl : public AbstractVariantImpl
	{
		VariantImpl(T inValue) : mValue(inValue) { }

		~VariantImpl() {}

		T mValue;
	};

	boost::shared_ptr<AbstractVariantImpl> mImpl;
};

int main()
{
	// Store int
	Variant v(10);
	int & a = v.getValue<const int &>();
	std::cout << "a = " << a << std::endl;

	// Store float
	v.setValue<float>(12.34);
	float & f = v.getValue<float>();
	std::cout << "f = " << f << std::endl;
	f += 1.0;
	std::cout << "f2 = " << v.getValue<float>() << std::endl;

	// Store map<string, string>
	typedef std::map<std::string, std::string> Mapping;
	Mapping m;
	m["one"] = "uno";
	m["two"] = "due";
	m["three"] = "tre";
	v.setValue<const Mapping>(m);
	Mapping & m2 = v.getValue<const Mapping&>();
	std::cout << "m2[\"one\"] = " << m2["one"] << std::endl;


	// Trigger a bad_cast exception!
	try
	{
		double d = v.getValue<double>();
	}
	catch (const std::exception & exc)
	{
		std::cout << "Exception caught: " << exc.what() << std::endl;
	}
	std::cout << "Graceful program exit." << std::endl;
	
	return 0;
}

