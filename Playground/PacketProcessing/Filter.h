#pragma once


#include <boost/functional/hash.hpp>
#include <cstdint>
#include <vector>



// TODO: Test this hash function and check if unrolling can help.
std::size_t hash(uint8_t* offsets, const uint8_t** values, int* lengths, int num_fields)
{
    std::size_t result = 0;
    for (auto i = 0; i != num_fields; ++i)
    {
        auto field_byte = values[i] + offsets[i];
        auto field_length = lengths[i];

        for (auto i = 0; i != field_length; ++i)
        {
            boost::hash_combine(result, field_byte[i]);
        }
    }
    return result;
}


struct Filter
{
    void add(void* value, int offset, int length)
    {
        mItems.emplace_back(value, offset, length);
    }

    bool match(const uint8_t* bytes, int len)
    {
        for (auto& item : mItems)
        {
            if (!item.match(bytes, len))
            {
                return false;
            }
        }
        return true;
    }


private:
    struct Item
    {
        Item(void* value, int offset, int length) : value(value), offset(offset), length(length) {}

        bool match(const uint8_t*  frame_bytes, int frame_size)
        {
            return length + offset < frame_size && !memcmp(frame_bytes + offset, value, frame_size);
        }

        void* value;
        int offset;
        int length;
    };

    std::vector<Item> mItems;
};

