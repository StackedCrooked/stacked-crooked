#ifndef WINXUL_NATIVECOMPONENT_H_INCLUDED
#define WINXUL_NATIVECOMPONENT_H_INCLUDED


#include "ComponentImpl.h"


namespace WinXUL
{
    class Component;

    /**
     * Base class for all UI components.
     *
     * Base class of all XUL wigets. It holds a pointer to the XML object so that you can get attributes and child nodes.
     * The ComponentImpl object manages the native aspects.
     */
    class NativeComponent : public ComponentImpl
    {
    public:
        NativeComponent(NativeComponent * inNativeParent);

        virtual ~NativeComponent();

        // Finds a native parent so that we can create a child component for inComponent.
        // So inComponent itself is also a possible return value (if it is a NativeComponent).
        static NativeComponent * FindNativeParent(Component * inComponent);

        void * getWindowId();

        const void * getWindowId() const;

        void * getModuleId();

        const void * getModuleId() const;

    protected:
        void setWindowId(void * inWindowId);
        void setModuleId(void * inModuleId);

    private:
        NativeComponent * mNativeParent;
        void * mWindowId; // HWND on Windows
        void * mModuleId; // HINSTANCE on Windows
    };


} // namespace WinXUL


#endif // WINXUL_NATIVECOMPONENT_H_INCLUDED
