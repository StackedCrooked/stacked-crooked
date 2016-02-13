#pragma once


#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))


#include <boost/functional/hash.hpp>
#include <boost/strong_typedef.hpp>
#include <cstdint>
#include <iostream>
#include <vector>


struct Filter
{
    void add(uint8_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
        recalculate_hash();
    }

    void add(uint16_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
        recalculate_hash();
    }

    void add(uint32_t value, int offset)
    {
        mItems.push_back(Item(value, offset));
        recalculate_hash();
    }

    std::size_t getHash() const
    {
        return mHash;
    }

    void recalculate_hash()
    {
        std::size_t result = 0;
        for (const Item& item : mItems)
        {
            boost::hash_combine(result, item.calculate_hash());
        }
        mHash = result;
    }

    bool match(const uint8_t* bytes, int len) const
    {
        for (const Item& item : mItems)
        {
            auto i = &item - mItems.data(); (void)i;
            if (!item.match(bytes, len))
            {
                //std::cout << "Filter[" << i << "] failed" << std::endl;
                return false;
            }
        }
        //std::cout << "Filter::match: " << mItems.size() << " all matched!" << std::endl;
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
            auto field_length = field_length_;
            auto field_offset = field_offset_;

            //std::cout << "Match with offset=" << field_offset_ << " field_length=" << field_length_ << std::endl;
            if (UNLIKELY(field_length + field_offset > size))
            {
                //std::cout << "field_length_(" << field_length_ << ") + field_offset_(" << field_offset_ << ") > size(" << size << ")" << std::endl;
               return false;
            }


            auto input = frame + field_offset;

            #if 0
            return !memcmp(input, &storage_, field_length_);
            #else
            switch (field_length)
            {
                case 1:
                {
                    return get_field_8() == *input;
                }
                case 2:
                {
                    return get_field_16() == *(uint16_t*)input;
                }
                case 4:
                {
                    //auto u16 = (uint16_t*)input; return get_field_16(1) == u16[1] && get_field_16(0) == u16[0];
                    return get_field_32() == *(uint32_t*)input;
                }
            }
            #endif
            __builtin_unreachable();
            //return !memcmp(input, &storage_, field_length_);
        }

        std::size_t calculate_hash() const
        {
            std::size_t result = 0;
            const uint8_t* field_bytes = get_field();

            for (auto i = 0u; i != field_length_; ++i)
            {
                boost::hash_combine(result, field_bytes[i]);
            }

            return result;
        }

        const uint8_t* get_field() const { return static_cast<const uint8_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_8() const { return *static_cast<const uint8_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_16() const { return *static_cast<const uint16_t*>(static_cast<const void*>(&storage_)); }
        uint16_t get_field_16(std::size_t i) const { return static_cast<const uint16_t*>(static_cast<const void*>(&storage_))[i]; }
        uint32_t get_field_32() const { return storage_; }

        uint32_t storage_;
        uint16_t field_offset_;
        uint16_t field_length_;
        //std::size_t hash_ = 0;
    };

    std::size_t mHash = 0;
    std::vector<Item> mItems;
};

