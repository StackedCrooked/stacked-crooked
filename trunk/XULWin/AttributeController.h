#ifndef ATTRIBUTECONTROLLER_H_INCLUDED
#define ATTRIBUTECONTROLLER_H_INCLUDED


#include "Layout.h"
#include <string>


namespace XULWin
{
    
    class AttributeController
    {
    public:
        virtual void get(std::string & outValue) = 0;
        virtual void set(const std::string & inValue) = 0;
    };


    class TitleController : public AttributeController
    {
    public:
        TitleController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual std::string getTitle() const = 0;

        virtual void setTitle(const std::string & inTitle) = 0;
    };


    class WidthController : public AttributeController
    {
    public:
        WidthController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getWidth() const = 0;

        virtual void setWidth(int inWidth) = 0;
    };


    class HeightController : public AttributeController
    {
    public:
        HeightController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getHeight() const = 0;

        virtual void setHeight(int inHeight) = 0;
    };


    class DisabledController : public AttributeController
    {
    public:
        DisabledController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual bool getDisabled() const = 0;

        virtual void setDisabled(bool inDisabled) = 0;
    };


    class LabelController : public AttributeController
    {
    public:
        LabelController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual std::string getLabel() const = 0;

        virtual void setLabel(const std::string & inLabel) = 0;
    };


    class StringValueController : public AttributeController
    {
    public:
        StringValueController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual std::string getValue() const = 0;

        virtual void setValue(const std::string & inStringValue) = 0;
    };


    class ReadOnlyController : public AttributeController
    {
    public:
        ReadOnlyController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual bool isReadOnly() const = 0;

        virtual void setReadOnly(bool inReadOnly) = 0;
    };


    class CheckedController : public AttributeController
    {
    public:
        CheckedController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual bool isChecked() const = 0;

        virtual void setChecked(bool inChecked) = 0;
    };


    class OrientController : public AttributeController
    {
    public:
        OrientController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual Orientation getOrient() const = 0;

        virtual void setOrient(Orientation inOrient) = 0;
    };


    class AlignController : public AttributeController
    {
    public:
        AlignController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual Alignment getAlign() const = 0;

        virtual void setAlign(Alignment inAlign) = 0;
    };


    class IntValueController : public AttributeController
    {
    public:
        IntValueController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getValue() const = 0;

        virtual void setValue(int inValue) = 0;
    };


    class SelectedIndexController : public AttributeController
    {
    public:
        SelectedIndexController(){}

        virtual void get(std::string & outSelectedIndex);

        virtual void set(const std::string & inSelectedIndex);

        virtual int getSelectedIndex() const = 0;

        virtual void setSelectedIndex(int inSelectedIndex) = 0;
    };


    class ScrollbarCurrentPositionController : public AttributeController
    {
    public:
        ScrollbarCurrentPositionController(){}

        virtual void get(std::string & outCurpos);

        virtual void set(const std::string & inCurpos);

        virtual int getCurrentPosition() const = 0;

        virtual void setCurrentPosition(int inCurrentPosition) = 0;
    };


    class ScrollbarMaxPositionController : public AttributeController
    {
    public:
        ScrollbarMaxPositionController(){}

        virtual void get(std::string & outMaxPosition);

        virtual void set(const std::string & inMaxPosition);

        virtual int getMaxPosition() const = 0;

        virtual void setMaxPosition(int inMaxPosition) = 0;
    };


    class ScrollbarIncrementController : public AttributeController
    {
    public:
        ScrollbarIncrementController(){}

        virtual void get(std::string & outIncrement);

        virtual void set(const std::string & inIncrement);

        virtual int getIncrement() const = 0;

        virtual void setIncrement(int inIncrement) = 0;
    };


    class ScrollbarPageIncrementController : public AttributeController
    {
    public:
        ScrollbarPageIncrementController(){}

        virtual void get(std::string & outPageIncrement);

        virtual void set(const std::string & inPageIncrement);

        virtual int getPageIncrement() const = 0;

        virtual void setPageIncrement(int inPageIncrement) = 0;
    };


    class SrcController : public AttributeController
    {
    public:
        SrcController(){}

        virtual void get(std::string & outPageIncrement);

        virtual void set(const std::string & inPageIncrement);

        virtual std::string getSrc() const = 0;

        virtual void setSrc(const std::string & inSrc) = 0;
    };

} // namespace XULWin


#endif // ATTRIBUTECONTROLLER_H_INCLUDED
