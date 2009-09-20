#ifndef LAYOUT_H_INCLUDED
#define LAYOUT_H_INCLUDED


#include <vector>


namespace XULWin
{

	class Point
	{
    public:		
		Point() : mX(0), mY(0) {}
		
        Point(int inX, int inY) : mX(inX), mY(inY) {}
		
		bool operator==(const Point & inOtherPoint)
		{
			return mX == inOtherPoint.mX && mY == inOtherPoint.mY;
		}
		
		bool operator!=(const Point & inOtherPoint)
		{
			return mX != inOtherPoint.mX || mY != inOtherPoint.mY;
		}

        int x() const { return mX; }

        int y() const { return mY; }
    
    private:
		int mX;
		int mY;
	};

	
	class Dimension
	{
    public:
		Dimension() : mWidth(0), mHeight(0) {}

		Dimension(int inWidth, int inHeight) : mWidth(inWidth), mHeight(inHeight) {}
		
		bool operator==(const Dimension & inOtherDimension)
		{
			return mWidth == inOtherDimension.mWidth && mHeight == inOtherDimension.mHeight;
		}
		
		bool operator!=(const Dimension & inOtherDimension)
		{
			return mWidth != inOtherDimension.mWidth || mHeight != inOtherDimension.mHeight;
		}

        int width() const { return mWidth; }

        int height() const { return mHeight; }

    private:
		int mWidth;
		int mHeight;		
	};


	class Rect
	{
    public:		
		Rect() : mLocation(), mSize() {}
        
        Rect(const Point & inLocation, const Dimension & inSize) : mLocation(inLocation), mSize(inSize) {}

		Rect(int inX, int inY, int inWidth, int inHeight) : mLocation(inX, inY), mSize(inWidth, inHeight) {}
		
		bool operator==(const Rect & inOtherRect)
		{
			return mLocation == inOtherRect.mLocation && mSize == inOtherRect.mSize;
		}
		
		bool operator!=(const Rect & inOtherRect)
		{
			return mLocation != inOtherRect.mLocation || mSize != inOtherRect.mSize;
		}

        int x() const { return mLocation.x(); }

        int y() const { return mLocation.y(); }

        int width() const { return mSize.width(); }

        int height() const { return mSize.height(); }

        const Point & location() const { return mLocation; }

        const Dimension & size() const { return mSize; }

    private:
		Point mLocation;
		Dimension mSize;
	};
	
	
    class Margins
	{
    public:		
		Margins() : mLeft(0), mTop(0), mRight(0), mBottom(0) {}

		Margins (int inLeft, int inTop, int inRight, int inBottom) : mLeft(inLeft), mTop(inTop), mRight(inRight), mBottom(inBottom) {}

        int left() const { return mLeft; }

        int top() const { return mTop; }

        int right() const { return mRight; }

        int bottom() const { return mBottom; }
	
		bool operator==(const Margins & inOtherMargins)
		{
			return mLeft == inOtherMargins.mLeft
				&& mTop == inOtherMargins.mTop
				&& mRight == inOtherMargins.mRight
				&& mBottom == inOtherMargins.mBottom;
		}
		
		bool operator!=(const Margins & inOtherMargins)
		{
			return mLeft != inOtherMargins.mLeft
				|| mTop != inOtherMargins.mTop
				|| mRight != inOtherMargins.mRight
				|| mBottom != inOtherMargins.mBottom;
		}

    private:
		int mLeft;
		int mTop;
		int mRight;
		int mBottom;
	};

    enum Orientation
    {
        HORIZONTAL,
        VERTICAL
    };

    
    class LinearLayoutManager
    {
    public:
        static void GetPortions(int inLength, size_t inItemCount, std::vector<int> & outPortions);

        static void GetPortions(int inLength, const std::vector<int> & inProportions, std::vector<int> & outPortions);

        struct Portion
        {
            Portion(int inFlex, int inMinSize) : Flex(inFlex), MinSize(inMinSize){}
            int Flex;
            int MinSize;
        };
        static void GetPortions(int inLength, const std::vector<Portion> & inProportions, std::vector<int> & outPortions);

        LinearLayoutManager(Orientation inOrientation);

        void getRects(const Rect & inRect, size_t inItemCount, std::vector<Rect> & outRects);

        void getRects(const Rect & inRect, const std::vector<int> & inProportions, std::vector<Rect> & outRects);

    private:
        Orientation mOrientation;
    };

} // namespace XULWin


#endif // LAYOUT_H_INCLUDED
