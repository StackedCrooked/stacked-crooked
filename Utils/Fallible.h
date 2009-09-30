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


        // This method allows having a default value:
        //
        // Fallible number<int>(0);
        // number.setInvalid();
        // int n = number; // 0 returned
        //
        // I prefer above approach to raising an exceptions.
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


        const T & getValue(const T & inDefault) const
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
