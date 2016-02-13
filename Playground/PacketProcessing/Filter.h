

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
