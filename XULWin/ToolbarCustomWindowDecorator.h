#ifndef TOOLBARCUSTOMWINDOWDECORATOR_H_INCLUDED
#define TOOLBARCUSTOMWINDOWDECORATOR_H_INCLUDED


#include "Decorator.h"
#include "Utils/ToolbarItem.h"


namespace XULWin
{

    class ToolbarCustomWindowDecorator : public Decorator,
                                         public Utils::IECustomWindow
    {
    public:
        ToolbarCustomWindowDecorator(ElementImpl * inDecoratedElement, boost::weak_ptr<Utils::Toolbar> inToolbar);

        virtual int commandId() const;

		virtual bool hasFocus() const;

		virtual void setFocus() const;

		virtual HWND handle() const;

        virtual bool initImpl();

    private:
    
        class ConcreteCustomWindow : public Utils::IECustomWindow
        {
        public:
            ConcreteCustomWindow(boost::weak_ptr<Utils::Toolbar> inToolbar, int inCommandId, HWND inHandle);

            virtual bool hasFocus() const;

		    virtual void setFocus() const;

		    virtual HWND handle() const;

            virtual int commandId() const;

        private:
            boost::weak_ptr<Utils::Toolbar> mToolbar;
            HWND mHandle;
            int mCommandId;
        };
        ConcreteCustomWindow * mCustomWindow;
    };

} // namespace XULWin

#endif // TOOLBARCUSTOMWINDOWDECORATOR_H_INCLUDED
