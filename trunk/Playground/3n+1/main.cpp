#include <algorithm>
#include <iostream>

// For mor information see:
// http://www.programming-challenges.com/english/pdfs/110101.pdf

unsigned cycle_length(unsigned n)
{
	unsigned count = 1;
	while (n != 1)
	{
		if (n%2 == 0)
		{
			n /= 2;
		}
		else
		{
			n *= 3;
			n += 1;
		}
		count++;
	}
	return count;
}


unsigned max_cycle_length(unsigned a, unsigned b)
{
	unsigned result = cycle_length(a);
	for (unsigned i = a + 1; i <= b; ++i)
	{
		unsigned len = cycle_length(i);
		if (len > result)
		{
			result = len;
		}
	}
	return result;
}


void print_max_cycle_length(unsigned a, unsigned b)
{
	std::cout << a << " " << b << " " << max_cycle_length(a, b) << std::endl;
}


int main()
{
	print_max_cycle_length(1, 10);
	print_max_cycle_length(100, 200);
	print_max_cycle_length(201, 210);
	print_max_cycle_length(900, 1000);
	return 0;
}
