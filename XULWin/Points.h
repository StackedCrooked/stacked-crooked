#ifndef POINTS_H_INCLUDED
#define POINTS_H_INCLUDED


#include <vector>


namespace XULWin
{

	class Point
	{
    public:		
		Point();
		
        Point(int x, int y);
		
		bool operator==(const Point & inOtherPoint);
		
		bool operator!=(const Point & inOtherPoint);

        int x() const;

        int y() const;
    
    private:
		int mX;
		int mY;
	};

    typedef std::vector<Point> Points;

} // namespace XULWin


#endif // POINTS_H_INCLUDED
