#ifndef PERSON_H
#define PERSON_H


#include <string>


class Person
{
public:
    Person();

	Person(const std::string & inName);

private:
	std::string mName;
};


#endif // PERSON_H

