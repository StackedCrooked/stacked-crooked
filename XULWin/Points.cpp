#include "Points.h"


namespace XULWin
{

    Point::Point() :
        mX(0),
        mY(0)
    {
    }

    Point::Point(int x, int y) :
        mX(x),
        mY(y)
    {
    }


    int Point::x() const
    {
        return mX;
    }


    int Point::y() const
    {
        return mY;
    }		
		

	bool Point::operator==(const Point & inOtherPoint)
	{
		return mX == inOtherPoint.mX && mY == inOtherPoint.mY;
	}

	
	bool Point::operator!=(const Point & inOtherPoint)
	{
		return mX != inOtherPoint.mX || mY != inOtherPoint.mY;
	}

} // namespace XULWin
