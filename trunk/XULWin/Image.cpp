#include "Image.h"
#include "Decorator.h"
#include "ElementImpl.h"
#include "ChromeURL.h"
#include "Defaults.h"
#include "Poco/Path.h"
#include "Poco/UnicodeConverter.h"
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <gdiplus.h>


namespace XULWin
{	
	class GdiplusLoader : boost::noncopyable
	{
		public:
			GdiplusLoader()
			{
				assert(sRefCount >= 0);
				if (sRefCount++ == 0)
				{
					// Init Gdiplus
					Gdiplus::GdiplusStartupInput gdiplusStartupInput;
					Gdiplus::GdiplusStartup(&mGdiPlusToken, &gdiplusStartupInput, NULL);
				}
			}

			~GdiplusLoader()
			{
				assert(sRefCount >= 0);
				if (--sRefCount == 0)
				{
					Gdiplus::GdiplusShutdown(mGdiPlusToken);
				}
				assert(sRefCount >= 0);
			}
		private:
			ULONG_PTR mGdiPlusToken;
			static int sRefCount;
	};

    int GdiplusLoader::sRefCount = 0;


    class NativeImage : public NativeControl, public GdiplusLoader
    {
    public:
        typedef NativeControl Super;

        NativeImage(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual int calculateMinimumWidth() const;

        virtual int calculateMinimumHeight() const;

        bool initOldAttributeControllers();

        void setSrc(const std::string & inSrc);

        const std::string & src() const;
        
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


    int NativeImage::calculateMinimumWidth() const
    {
        if (mImage)
        {
            return mImage->GetWidth();
        }
        return 0;
    }

    
    int NativeImage::calculateMinimumHeight() const
    {
        if (mImage)
        {
            return mImage->GetHeight();
        }
        return 0;
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

    
    const std::string & NativeImage::src() const
    {
        return mSrc;
    }


    bool NativeImage::initOldAttributeControllers()
    {
        AttributeGetter srcGetter = boost::bind(&NativeImage::src, this);
        AttributeSetter srcSetter = boost::bind(&NativeImage::setSrc, this, _1);
        setOldAttributeController("src", OldAttributeController(srcGetter, srcSetter));
        return NativeControl::initOldAttributeControllers();
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