#include <iostream>


enum Digit
{
	Digit_Begin,
	Digit_Zero = Digit_Begin,
	Digit_One,
	Digit_Two,
	Digit_Three,
	Digit_Four,
	Digit_Five,
	Digit_Size,
	Digit_Seven,
	Digit_Eight,
	Digit_Nine,
	Digit_End
};


const char * ToString(Digit inDigit)
{
	static const char * cStrings[] = {
	    "zero", "one", "two", "three", "four", "five", "size", "seven", "eight", "nine"
	};
	return cStrings[inDigit];
}


//
// Automatically convert the enum to its string representation when passing it to an ostream.
//
inline std::ostream& operator<<(std::ostream & os, const Digit & inDigit)
{
	os << ToString(inDigit);
}

int main()
{
	for (unsigned int i = Digit_Begin; i < Digit_End; ++i)
	{
		std::cout << "The digit is: " << i << " (aka \"" << static_cast<Digit>(i) << "\")" << std::endl;
	}
	return 0;
}

