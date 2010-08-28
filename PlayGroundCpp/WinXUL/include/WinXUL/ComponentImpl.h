#ifndef WINXUL_COMPONENTIMPL_H_INCLUDED
#define WINXUL_COMPONENTIMPL_H_INCLUDED


namespace WinXUL
{

    /**
     * Base class for NativeComponent and NonNativeComponent.
     */
    class ComponentImpl
    {
    public:
        ComponentImpl() {}

        virtual ~ComponentImpl() = 0 {}

    private:
        ComponentImpl(const ComponentImpl&);
        ComponentImpl & operator=(ComponentImpl &);
    };


} // namespace WinXUL


#endif // WINXUL_COMPONENTIMPL_H_INCLUDED
