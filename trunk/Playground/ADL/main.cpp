#include <string>

struct Protocol
{
    typedef void(*PopFunction)(void*, const std::string&);

    Protocol(PopFunction inPopFunction) :
        mPopFunction(inPopFunction)
    {

    }

    void pop(const std::string& str)
    {
        mPopFunction(*this, str);
    }

    typedef void(*PopFunction)(void*, const std::string&);
};


template<typename CLP>
struct BasicProtocol : Protocol
{
    BasicProtocol() : Protocol(&BasicProtocol<CLP>::static_pop) {}

    void pop(const std::string& s)
    {
        DecoderType dec(s);
        static_cast<CLP&>(*this).do_pop(dec);
    }

private:

    static void static_pop(Protocol& p, const std::string& s)
    {
        static_cast<this_type&>(p).pop(s):
    }
};


template<typename CLP, typename CLS, typename SessionKey>
struct BasicSessionManager
{
    template<typename DecoderType>
    void do_pop(const DecoderType& dec)
    {
        this->sessionPop(dec);
    }

    template<typename DecoderType>
    void sessionPop(const DecoderType& dec)
    {
        auto key = GetSessionKey(dec);

        auto it = mSessions.find(key);
        if (it != mSessions.end())
        {
            it->second->pop(GetPayload(dec));
            return;
        }

        auto cls = static_cast<CLP&>(*this).createSession(dec);
        mSessions.insert(std::make_pair(key, cls));
        cls->pop(GetPayload(dec));
    }

private:
    std::map<SessionKey, std::shared_ptr<CLS>> mSessions;
};


struct IPv4Protocol : BasicProtocol<IPv4Protocol>, BasicSessionManager<IPv4Protocol>
{
};


int main()
{
    IPv4Protocol theIPv4Protocol;
}
