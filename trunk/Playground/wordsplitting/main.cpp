#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

typedef std::unordered_set<std::string> Words;

Words get_words(const std::string& path, Words::size_type limit = -1)
{
    Words result;
    std::ifstream is(path);
    std::string word;
    while (result.size() < limit && is >> word)
    {
        result.insert(word);
    }
    return result;
}

bool split(const std::string& text, std::string::size_type offset, const Words& word_list, Words& output)
{
    for (std::string::size_type i = 0; i + offset != text.size(); ++i)
    {
        std::string word = text.substr(offset, text.size() - offset - i);
        if (word_list.count(word))
        {
            if (split(text, offset + word.size(), word_list, output))
            {
                output.insert(word);
                return true;
            }
        }
    }
    return offset == text.size();
}

Words split(const std::string& text, const Words& input)
{
    Words result;
    split(text, 0, input, result);
    return result;
}

int main()
{
    // zillions, zig, zigzagged, zag, zigzagging, zigzags
    const std::string s = "zillionszigzigzaggedzagzigzaggingzigzags";
    std::cout << "Splitting \"" << s << "\" in words." << std::endl;
    Words result = split(s, get_words("words.txt"));
    if (!result.empty())
    {
        std::cout << "We found " << result.size() << " words: ";
        for (const std::string& w : result)
        {
            std::cout << w << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Not splittable." << std::endl;
    }
}
