#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>


#define TRACE std::cout << __FILE__ << ":" << __LINE__ << "\t\t\t" << __PRETTY_FUNCTION__ << std::endl;


void test_auto()
{
	std::vector<std::string> words;
	words.push_back("one");
	words.push_back("two");
	words.push_back("three");
	words.push_back("four");
	words.push_back("five");
	words.push_back("six");
	words.push_back("seven");


	std::cout << "Using 'auto it' in for loop..." << std::endl;
	for (auto it = words.begin(); it != words.end(); ++it)
	{
		const std::string & word(*it);
		std::cout << word << " ";
	}
    std::cout << std::endl;
    std::cout << "Done.\n\n";


	std::cout << "Using for_each + lambda..." << std::endl;
	std::for_each(words.begin(), words.end(), [] (const std::string & word) { std::cout << word << " "; });
    std::cout << std::endl;
    std::cout << "Done.\n\n";


	std::cout << "Using find_if + lambda..." << std::endl;
	auto it = std::find_if(
		words.begin(),
		words.end(),
		[] (const std::string & word) {
			return word.size() == 5;
		} );

	if (it != words.end())
	{
		std::cout << "find_if succeeded: " << *it << std::endl;
	}
	else
	{
		std::cout << "find_if failed (not found)" << std::endl;
	}
    std::cout << "Done.\n\n";


	std::cout << "Using sort + lambda..." << std::endl;
	std::sort(
		words.begin(),
		words.end(),
		[] (const std::string & lhs, const std::string & rhs) {
			return lhs.size() < rhs.size();
		} );
    std::cout << "Done.\n\n";


	std::cout << "Using for_each + lambda..." << std::endl;
	std::for_each(
		words.begin(),
		words.end(),
		[] (const std::string & s) {
			std::cout << s << " ";
		} );
    std::cout << std::endl;
    std::cout << "Done.\n\n";
}


// Swap implemented with move-semantics
template<class T>
void moving_swap(T & lhs, T & rhs)
{
    T tmp(std::move(lhs));
    lhs = std::move(rhs);
    rhs = std::move(tmp);
}


class Array
{
public:
    // Constructor taking length
    Array(int inLength, int inInitialValue = 0) :
        mData(static_cast<int*>(malloc(inLength * sizeof(int)))),
        mLength(inLength)
    {
        for (int idx = 0; idx < mLength; ++idx)
        {
            mData[idx] = inInitialValue;
        }
    }

    Array(const std::vector<int> & inData) :
        mData(0),
        mLength(static_cast<int>(inData.size()))
    {
        mData = static_cast<int*>(malloc(mLength * sizeof(int)));
        for (int idx = 0; idx < mLength; ++idx)
        {
            mData[idx] = inData[idx];
        }
    }


    // Destructor
    ~Array()
    {
        free(mData);
    }

    // Move constructor
    Array(Array && rhs) :
        mData(rhs.mData),
        mLength(rhs.mLength)
    {
        std::cout << "This is the move constructor.\n";
        // Invalidate the right-hand side
        rhs.mData = 0;
        rhs.mLength = 0;
    }

    // Assignment operator
    Array& operator=(Array && rhs)
    {
        std::cout << "This is the move operator.\n";
        moving_swap(mData, rhs.mData);
        moving_swap(mLength, rhs.mLength);
        return *this;
    }

    int length() const
    {
        return mLength;
    }

    const int & operator[](size_t inIndex) const 
    {
        return mData[inIndex];
    }

    int & operator[](size_t inIndex)
    {
        return mData[inIndex];
    }

private:
    // non-copyable
    Array(const Array&);
    Array& operator=(const Array&);

    int * mData;
    int mLength;
};


Array MakeArray(int inFrom, int inTo, int inStep)
{
    std::vector<int> helper;
    for (size_t idx = inFrom; idx < inTo; idx += inStep)
    {
        helper.push_back(inFrom + idx);
    }
    return Array(helper);
}


void test_move_semantics()
{
    std::cout << "Construct vector with array literal.\n";
    std::vector<double> v = { 1, 2, 3, 4 };
    std::cout << "Done.\n\n";

    std::cout << "Swap with std::move.\n";
    std::vector<double> w;
    moving_swap(v, w);
    assert(v.empty());
    assert(w.size() == 4);
    std::cout << "Done.\n\n";

    std::cout << "Simplest rvalue-reference sample\n";
    class A {};
    A && a = A(); 
    std::cout << "Done.\n\n";

    // Simple Array test
    {
        std::cout << "Testing Array Normal Construction\n";
        Array array0(10, 0);
        for (size_t idx = 0; idx < array0.length(); ++idx)
        {
            array0[idx] = idx;
        }

        for (size_t idx = 0; idx < array0.length(); ++idx)
        {
            if (idx != 0)
            {
                std::cout << ", ";
            }
            std::cout << array0[idx];
        }
        std::cout << "\nDone.\n\n";
    }


    // Test move operator
    {
        std::cout << "Testing move operator.\n";
        Array array(10, 0);
        Array other_array(5, 0);

        // Invokes the move operator
        array = std::move(other_array);
    }

    // Test move constructor 1
    {
        std::cout << "Testing Array move constructor 1\n";
        Array array1(Array(10, 2));
        std::cout << "Done.\n\n";
    }


    // Test move constructor 2
    {
        std::cout << "Testing Array move constructor 2\n";
        Array array2(MakeArray(10, 20, 3));
        for (size_t idx = 0; idx < array2.length(); ++idx)
        {
            std::cout << array2[idx] << " ";
        }
        std::cout << std::endl;
        std::cout << "Done.\n\n";
    }

    // Test move constructor 3
    {
        std::cout << "Testing Array move constructor 3\n";
        Array array3(10, 3);
        for (size_t idx = 0; idx < array3.length(); ++idx)
        {
            array3[idx] *= idx;
            std::cout << array3[idx] << " ";
        }
        std::cout << std::endl;

        // Print the array length before it's moved.
        std::cout << "Before move\narray3 length: " << array3.length() << std::endl;

        // Move the array
        Array array3_moved = std::move(array3);

        // Print the array lengths after the move.
        std::cout << "\nAfter move:\narray3 length: " << array3.length() << std::endl;

        std::cout << "array3_moved length: " << array3_moved.length() << std::endl;

        // Print the contents of the moved array
        std::cout << "array3_moved contents:\n";
        for (size_t idx = 0; idx < array3_moved.length(); ++idx)
        {
            std::cout << array3_moved[idx] << " ";
        }
        std::cout << std::endl;
        std::cout << "Done.\n\n";
    }
}


int main()
{
    test_auto();
    test_move_semantics();
	return 0;
}


