#include <iostream>

struct Base {
    Base(int n) : n(n) {}

    virtual ~Base() = 0;

    int n;
};

Base::~Base() {}

struct A : public virtual Base {
    A(int n) : Base(n) {}

    virtual ~A() = 0;
};

A::~A() {}

struct B : public virtual Base {
    B(int n) : Base(n) {}

    virtual ~B() = 0;
};

B::~B() {}

struct Test : public virtual A, public virtual B {
    Test(int n) : Base(n), A(n), B(n) {}
};

int main() {
    Test c(0);
    (void)c;
}
