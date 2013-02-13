#include <boost/algorithm/string/replace.hpp>


struct Transcoder
{
    Transcoder(const std::string & sep, const std::string & esc) :
        sep(sep),
        esc(esc)
    {
    }

    std::string sep, esc;
};


struct Escape : Transcoder
{
    Escape(const std::string & sep, const std::string & esc) : Transcoder(sep, esc) {}

    std::string operator()(std::string s) const
    {
        using boost::algorithm::replace_all;
        replace_all(s, esc, esc + esc);
        replace_all(s, sep, esc + sep);
        return s;
    }
};


struct Unescape : Transcoder
{
    Unescape(const std::string & sep, const std::string & esc) : Transcoder(sep, esc) {}

    std::string operator()(std::string s) const
    {
        using boost::algorithm::replace_all;
        replace_all(s, esc + sep, sep);
        replace_all(s, esc + esc, esc);
        return s;
    }
};


struct Join
{
    Join(const std::string & sep, const std::string & esc) : sep(sep), esc(esc) {}

    std::string operator()(const std::string & left, const std::string & right) const
    {
        Escape escape(sep, esc);
        return escape(left) + esc + escape(right);
    }

    std::string sep, esc;
};


struct Encoder
{
    Encoder(const std::string & inDelimiter, const std::string & inEscape, const std::string & inMetaEscape) :
        delim(inDelimiter),
        esc(inEscape),
        mMetaEscape(inMetaEscape)
    {
    }

    Encoder(const std::string & inDelimiter, const std::string & inEscape) :
        delim(inDelimiter),
        esc(inEscape),
        mMetaEscape(inEscape + inEscape)
    {
    }

    void encode(std::string & text)
    {
        using boost::algorithm::replace_all;
        replace_all(text, esc,    mMetaEscape);
        replace_all(text, delim, esc + delim);
    }

    void decode(std::string & text)
    {
        using boost::algorithm::replace_all;
        replace_all(text, esc + delim, delim);
        replace_all(text, mMetaEscape, esc);
    }

    std::string delim, esc, mMetaEscape;
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
