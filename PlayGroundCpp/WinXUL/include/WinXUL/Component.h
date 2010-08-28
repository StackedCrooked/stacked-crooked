#ifndef WINXUL_COMPONENT_H_INCLUDED
#define WINXUL_COMPONENT_H_INCLUDED


namespace WinXUL
{

    /**
     * Component is the pure abstract base class for Decorator and ConcreateComponent classes.
     */
    class Component
    {
    public:
        Component(){}

        virtual ~Component() = 0 {}

        virtual Component * getParent() = 0;

        virtual const Component * getParent() const = 0;

        // Should use external properties. Avoid adding hundreds of methods.
        virtual int getWidth() const = 0;

        virtual int getHeight() const = 0;

    private:
        Component(const Component &);
        Component & operator= (const Component &);
    };


} // namespace WinXUL


#endif // WINXUL_COMPONENT_H_INCLUDED
