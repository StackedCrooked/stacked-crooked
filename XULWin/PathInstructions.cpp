#include "PathInstructions.h"


namespace XULWin
{

namespace SVG
{

    
    PathInstruction::PathInstruction(Type inType, Positioning inPositioning, const PointFs & inPoints) :
        mType(inType),
        mPoints(inPoints),
        mPositioning(inPositioning)
    {
    }
        
        
    PathInstruction::Type PathInstruction::type() const
    {
        return mType;
    }


    size_t PathInstruction::numPoints() const
    {
        return mPoints.size();
    }


    const PointF & PathInstruction::getPoint(size_t inIdx) const
    {
        return mPoints[inIdx];
    }
    
    
    PointF PathInstruction::getAbsolutePoint(const PointF & inRef, size_t inIdx) const
    {
        return PointF(inRef.x() + mPoints[inIdx].x(), inRef.y() + mPoints[inIdx].y());
    }
    
    
    PathInstruction::Positioning PathInstruction::positioning() const
    {
        return mPositioning;
    }
    
    
    const PointFs & PathInstruction::points() const
    {
        return mPoints;
    }
    
    
    PointFs & PathInstruction::points()
    {
        return mPoints;
    }

} // namespace SVG

} // namespace XULWin
