#include "PathInstructions.h"


namespace XULWin
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

} // namespace XULWin
