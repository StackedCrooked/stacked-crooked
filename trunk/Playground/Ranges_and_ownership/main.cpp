#include <boost/variant.hpp>
#include <string>
#include <stdint.h>
#include <string.h>


namespace Detail {
    
    
struct NonOwningRange
{
    NonOwningRange(const char* b, const char* e) : b(b), e(e) {}
    NonOwningRange(char* b, std::size_t size) : b(b), e(b + size) {}
    
    std::size_t size() const { return e - b; }

    
    const char* b;
    const char* e;
};


struct OwningRange : NonOwningRange
{  
    OwningRange(const char* b, std::size_t length) : NonOwningRange(static_cast<char*>(malloc(length)), length)
    {
        memcpy(const_cast<char*>(this->b), b, size());
    }
    
    OwningRange(const char* b, const char* e) : OwningRange(b, e - b) { }
    
    ~OwningRange()
    {
        free(const_cast<char*>(b));
        b = e = nullptr;
    }
    
    OwningRange(const OwningRange& rhs) : OwningRange(rhs.b, rhs.e) {}
    
    OwningRange& operator=(OwningRange rhs)
    {
        std::swap(b, rhs.b);
        std::swap(e, rhs.e);
        return *this;
    }
    
    OwningRange(OwningRange&& rhs) : NonOwningRange(rhs.b, rhs.e)
    {
        rhs.b = rhs.e = nullptr;
    }
    
    OwningRange& operator==(OwningRange&& rhs)
    {
        b = rhs.b;
        e = rhs.e;
        rhs.b = rhs.e = nullptr;
        return *this;
    }
};


} // namespace Detail



struct Range
{
    struct Refer{};
    struct Copy{};
    
    Range(const char* b, const char* e, Refer) : impl(NonOwningRange(b, e)) {}    
    Range(const char* b, const char* e, Copy) : impl(OwningRange(b, e)) {}
    

    const char* data() const
    {
        return obj().b;
    }

    const char* begin() const
    {
        return data();
    }
    
    const char* end() const
    {
        return obj().e;
    }
    
    std::size_t size() const
    {
        return obj().size();
    }

    
private:
    using OwningRange = Detail::OwningRange;
    using NonOwningRange = Detail::NonOwningRange;

    const NonOwningRange& obj() const
    {
        if (auto p = boost::get<NonOwningRange>(&impl)) return *p;
        if (auto p = boost::get<OwningRange>(&impl)) return *p;
        assert(!"Object is neither an OwningRange nor an NonOwningRange.");
        throw 1;
    }

    boost::variant<OwningRange, NonOwningRange> impl;
};


int main()
{
    std::string test = "abc";
    Range alias(test.data(), test.data() + test.size(), Range::Refer{});
    assert(std::string(alias.begin(), alias.end()) == test);
    assert(alias.data() == test.data());
    assert(alias.begin() == &*test.begin());
    assert(alias.end() == &*test.end());

    Range copy(test.data(), test.data() + test.size(), Range::Copy{});
    assert(std::string(copy.begin(), copy.end()) == test);
    assert(copy.data() != test.data());
    assert(copy.begin() != &*test.begin());
    assert(copy.end() != &*test.end());
    alias = copy;
    copy = alias;
}
