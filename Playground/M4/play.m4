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
    std::vector<std::string> wordList;
    wordList.push_back("one");
    wordList.push_back("two");
    wordList.push_back("three");

    // nested foreach
    FOREACH(wordList, const std::string & word,
        FOREACH(word, char c,
            // print the character
            printf("Char: %c.\n", c);))
    return 0;
}

