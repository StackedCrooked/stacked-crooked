#ifndef WINXUL_DECORATOR_H_INCLUDED
#define WINXUL_DECORATOR_H_INCLUDED


#include "WinXUL/Component.h"
#include <boost/scoped_ptr.hpp>
#include <memory>


namespace WinXUL
{

    class Decorator : public Component
    {
    public:
        Decorator(std::auto_ptr<Component> inDecoratedComponent);

        virtual ~Decorator();

        Component * getDecoratedComponent();

        const Component * getDecoratedComponent() const;

        virtual int getWidth() const;

        virtual int getHeight() const;

    private:
        boost::scoped_ptr<Component> mDecoratedComponent;
    };

} // namespace WinXUL


#endif // WINXUL_DECORATOR_H_INCLUDED
