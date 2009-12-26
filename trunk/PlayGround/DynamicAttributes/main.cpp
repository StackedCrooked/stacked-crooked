#include <boost/lexical_cast.hpp>
#include <map>
#include <string>

class Attribute
{
public:
    Attribute(const std::string & inName) : mName(inName) {}

    virtual ~Attribute(){}

    virtual void getStringValue(std::string & outValue) = 0;

    virtual void setStringValue(const std::string & inValue) = 0;

    const std::string & name() const { return mName; }

private:
    std::string mName;
};


template<class ValueT, class RefT>
class TypedAttribute : public Attribute
{
public:
    typedef RefT RefType;
    typedef ValueT ValueType;

    TypedAttribute(const std::string & inName, RefType inValue) :
        Attribute(inName),
        mValue(inValue)
    {
    }

    RefType get() const
    {
        return mValue;
    }

    void set(RefType inValue)
    {
        mValue = inValue;
    }

    virtual void getStringValue(std::string & outValue)
    {
        outValue = boost::lexical_cast<std::string>(get());
    }

    virtual void setStringValue(const std::string & inValue)
    {
        set(boost::lexical_cast<ValueType>(inValue));
    }

private:
    ValueType mValue;
};

#define DECLARE_ATTRIBUTE(classname, valuetype, reftype, attributename, defaultvalue)             \
    class classname : public TypedAttribute<valuetype, reftype>                                   \
    {                                                                                             \
    public:                                                                                       \
        typedef TypedAttribute<valuetype, reftype> Super;                                         \
        classname(RefType inValue) :                                                              \
            Super(Name(), inValue)                                                                \
        {                                                                                         \
        }                                                                                         \
                                                                                                  \
        static const char * Name() { return attributename; }                                      \
                                                                                                  \
        static RefType DefaultValue()                                                             \
        {                                                                                         \
            static valuetype fDefaultValue(defaultvalue);                                         \
            return fDefaultValue;                                                                 \
        }                                                                                         \
    };

#define DECLARE_INT_ATTRIBUTE(classname, attributename, defaultvalue) \
    DECLARE_ATTRIBUTE(classname, int, int, attributename, defaultvalue)

#define DECLARE_BOOL_ATTRIBUTE(classname, attributename, defaultvalue) \
    DECLARE_ATTRIBUTE(classname, bool, bool, attributename, defaultvalue)

#define DECLARE_STRING_ATTRIBUTE(classname, attributename, defaultvalue) \
    DECLARE_ATTRIBUTE(classname, std::string, const std::string &, attributename, defaultvalue)

DECLARE_INT_ATTRIBUTE(Width, "width", 0)
DECLARE_INT_ATTRIBUTE(Height, "width", 0)
DECLARE_BOOL_ATTRIBUTE(Hidden, "hidden", 0)
DECLARE_STRING_ATTRIBUTE(Title, "title", "")

class Point
{
public:
    Point() : mX(0), mY(0) {}
    Point(int x, int y) : mX(x), mY(y){}
    int x() const { return mX; }
    int y() const { return mY; }
private:
    int mX, mY;
};

std::istream& operator>>(std::istream& str, Point & outPoint)
{
    std::string text;
    std::getline(str, text);
    int x(0), y(0);
    sscanf(text.c_str(), "(%d, %d)", &x, &y);
    outPoint = Point(x, y);
    return str;
}

std::ostream& operator<<(std::ostream& str, const Point & inPoint)
{
    str << "(" << inPoint.x() << ", " << inPoint.y() << ")";
    return str;
}

DECLARE_ATTRIBUTE(Location, Point, const Point &,  "location", Point(0,0))


class AttributeContainer
{
public:
    template<class T>
    void registerAttribute()
    {
        if (mAttributes.find(T::Name()) == mAttributes.end())
        {
            mAttributes.insert(std::make_pair(T::Name(), new T(T::DefaultValue())));
        }
    }

    template<class T>
    bool hasAttribute() const
    {
        return mAttributes.find(T::Name()) != mAttributes.end();
    }

    template<class T>
    const T & attribute() const
    {
        Attributes::const_iterator it = mAttributes.find(T::Name());
        if (it == mAttributes.end())
        {
            throw std::runtime_error("No attribute found with this name.");
        }
        T * attr = dynamic_cast<T*>(it->second);
        if (!attr)
        {
            throw std::runtime_error("Dynamic cast failed.");
        }
        return *attr;
    }

    template<class T>
    T & attribute()
    {
        return const_cast<T &>(static_cast<const Component*>(this)->attribute<T>());
    }


    template<class T>
    typename T::RefType get() const
    {
        const T & attr = attribute<T>();
        return attr.get();
    }

    template<class T>
    void set(typename T::RefType inValue)
    {
        T & attr = attribute<T>();
        attr.set(inValue);
    }

private:
    typedef std::map<std::string, Attribute*> Attributes;
    Attributes mAttributes;
};


class Component : public AttributeContainer
{
};


int main()
{   
    Component comp;
    comp.registerAttribute<Title>();
    comp.registerAttribute<Width>();
    comp.registerAttribute<Location>();

    int w = 0;
    if (comp.hasAttribute<Width>())
    {
        w = comp.get<Width>();
    }
    comp.set<Width>(80);

    const std::string & title = comp.get<Title>();
    comp.set<Title>("test");

    std::string widthAsString;
    Width & width = comp.attribute<Width>();
    width.getStringValue(widthAsString);

    comp.set<Location>(Point(12, 13));    
    comp.attribute<Location>().setStringValue("(14, 15)");
    std::string p;
    comp.attribute<Location>().getStringValue(p);
    return 0;
}


/**
                            Attribute Controllers          Dynamic Attributes
                            ---------------------          ------------------
Example usage:              comp->getTitle();              comp->get<Title>();
                            comp->setWidth(80);            comp->set<Width>(80);
Attributes are defined:     per class                      per object
Attributes are added:       at compile-time                at run-time
Defining new attributes:    intrusive                      non-intrusive
Attribute lookup:           string - AC mapping            string - DA mapping

Dynamic attributes allow to create a small core API that can be extended.
The attributes can be defined using C macros.

**/
