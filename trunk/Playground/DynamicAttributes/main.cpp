#include "Attribute.h"
#include "AttributeContainer.h"
#include "CommonAttributes.h"
#include "TypedAttribute.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <sstream>
#include <string>
#include <windows.h>


namespace XULWin
{

    //**
    // * DECLARE_HWND_ATTRIBUTE
    // *
    // */
    #define DECLARE_HWND_ATTRIBUTE(classname, attributename) \
        DECLARE_ATTRIBUTE(classname, HWND, HWND, attributename)

    
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


#define ADD_ATTRIBUTE(AttributeType, obj) \
    obj->addAttribute<AttributeType>(boost::bind(&get##AttributeType, obj, _1), \
                                     boost::bind(&set##AttributeType, obj, _1));


class ComponentFactory
{
public:
    std::auto_ptr<WinComponent> makeComponent(WinComponent * inParent)
    {
        std::auto_ptr<WinComponent> comp(new WinComponent);
        ADD_ATTRIBUTE(Factory, comp.get())
        ADD_ATTRIBUTE(Initializer, comp.get())
        ADD_ATTRIBUTE(Destructor, comp.get())
        ADD_ATTRIBUTE(Title, comp.get())
        ADD_ATTRIBUTE(Width, comp.get())
        ADD_ATTRIBUTE(Height, comp.get())
        ADD_ATTRIBUTE(Handle, comp.get())
        ADD_ATTRIBUTE(Parent, comp.get())
        return comp;
    }
};


int main()
{   
    ComponentFactory fact;
    std::auto_ptr<WinComponent> comp(fact.makeComponent(0));
    comp->create();
    comp->init();

	std::string title = comp->get<Title>();
	assert(title.empty());
	comp->set<Title>("test");
	title = comp->get<Title>();
	assert(title == "test");

    comp->set<Width>(80);
	int w = comp->get<Width>();

    Width & width = comp->getAttribute<Width>();
	width.getValue(w);
    return 0;
}
