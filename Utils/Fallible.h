#ifndef FALLIBLE_H_INCLUDED
#define FALLIBLE_H_INCLUDED


namespace Utils
{

    template<class T>
    class Fallible
    {
    public:
        Fallible() :
          mValid(false)
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


        // This method allows setting a default value without making the
        // object valid. The motivation behind this that in case a value
        // was queried from a object in fault state, that it would then
        // be better to return a default value than an uninitialized one.
        //
        // Fallible number<int>(0);
        // number.setInvalid();
        // int n = number; // 0 returned instead of garbage
        //
        // I prefer above approach to raising exceptions.
        //
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


        const T & or(const T & inDefault) const
        {
            if (mValid)
            {
                return mValue;
            }
            return inDefault;
        }

    private:
        bool mValid;
        T mValue;
    };

} // namespace Utils


#endif // FALLIBLE_H_INCLUDED
