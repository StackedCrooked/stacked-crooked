#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <sstream>
#include <string>
#include <windows.h>


namespace XULWin
{

    class Attribute
    {
    public:
        Attribute(const std::string & inName) : mName(inName) {}

        virtual ~Attribute() {}

        //virtual bool getStringValue(std::string & outValue) = 0;

        //virtual bool setStringValue(const std::string & inValue) = 0;

        const std::string & name() const { return mName; }

    private:
        std::string mName;
    };


    template<class ValueT, class ConstRefT>
    class TypedAttribute : public Attribute
    {
    public:
        typedef ConstRefT ConstRefType;
        typedef ValueT ValueType;
        
        // We have to return a value type for the getter because we don't know how the variable
        // will be stored and retrieved.
        typedef boost::function<bool(ValueType&)> Getter;
        typedef boost::function<bool(ConstRefType)> Setter;

        TypedAttribute(const std::string & inName, Getter inGetter, Setter inSetter) :
            Attribute(inName),
            mGetter(inGetter),
            mSetter(inSetter)
        {
        }

        bool getValue(ValueType & outValue) const
        {
            return mGetter(outValue);
        }

        bool setValue(ConstRefType inValue)
        {
            return mSetter(inValue);
        }

        //virtual bool getStringValue(std::string & outValue)
        //{
        //    ValueType value;
        //    if (getValue(value))
        //    {
        //        outValue = boost::lexical_cast<std::string>(value);
        //        return true;
        //    }
        //    return false;
        //}

        //virtual bool setStringValue(const std::string & inValue)
        //{            
        //    ValueType value = boost::lexical_cast<ValueType>(inValue);
        //    return setValue(value);
        //}

    private:
        Getter mGetter;
        Setter mSetter;
    };


    class AttributeContainer
    {
    public:
        template<class T>
        void registerAttribute(typename T::Getter inGetter,
                               typename T::Setter inSetter)
        {
            if (mAttributes.find(T::Name()) == mAttributes.end())
            {
                mAttributes.insert(std::make_pair(T::Name(), new T(inGetter, inSetter)));
                return;
            }
            std::stringstream ss;
            ss << "An attribute with name '" << T::Name() << "' already exists.";
            std::string msg = ss.str();
            throw std::runtime_error(msg.c_str());
        }


        template<class T>
        const T & attribute() const
        {
            Attributes::const_iterator it = mAttributes.find(T::Name());
            if (it == mAttributes.end())
            {
                std::stringstream ss;
                ss << "No attribute found with name '" << T::Name() << "'.";
                std::string msg = ss.str();
                throw std::runtime_error(msg.c_str());
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
            return const_cast<T &>(static_cast<const AttributeContainer*>(this)->attribute<T>());
        }


        bool getAttribute(const std::string & inName, Attribute *& outAttribute) const
        {
            Attributes::const_iterator it = mAttributes.find(inName);
            if (it != mAttributes.end())
            {
                outAttribute = it->second;
                return true;
            }
            return false;
        }


        template<class T>
        typename T::ValueType get() const
        {
            const T & attr = attribute<T>();
            T::ValueType result;
            if (!attr.getValue(result))
            {
                std::stringstream ss;
                ss << "No attribute found with name '" << T::Name() << "'.";
                std::string msg = ss.str();
                throw std::runtime_error(msg.c_str());
            }
            return result;
        }


        template<class T>
        void set(typename T::ConstRefType inValue)
        {
            T & attr = attribute<T>();
            if (!attr.setValue(inValue))
            {
                throw std::runtime_error("Failed to set attribute value.");
            }
        }


    private:
        typedef std::map<std::string, Attribute*> Attributes;
        Attributes mAttributes;
    };

    
    //**
    // * DECLARE_ATTRIBUTE
    // * 
    // * This macro declares a XUL attribute.
    // */
    #define DECLARE_ATTRIBUTE(classname, valuetype, reftype, attributename)                           \
        class classname : public TypedAttribute<valuetype, reftype>                                   \
        {                                                                                             \
        public:                                                                                       \
            typedef TypedAttribute<valuetype, reftype> Super;                                         \
            classname(Super::Getter inGetter, Super::Setter inSetter) :                               \
                Super(Name(), inGetter, inSetter)                                                     \
            {                                                                                         \
            }                                                                                         \
                                                                                                      \
            static const char * Name() { return attributename; }                                      \
                                                                                                      \
        };

    //**
    // * DECLARE_INT_ATTRIBUTE
    // *
    // * Declares a XUL attribute that has an int value.
    // */
    #define DECLARE_INT_ATTRIBUTE(classname, attributename) \
        DECLARE_ATTRIBUTE(classname, int, int, attributename)

    //**
    // * DECLARE_BOOL_ATTRIBUTE
    // *
    // * Declares a XUL attribute that has a boolean value.
    // */
    #define DECLARE_BOOL_ATTRIBUTE(classname, attributename) \
        DECLARE_ATTRIBUTE(classname, bool, bool, attributename)

    //**
    // * DECLARE_STRING_ATTRIBUTE
    // *
    // * Declares a XUL attribute that has a std::string value.
    // */
    #define DECLARE_STRING_ATTRIBUTE(classname, attributename) \
        DECLARE_ATTRIBUTE(classname, std::string, const std::string &, attributename)

    //**
    // * DECLARE_FUNCTOR_ATTRIBUTE
    // *
    // */
    #define DECLARE_FUNCTOR_ATTRIBUTE(classname, attributename) \
        DECLARE_ATTRIBUTE(classname, boost::function<void()>, const boost::function<void()> &, attributename)

    //**
    // * DECLARE_HWND_ATTRIBUTE
    // *
    // */
    #define DECLARE_HWND_ATTRIBUTE(classname, attributename) \
        DECLARE_ATTRIBUTE(classname, HWND, HWND, attributename)

    
    DECLARE_INT_ATTRIBUTE(Width, "width")
    DECLARE_INT_ATTRIBUTE(Height, "height")
    DECLARE_BOOL_ATTRIBUTE(Hidden, "hidden")
    DECLARE_STRING_ATTRIBUTE(Title, "title")
    DECLARE_FUNCTOR_ATTRIBUTE(Factory, "Component_Constructor");
    DECLARE_FUNCTOR_ATTRIBUTE(Destructor, "Component_Destructor");
    DECLARE_FUNCTOR_ATTRIBUTE(Initializer, "Component_Initializer");
    DECLARE_HWND_ATTRIBUTE(Handle, "hwnd");



    template<class State>
    class Component : public AttributeContainer
    {
    public:
        static std::auto_ptr<Component> Create()
        {
            std::auto_ptr<Component> comp(new Component);
            return comp;
        }

        Component()
        {
        }

        ~Component()
        {
            get<Destructor>()();
        }

        virtual void create()
        {
            get<Factory>()();
        }

        virtual void init()
        {
            get<Initializer>()();
        }

        State state;
    };


}


using namespace XULWin;

struct WinComponentState
{
    HWND hwnd;
};

typedef Component<WinComponentState> WinComponent;
DECLARE_ATTRIBUTE(Parent, WinComponent*, const WinComponent *, "parent")


typedef boost::function<void()> VoidFunction;

bool getFactory(WinComponent * inComponent, VoidFunction & outFactory)
{
    struct Helper
    {
        static void CreateAWindow()
        {
            
        }
    };
    outFactory = boost::bind(&Helper::CreateAWindow);
    return true;
}


bool setFactory(WinComponent * inComponent, const VoidFunction & inFactory)
{
    return true;
}


bool getInitializer(WinComponent * inComponent, VoidFunction & outInitializer)
{
    struct Helper
    {
        static void CreateAWindow()
        {
            
        }
    };
    outInitializer = boost::bind(&Helper::CreateAWindow);
    return true;
}


bool setInitializer(WinComponent * inComponent, const VoidFunction & inInitializer)
{
    return true;
}


bool getDestructor(WinComponent * inComponent, VoidFunction & outDestructor)
{
    struct Helper
    {
        static void CreateAWindow()
        {
            
        }
    };
    outDestructor = boost::bind(&Helper::CreateAWindow);
    return true;
}


bool setDestructor(WinComponent * inComponent, const VoidFunction & inDestructor)
{
    return true;
}


bool getTitle(WinComponent * inComponent, std::string & outTitle)
{
    TCHAR buffer[512];
    ::GetWindowText(inComponent->state.hwnd, &buffer[0], sizeof(buffer)/sizeof(TCHAR));
    return true;
}

bool setTitle(WinComponent * inComponent, const std::string & inTitle)
{
    ::SetWindowText(inComponent->state.hwnd, L"");
    return true;
}


bool getWidth(WinComponent * inComponent, int & outWidth)
{
    HWND hwnd = inComponent->get<Handle>();
    RECT rw;
    ::GetWindowRect(hwnd, &rw);
    outWidth = rw.right - rw.left;
    return true;
}


bool setWidth(WinComponent * inComponent, int inWidth)
{
    HWND hwnd = inComponent->get<Handle>();
    RECT rw;
    ::GetWindowRect(hwnd, &rw);
    ::MoveWindow(hwnd, rw.left, rw.top, rw.left + inWidth, rw.bottom - rw.top, FALSE);
    return true;
}


bool getHeight(WinComponent * inComponent, int & outHeight)
{
    HWND hwnd = inComponent->get<Handle>();
    RECT rw;
    ::GetWindowRect(hwnd, &rw);
    outHeight = rw.bottom - rw.top;
    return true;
}


bool setHeight(WinComponent * inComponent, int inHeight)
{
    HWND hwnd = inComponent->get<Handle>();
    RECT rw;
    ::GetWindowRect(hwnd, &rw);
    ::MoveWindow(hwnd, rw.left, rw.top + inHeight, rw.right - rw.left, rw.bottom - rw.top, FALSE);
    return true;
}


bool getHandle(WinComponent * inComponent, HWND & outHandle)
{
    inComponent->state.hwnd;
    return true;
}


bool setHandle(WinComponent * inComponent, HWND inHandle)
{
    inComponent->state.hwnd = inHandle;
    return true;
}



bool getParent(WinComponent * inComponent, WinComponent *& outParent)
{
    return true;
}


bool setParent(WinComponent * inComponent, const WinComponent * inParent)
{
    return true;
}

#define REGISTER_ATTRIBUTE(attribute, object) \
    object->registerAttribute<attribute>(boost::bind(&get##attribute, object, _1), \
                                         boost::bind(&set##attribute, object, _1));

class ComponentFactory
{
public:
    std::auto_ptr<WinComponent> makeComponent(WinComponent * inParent)
    {
        std::auto_ptr<WinComponent> comp(new WinComponent);
        REGISTER_ATTRIBUTE(Factory, comp.get())
        REGISTER_ATTRIBUTE(Initializer, comp.get())
        REGISTER_ATTRIBUTE(Destructor, comp.get())
        REGISTER_ATTRIBUTE(Title, comp.get())
        REGISTER_ATTRIBUTE(Width, comp.get())
        REGISTER_ATTRIBUTE(Height, comp.get())
        REGISTER_ATTRIBUTE(Handle, comp.get())
        REGISTER_ATTRIBUTE(Parent, comp.get())
        return comp;
    }
};

int main()
{   
    ComponentFactory fact;
    std::auto_ptr<WinComponent> comp(fact.makeComponent(0));
    comp->create();
    comp->init();

    int w = 0;
    comp->get<Title>();
    comp->set<Width>(80);

    const std::string & title = comp->get<Title>();
    comp->set<Title>("test");

    std::string widthAsString;
    Width & width = comp->attribute<Width>();
    return 0;
}
