#include "Image.h"
#include "AttributeController.h"
#include "Decorator.h"
#include "ChromeURL.h"
#include "Defaults.h"
#include "ElementCreationSupport.h"
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
                        public virtual KeepAspectRatioController,
                        public GdiplusLoader
    {
    public:
        typedef NativeControl Super;

        NativeImage(ElementImpl * inParent, const AttributesMapping & inAttributesMapping);

        virtual std::string getSrc() const;

        virtual void setSrc(const std::string & inSrc);
        
        virtual int calculateWidth(SizeConstraint inSizeConstraint) const;

        virtual int calculateHeight(SizeConstraint inSizeConstraint) const;
        
        virtual int getWidth(SizeConstraint inSizeConstraint) const;
        
        virtual int getHeight(SizeConstraint inSizeConstraint) const;

        virtual void move(int x, int y, int w, int h);

        virtual bool getKeepAspectRatio() const;

        virtual void setKeepAspectRatio(bool inKeepAspectRatio);

        virtual bool initAttributeControllers();
        
        virtual LRESULT handleMessage(UINT inMessage, WPARAM wParam, LPARAM lParam);

    private:
        void paintImage(HDC inHDC, const RECT & rc);

        void getWidthAndHeight(int & outWidth, int & outHeight) const;

        boost::scoped_ptr<Gdiplus::Bitmap> mImage;
        boost::scoped_ptr<Gdiplus::Bitmap> mCachedImage;
        std::string mSrc;
        bool mKeepAspectRatio;
    };


    Image::Image(Element * inParent, const AttributesMapping & inAttributesMapping) :
        Element(Image::Type(),
                inParent,
                new MarginDecorator(CreateNativeControl<NativeImage>(inParent, inAttributesMapping)))
    {
    }


    NativeImage::NativeImage(ElementImpl * inParent, const AttributesMapping & inAttributesMapping) :
        NativeControl(inParent, inAttributesMapping, L"STATIC", 0, 0),
        mKeepAspectRatio(false)
    {
    }


    std::string NativeImage::getSrc() const
    {
        return mSrc;
    }


    void NativeImage::setSrc(const std::string & inSrc)
    {
        if (inSrc.find("chrome://") != std::string::npos)
        {        
            ChromeURL url(inSrc, Defaults::locale());
            mSrc = url.convertToLocalPath();
        }
        else
        {
            mSrc = inSrc;
        }
        std::wstring utf16Path;
        Poco::UnicodeConverter::toUTF16(mSrc, utf16Path);
        Gdiplus::Bitmap * img = new Gdiplus::Bitmap(utf16Path.c_str());
        mImage.reset(img);
        if (mImage->GetLastStatus() != Gdiplus::Ok)
        {
            mImage.reset();
        }
    }


    void NativeImage::getWidthAndHeight(int & width, int & height) const
    {
        float optimalWidth = (float)mImage->GetWidth();
        float optimalHeight = (float)mImage->GetHeight();        	
        if (optimalWidth < 1.0 || optimalHeight < 1.0)
        {
            width = 1;
            height = 1;
            return;
        }

        float resizeFactorX = mWidth.or(mHeight.or(MAXINT32))/optimalWidth;
        float resizeFactorY = mHeight.or(mWidth.or(MAXINT32))/optimalHeight;
        float resizeFactor = std::min<float>(resizeFactorX, resizeFactorY);
		
        width = (int)(resizeFactor*optimalWidth + 0.5f);
        if (width == 0)
        {
            width = 1;
        }

        height = (int)(resizeFactor*optimalHeight + 0.5f);		        
        if (height == 0)
        {
            height = 1;
        }
    }

    
    int NativeImage::getWidth(SizeConstraint inSizeConstraint) const
    {
        if (mWidth)
        {
            if (mHeight && getKeepAspectRatio())
            {
                int width = 0;
                int height = 0;  
                getWidthAndHeight(width, height);
                return width;
            }
            else
            {
                return mWidth;
            }
        }
        // deduce width from height
        else if (mHeight && !mWidth)
        {
            int width = 0;
            int height = 0;  
            getWidthAndHeight(width, height);
            return width;
        }
        // if flex=0, then choose the natural width & height
        else if (mImage && !mWidth && !mHeight && getFlex() == 0)
        {
            return mImage->GetWidth();
        }
        else
        {
            return Super::getWidth(inSizeConstraint);
        }
    }
        
    
    int NativeImage::getHeight(SizeConstraint inSizeConstraint) const
    {        
        if (mHeight)
        {
            if (mWidth && getKeepAspectRatio())
            {
                int width = 0;
                int height = 0;  
                getWidthAndHeight(width, height);
                return height;
            }
            else
            {
                return mHeight;
            }
        }
        // deduce height from width
        else if (mWidth && !mHeight)
        {
            int width = 0;
            int height = 0;  
            getWidthAndHeight(width, height);
            return height;
        }
        // if flex=0, then choose the natural width & height
        else if (mImage && !mWidth && !mHeight && getFlex() == 0)
        {
            return mImage->GetHeight();
        }
        else
        {
            return Super::getHeight(inSizeConstraint);
        }
    }
    
    
    void NativeImage::move(int x, int y, int w, int h)
    {
        if (mImage && (w != clientRect().width() || h != clientRect().height()))
        {
            // create a resized copy of the original
            mCachedImage.reset(new Gdiplus::Bitmap(w, h, PixelFormat32bppARGB));
        	
            Gdiplus::Graphics g(mCachedImage.get());
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
            g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);        	
            g.DrawImage(mImage.get(), Gdiplus::Rect(0, 0, INT(w), INT(h)));
        }
        Super::move(x, y, w, h);
    }
    
    
    bool NativeImage::getKeepAspectRatio() const
    {
        return mKeepAspectRatio;
    }


    void NativeImage::setKeepAspectRatio(bool inKeepAspectRatio)
    {
        mKeepAspectRatio = inKeepAspectRatio;
    }


    int NativeImage::calculateWidth(SizeConstraint inSizeConstraint) const
    {
        if (inSizeConstraint == Minimum)
        {
            return 0;
        }

        if (mImage)
        {
            return mImage->GetWidth();
        }

        return 0;
    }


    int NativeImage::calculateHeight(SizeConstraint inSizeConstraint) const
    {
        if (inSizeConstraint == Minimum)
        {
            return 0;
        }

        if (mImage)
        {
            return mImage->GetHeight();
        }

        return 0;
    }


    bool NativeImage::initAttributeControllers()
    {
        Super::setAttributeController("src", static_cast<SrcController*>(this));
        Super::setAttributeController("keepaspectratio", static_cast<KeepAspectRatioController*>(this));
        return Super::initAttributeControllers();
    }


    void NativeImage::paintImage(HDC inHDC, const RECT & rc)
    {
        Gdiplus::Graphics g(inHDC);        
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        if (mCachedImage)
        {
            g.DrawImage(mCachedImage.get(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
        }
        else if (mImage)
        {
            g.DrawImage(mImage.get(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
        }
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
