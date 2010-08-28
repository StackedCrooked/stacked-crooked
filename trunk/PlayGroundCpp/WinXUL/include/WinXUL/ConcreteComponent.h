#ifndef WINXUL_CONCRETECOMPONENT_H_INCLUDED
#define WINXUL_CONCRETECOMPONENT_H_INCLUDED


#include "WinXUL/Component.h"
#include <memory>
#include <boost/scoped_ptr.hpp>


namespace Poco
{
    namespace XML
    {
        class Element;
    }
}


namespace WinXUL
{

    class ComponentImpl;

    /**
     * ConcreteComponent is the base class for a GUI component.
     */
    class ConcreteComponent : public Component
    {
    public:
        virtual ~ConcreteComponent();

        virtual Component * getParent();

        virtual const Component * getParent() const;

        virtual int getWidth() const;

        virtual int getHeight() const;

    protected:
        ConcreteComponent(Component * inParent, std::auto_ptr<ComponentImpl> inImpl);

    private:
        Component * mParent;
        boost::scoped_ptr<ComponentImpl> mImpl;
        Poco::XML::Element * mElement;
    };

} // namespace WinXUL


#endif // WINXUL_CONCRETECOMPONENT_H_INCLUDED
