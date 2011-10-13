#ifndef GST_FALLIBLE_H_INCLUDED
#define GST_FALLIBLE_H_INCLUDED


namespace Gst {


template<class T>
class Fallible
{
public:
    Fallible() :
      mValid(false),
      mValue()
    {
    }

    Fallible(const T & inValue) :
        mValue(inValue),
        mValid(true)
    {
    }

    T & operator= (const T & inValue)
    {
        mValid = true;
        mValue = inValue;
        return mValue;
    }

    bool isValid() const
    {
        return mValid;
    }

    void setInvalid()
    {
        mValid = false;
    }

    operator bool() const
    {
        return mValid;
    }
    
    operator const T &() const
    {
        return mValue;
    }

    T & operator->()
    {
        return mValue;
    }

    const T & operator->() const
    {
        return mValue;
    }

    const T & getValue() const
    {
        return mValue;
    }

private:
    bool mValid;
    T mValue;
};


} // namespace Gst


#endif // GST_FALLIBLE_H_INCLUDED
