#include "Image.h"
#include "AttributeController.h"
#include "Decorator.h"
#include "ChromeURL.h"
#include "Defaults.h"
#include "ElementImpl.h"
#include "Graphics.h"
#include "Poco/Path.h"
#include "Poco/UnicodeConverter.h"
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <gdiplus.h>


namespace XULWin
{	

    class NativeImage : public NativeControl,
                        public virtual SrcController,
                        public GdiplusLoader
    {
    public:
        typedef NativeControl Super;

        NativeImage(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual std::string getSrc() const;

        virtual void setSrc(const std::string & inSrc);

        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;

        bool initAttributeControllers();
        
        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        void paintImage(HDC inHDC, const RECT & rc);

        boost::scoped_ptr<Gdiplus::Bitmap> mImage;
        std::string mSrc;
    };


    Image::Image(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Image::Type(),
                inParent,
                new Decorator(new NativeImage(inParent->impl(), inAttributesMapping)))
    {
    }

    
    NativeImage::NativeImage(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, L"STATIC", 0, 0)
    {
    }

        
    std::string NativeImage::getSrc() const
    {
        return mSrc;
    }


    void NativeImage::setSrc(const std::string & inSrc)
    {
        ChromeURL url(inSrc, Defaults::locale());
        mSrc = url.convertToLocalPath();
        std::wstring utf16Path;
        Poco::UnicodeConverter::toUTF16(mSrc, utf16Path);
        Gdiplus::Bitmap * img = new Gdiplus::Bitmap(utf16Path.c_str());
        mImage.reset(img);
        if (mImage->GetLastStatus() != Gdiplus::Ok)
        {
            mImage.reset();
        }
    }


    int NativeImage::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        if (mImage)
        {
            return mImage->GetWidth();
        }
        return 0;
    }

    
    int NativeImage::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        if (mImage)
        {
            return mImage->GetHeight();
        }
        return 0;
    }


    bool NativeImage::initAttributeControllers()
    {
        Super::setAttributeController("src", static_cast<SrcController*>(this));
        return Super::initAttributeControllers();
    }
    
    
    void NativeImage::paintImage(HDC inHDC, const RECT & rc)
    {
        Gdiplus::Graphics g(inHDC);        
	    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.DrawImage(mImage.get(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }

    
    LRESULT NativeImage::handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam)
    {
        if (inMessage == WM_PAINT)
        {
            if (mImage)
            {
                RECT rc;
                ::GetClientRect(handle(), &rc);
                PAINTSTRUCT ps;
                HDC hDC = ::BeginPaint(handle(), &ps);
                paintImage(hDC, rc);
                ::EndPaint(handle(), &ps);
                return 0;
            }

        }
        return Super::handleMessage(inMessage, wParam, lParam);
    }


} // namespace XULWin