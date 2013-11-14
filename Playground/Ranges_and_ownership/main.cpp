#include <boost/variant.hpp>
#include <string>
#include <stdint.h>
#include <string.h>


namespace Detail {
    
    
struct NonOwningRange
{
    NonOwningRange(const char* b, const char* e) : b(b), e(e) {}
    NonOwningRange(char* b, unsigned size) : b(b), e(b + size) {}
    
    unsigned size() const { return e - b; }
    
    const char* b;
    const char* e;
};


struct OwningRange : NonOwningRange
{  
    OwningRange(const char* b, unsigned length) : NonOwningRange(static_cast<char*>(malloc(length)), length)
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



struct Range
{
    struct Refer {};
    struct Copy {};
    
    Range(const char* b, const char* e, Refer) : impl(NonOwningRange(b, e)) {}    
    Range(const char* b, const char* e, Copy) : impl(OwningRange(b, e)) {}
    

    const char* begin() const
    {
        //boost::apply_visitor([](NonOwningRange& r) { return r.b; }, impl);
        visit([](NonOwningRange& r) { return r.b; });
    }
    
    const char* end() const
    {
        boost::apply_visitor([](NonOwningRange& r) { return r.e; }, impl);
    }
    
    unsigned long size() const
    {
        boost::apply_visitor([](NonOwningRange& r) { return r.size(); }, impl);
    }
    
private:
    typedef boost::variant<OwningRange, NonOwningRange> Impl;
    
    template<typename F>
    struct Visitor
    {
        Visitor(F f) : f(f) {}
        
        void operator()(OwningRange& r) const { NonOwningRange& nor = r; f(nor); }
        void operator()(NonOwningRange& r) const {  f(r); }
        
        F f;        
    };
    
    template<typename F>
    static void make_visitor(F f) { return Visitor<F>(f); }
    
    template<typename F>
    void visit(F f) const { boost::apply_visitor(make_visitor(f), impl); }
    
    Impl  impl;
};


//
//
//
//
//struct Segment
//{    
//    struct Copy {};
//    struct Refer {};
//    
//    Segment(ByteRange range, Copy) : data(range) {}
//    Segment(ByteRange range, Refer) : data(std::string(range.b, range.e)) {}
//    
//    Segment(std::string str) : data(str) {}
//    
//    const char* begin() const
//    {        
//        if (auto range = data.get<ByteRange*>())
//        {
//            return range.b; 
//        }
//        else
//        {
//            return data.get<std::string*>()->data();
//        }
//    }
//    
//    const char* end() const
//    {      
//        if (auto range = data.get<ByteRange*>())
//        {
//            return range.e; 
//        }
//        else
//        {
//            std::string* s = data.get<std::string*>();
//            return s->data() + s->size();
//        }
//    }
//    
//    unsigned size() const
//    {
//        if (auto range = data.get<ByteRange*>())
//        {
//            return range.size(); 
//        }
//        else
//        {
//            std::string* s = data.get<std::string*>();
//            return s->size();
//        }
//    }
//    
//private:
//    typedef boost::variant<ByteRange, std::string> Data;
//    Data data;
//};

