#include <iostream>


struct Base
{
    int pop() const
    {
        return ((FPPop)(vtbl[tag][0]))(*this);
    }
    
    void push(int x)
    {
        return ((FPPush)vtbl[tag][1])(*this, x);
    }
    
protected:
    typedef void (*FP)();
    typedef int(*FPPop)(const Base&);
    typedef void(*FPPush)(Base&, int);
    
    Base(uint8_t n, FPPop getter, FPPush setter) : tag(n)
    {
        vtbl[tag][0] = (FP)getter;
        vtbl[tag][1] = (FP)setter;
    }
    
private:    
    enum
    {
        totalClasses = 2,
        totalMethods = 2
    };
    static FP vtbl[totalClasses][totalMethods];
    uint8_t tag;
};


Base::FP Base::vtbl[Base::totalClasses][Base::totalMethods];


template<typename Subtype, unsigned Index>
struct Object : Base
{
    Object() : Base(Index, &Self::static_pop, &Self::static_push)
    {
    }    
    
    typedef Object<Subtype, Index> Self;
    
    static int static_pop(const Base& b)
    {
        return static_cast<const Subtype&>(b).do_pop();
    }
    
    static void static_push(Base& b, int n)
    {
        static_cast<Subtype&>(b).do_push(n);
    }
};


struct SendCounter : Object<SendCounter, 0>
{
    SendCounter() : n_() {}
    
    void do_push(int n)
    {
        n_ = n;
    }
    
    int do_pop() const
    {
        return n_;
    }
    
    int n_;
};


struct ReceiveCounter : Object<ReceiveCounter, 0>
{
    ReceiveCounter() : n_() {}
    
    void do_push(int n)
    {
        n_ = n;
    }
    
    int do_pop() const
    {
        return n_;
    }
    
    int n_;
};


int main()
{
    SendCounter t;    
    t.push(2);
    std::cout << t.pop() << std::endl;
    
    ReceiveCounter r;    
    r.push(4);
    std::cout << r.pop() << std::endl;
}


