// Build with this command:
// !m4 % | uncrustify % -l CPP -c uncrustify-config
//
// m4 is the macro language
// uncrustify is a code beautifier

define(`FOREACH', `for (int _idx = 0; _idx != $1.size(); ++_idx)
{
    $2 = $1[_idx];
    $3
}')

#include <string>


int main()
{
    // foreach
    FOREACH(str, char c,
        printf("ASCII %d", c);
        printf("String representation: %c.\n", c);)


    std::vector<std::string> sentence;
    sentence.push_back("one");
    sentence.push_back("two");
    sentence.push_back("three");

    // nested foreach
    FOREACH(sentence, const std::string & word,
        FOREACH(word, char c,
            // print the character
            printf("Char: %c.\n", c);))
    return 0;
}

