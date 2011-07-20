

struct Expression
{
};


struct Identity : public Expression
{
};

struct And : public Expression
{
};

struct Or : public Expression
{
};

struct Not : public Expression
{
};

struct Primitive
{
};

struct Qualifier
{
};


// type: host*, net , port and portrange
// direction: "src", "dst", "src or dst"*, "src and dst"
// protocol: ether, fddi, tr, wlan, ip, ip6, arp, rarp, decnet, tcp and udp


int main()
{
    return 0;
}
