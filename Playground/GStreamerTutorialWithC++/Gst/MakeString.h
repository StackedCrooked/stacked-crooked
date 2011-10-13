#ifndef MAKESTRING_H_INCLUDED
#define MAKESTRING_H_INCLUDED


#include <string>
#include <sstream>


namespace Gst
{

    /**
     * Gst::MakeString is a helper class for string concatenation.
     * Usage example:
     *    void foo(std::string const &);
     *    int main()
     *    {
     *       std::string name = "David";
     *       foo(Gst::MakeString() << "Hello " << name << "!");
     *    }
     *
     * Found on StackOverflow where the class was named "make_string".
     * http: *stackoverflow.com/questions/469696/what-is-your-most-useful-c-c-snippet
     */
    class MakeString
    {
    public:
       template <typename T>
       Gst::MakeString& operator<<(const T & datum)
       {
          mBuffer << datum;
          return *this;
       }
       operator std::string () const
       {
          return mBuffer.str();
       }
    private:
       std::ostringstream mBuffer;
    };

} // namespace Gst


#endif // MAKESTRING_H_INCLUDED
