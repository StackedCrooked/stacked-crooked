#include <boost/algorithm/string/replace.hpp>


template<typename CharSeq>
struct Encoder
{
    Encoder(const CharSeq & inDelimiter, const CharSeq & inEscape, const CharSeq & inMetaEscape) :
        mDelimiter(inDelimiter),
        mEscape(inEscape),
        mMetaEscape(inMetaEscape)
    {
    }

    Encoder(const CharSeq & inDelimiter, const CharSeq & inEscape) :
        mDelimiter(inDelimiter),
        mEscape(inEscape),
        mMetaEscape(inEscape + inEscape)
    {
    }

    void encode(CharSeq & text)
    {
        using boost::algorithm::replace_all;
        replace_all(text, mEscape,    mMetaEscape);
        replace_all(text, mDelimiter, mEscape + mDelimiter);
    }

    void decode(CharSeq & text)
    {
        using boost::algorithm::replace_all;
        replace_all(text, mEscape + mDelimiter, mDelimiter);
        replace_all(text, mMetaEscape, mEscape);
    }

    CharSeq mDelimiter, mEscape, mMetaEscape;
};


typedef std::string delimiter;
typedef std::string escape;


inline void encode(std::string & s, delimiter delim, escape esc)
{
    boost::algorithm::replace_all(s, esc, esc + esc);
    boost::algorithm::replace_all(s, delim, esc + delim);
}



inline void decode(std::string & s, delimiter delim, escape esc)
{
    boost::algorithm::replace_all(s, esc + delim, delim);
    boost::algorithm::replace_all(s, esc + esc, esc);
}


inline std::string encode_copy(std::string s, delimiter delim, escape esc)
{
    encode(s, delim, esc);
    return s;
}


inline std::string decode_copy(std::string s, delimiter delim, escape esc)
{
    decode(s, delim, esc);
    return s;
}
