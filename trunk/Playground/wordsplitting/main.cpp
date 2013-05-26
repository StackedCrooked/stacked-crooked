#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

typedef std::vector<std::string> Words;

Words get_words(const std::string& path, Words::size_type limit = -1)
{
    std::vector<std::string> result;
    std::ifstream is(path);
    std::string word;
    while (result.size() < limit && is >> word)
    {
        result.push_back(word);
    }

    std::sort(result.begin(), result.end());
    return result;
}

int split(const std::string& text, std::string::size_type offset, const Words& input, Words& output, std::size_t count)
{
    assert(offset < text.size());
    assert(count < 100);
    for (const std::string& word : input)
    {
        std::string::size_type current_offset = offset;
        std::string::size_type current_word_index = 0;
        while (true)
        {
            if (current_offset == text.size())
            {
                break;
            }

            if (current_word_index == word.size())
            {
                break;
            }

            if (text.at(current_offset) != word.at(current_word_index))
            {
                break;
            }
            current_offset++;
            current_word_index++;
        }

        if (current_word_index == word.size())
        {
            output.push_back(word);
            std::cout << "Adding \"" << output.back() << "\" to the set." << std::endl;

            if (text.size() == current_offset)
            {
                return true;
            }
            else if (split(text, current_offset, input, output, count + 1))
            {
                return true;
            }
            else
            {
                std::cout << "Removing \"" << output.back() << "\" from the set." << std::endl;
                output.pop_back();
            }
        }
    }
    return false;
}

Words split(const std::string& text, const Words& input)
{
    std::cout << "Split " << text << std::endl;
    Words result;
    if (!split(text, 0, input, result, 0))
    {
        result.clear();
    }
    return result;
}

int main()
{
    // zillions, yesterday, zig, zigzagged, zag, zigzagging, zigzags
    Words result = split("zillionsyesterdayzigzigzaggedzagzigzaggingzigzags", get_words("words.txt"));
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
