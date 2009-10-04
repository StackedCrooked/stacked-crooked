#ifndef POINTS_H_INCLUDED
#define POINTS_H_INCLUDED


#include <vector>


namespace XULWin
{

    template<class T>
    class GenericPoint
    {
    public:        
        GenericPoint() : mX(0), mY(0) {}
        
        GenericPoint(T x, T y) : mX(x), mY(y) {}
        
        bool operator==(const GenericPoint & inOtherPoint)
        { return mX == inOtherPoint.mX && mY == inOtherPoint.mY; }
        
        bool operator!=(const GenericPoint & inOtherPoint)
        { return mX != inOtherPoint.mX || mY != inOtherPoint.mY; }

        T x() const { return mX; }

        T y() const { return mY; }
    
    private:
        T mX;
        T mY;
    };


    typedef GenericPoint<int> Point;

    typedef GenericPoint<float> PointF;

    typedef std::vector<Point> Points;

    typedef std::vector<PointF> PointFs;

} // namespace XULWin


#endif // POINTS_H_INCLUDED
