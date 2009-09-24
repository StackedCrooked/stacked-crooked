#include "Image.h"
#include "ElementImpl.h"
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

        NativeImage(ElementImpl * inParent);

        virtual int minimumWidth() const;

        virtual int minimumHeight() const;

        bool initAttributeControllers();

        void setSource(const std::string & inSource);

        const std::string & source() const;
        
        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        void paintImage(HDC inHDC, const RECT & rc);

        boost::scoped_ptr<Gdiplus::Bitmap> mImage;
        std::string mSource;
    };


    Image::Image(Element * inParent) :
        Element(Image::Type(),
                inParent,
                new NativeImage(inParent->impl()))
    {
    }

    
    NativeImage::NativeImage(ElementImpl * inParent) :
        NativeControl(inParent, L"STATIC", 0, 0)
    {
    }


    int NativeImage::minimumWidth() const
    {
        if (mImage)
        {
            return mImage->GetWidth();
        }
        return 0;
    }

    
    int NativeImage::minimumHeight() const
    {
        if (mImage)
        {
            return mImage->GetHeight();
        }
        return 0;
    }

    
    void NativeImage::setSource(const std::string & inSource)
    {
        static const std::string cChrome = "chrome://";
        if (inSource.find(cChrome) == 0)
        {
            // Change this pattern: chrome://myapp/skin/icons/myimg.jpg
            // Into this pattern:   chrome/skin/icons/myimg.jpg
            mSource = inSource.substr(cChrome.size(), mSource.size() - cChrome.size());
            size_t slashIdx = mSource.find("/");
            if (slashIdx != std::string::npos)
            {
                mSource = "chrome/" + mSource.substr(slashIdx + 1, mSource.size() - slashIdx - 1);
            }
        }
        else
        {
            mSource = inSource;
        }
        std::wstring utf16Path;
        Poco::UnicodeConverter::toUTF16(mSource, utf16Path);
        Gdiplus::Bitmap * img = new Gdiplus::Bitmap(utf16Path.c_str());
        mImage.reset(img);
        if (mImage->GetLastStatus() != Gdiplus::Ok)
        {
            mImage.reset();
        }
    }

    
    const std::string & NativeImage::source() const
    {
        return mSource;
    }


    bool NativeImage::initAttributeControllers()
    {
        AttributeGetter srcGetter = boost::bind(&NativeImage::source, this);
        AttributeSetter srcSetter = boost::bind(&NativeImage::setSource, this, _1);
        setAttributeController("src", AttributeController(srcGetter, srcSetter));
        return NativeControl::initAttributeControllers();
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