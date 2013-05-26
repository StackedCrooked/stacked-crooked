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
    std::cout << "Recurse remainder: " << text.substr(offset) << std::endl;
    for (std::string::size_type ti = offset; ti < text.size(); ++ti)
    {
        std::cout << "ti: " << ti << std::endl;
        for (const std::string& word : input)
        {
            std::string::size_type current_text_index = ti;
            std::string debug;
            std::string::size_type current_word_index = 0;
            while (true)
            {
                //std::cout << "ti: " << ti << ", testing: " << word << ", current_text_index: " << current_text_index << ", current_word_index: " << current_word_index << std::endl;
                if (current_text_index == text.size())
                {
                    //std::cout << "    eof1 " << std::endl;
                    break;
                }

                if (current_word_index == word.size())
                {
                    //std::cout << "    match1 " << word << std::endl;
                    break;
                }

                if (text.at(current_text_index) != word.at(current_word_index))
                {
                    //std::cout << "    fail1 " << word << std::endl;
                    break;
                }
                debug += text.at(current_text_index);
                current_text_index++;
                current_word_index++;
            }

            if (current_word_index == word.size())
            {
                //std::cout << "    match2 " << debug << std::endl;
                assert(debug == word);

                Words output_copy = output;
                output_copy.push_back(word);

                if (text.size() == current_text_index)
                {
                    std::cout << "word was: " << word << std::endl;
                    std::cout << " end of text " << std::endl;
                    output = output_copy;
                    return true;
                }

                if (split(text, current_text_index, input, output_copy, count + 1))
                {
                    std::cout << " end of recurse " << std::endl;
                    output = output_copy;
                    return true;
                }
            }
        }
        return false;
    }
    return false;
}

Words split(const std::string& text, const Words& input)
{
    Words result;
    if (!split(text, 0, input, result, 0))
    {
        result.clear();
    }
    return result;
}

int main()
{
    Words result = split("zillionsyesterday", get_words("words.txt"));
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
