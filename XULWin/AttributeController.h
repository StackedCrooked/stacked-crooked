#ifndef ATTRIBUTECONTROLLER_H_INCLUDED
#define ATTRIBUTECONTROLLER_H_INCLUDED


#include "Enums.h"
#include "Points.h"
#include "PathInstructions.h"
#include "RGBColor.h"
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


    class FlexController : public AttributeController
    {
    public:
        FlexController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getFlex() const = 0;

        virtual void setFlex(int inFlex) = 0;
    };


    class DisabledController : public AttributeController
    {
    public:
        DisabledController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual bool isDisabled() const = 0;

        virtual void setDisabled(bool inDisabled) = 0;
    };


    class HiddenController : public AttributeController
    {
    public:
        HiddenController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual bool isHidden() const = 0;

        virtual void setHidden(bool inHidden) = 0;
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


    class RowsController : public AttributeController
    {
    public:
        RowsController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual int getRows() const = 0;

        virtual void setRows(int inRows) = 0;
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

        virtual Orient getOrient() const = 0;

        virtual void setOrient(Orient inOrient) = 0;
    };


    class AlignController : public AttributeController
    {
    public:
        AlignController(){}

        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual Align getAlign() const = 0;

        virtual void setAlign(Align inAlign) = 0;
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


    /**
     * This is a non-standard extension that applies to Image elements only.
     * The attribute "keepaspectratio" has a boolean value indicating that the
     * resized width and height should be congruent with the natural width and
     * height.
     */
    class KeepAspectRatioController : public AttributeController
    {
    public:
        KeepAspectRatioController(){}

        virtual void get(std::string & outPageIncrement);

        virtual void set(const std::string & inPageIncrement);

        virtual bool getKeepAspectRatio() const = 0;

        virtual void setKeepAspectRatio(bool inKeepAspectRatio) = 0;
    };


    class PointsController : public AttributeController
    {
    public:
        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual const Points & getPoints() const = 0;

        virtual void setPoints(const Points & inPoints) = 0;
    };


    class PathInstructionsController : public AttributeController
    {
    public:
        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual const SVG::PathInstructions & getPathInstructions() const = 0;

        virtual void setPathInstructions(const SVG::PathInstructions & inPathInstructions) = 0;
    };


    class FillController : public AttributeController
    {
    public:
        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void setFill(const RGBColor & inColor) = 0;

        virtual const RGBColor & getFill() const = 0;
    };


    class StrokeController : public AttributeController
    {
    public:
        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void setStroke(const RGBColor & inColor) = 0;

        virtual const RGBColor & getStroke() const = 0;
    };


    class StrokeWidthController : public AttributeController
    {
    public:
        virtual void get(std::string & outValue);

        virtual void set(const std::string & inValue);

        virtual void setStrokeWidth(int inStrokeWidth) = 0;

        virtual int getStrokeWidth() const = 0;
    };

} // namespace XULWin


#endif // ATTRIBUTECONTROLLER_H_INCLUDED
