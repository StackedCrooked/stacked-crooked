#pragma once


#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))


#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>


struct Filter
{
    void add(uint8_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
    }

    void add(uint16_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
    }

    void add(uint32_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
    }

    bool match(const uint8_t* bytes, int len) const
    {
        for (const Item& item : mItems)
        {
            auto i = &item - mItems.data(); (void)i;
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
        Item(uint32_t value, int offset) :
            storage_(value),
            field_offset_(offset),
            field_length_(sizeof(value))
        {
        }

        Item(uint16_t value, int offset) :
            storage_(value),
            field_offset_(offset),
            field_length_(sizeof(value))
        {
        }

        Item(uint8_t value, int offset) :
            storage_(value),
            field_offset_(offset),
            field_length_(sizeof(value))
        {
        }

        bool match(const uint8_t* frame, unsigned size) const
        {
            if (UNLIKELY(field_length_ + field_offset_ > size))
            {
               return false;
            }

            auto input = frame + field_offset_;

            if (field_length_ == 4) return get_field_32() == *(uint32_t*)(input);
            if (field_length_ == 2) return get_field_16() == *(uint16_t*)(input);
            return get_field_8() == *input;
        }

        const uint8_t* get_field() const { return static_cast<const uint8_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_8() const { return *static_cast<const uint8_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_16() const { return *static_cast<const uint16_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_16(std::size_t i) const { return static_cast<const uint16_t*>(static_cast<const void*>(&storage_))[i]; }
        uint32_t get_field_32() const { return storage_; }

        uint32_t storage_;
        uint16_t field_offset_;
        uint16_t field_length_;
    };

    std::vector<Item> mItems;
};

