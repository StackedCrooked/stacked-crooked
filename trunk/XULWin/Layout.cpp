#include "Layout.h"
#include <assert.h>


namespace XULWin
{

    void LinearLayoutManager::GetPortions(int inLength, size_t inItemCount, std::vector<int> & outPortions)
    {
        assert(outPortions.empty());
        for (size_t idx = 0; idx != inItemCount; ++idx)
        {
            outPortions.push_back((int)(0.5 + (float)inLength/(float)inItemCount));
        }
    }


    void LinearLayoutManager::GetPortions(int inLength, const std::vector<int> & inProportions, std::vector<int> & outPortions)
    {
        assert(outPortions.empty());
        
        int sumOfProportions = 0;
        for (size_t idx = 0; idx != inProportions.size(); ++idx)
        {
            sumOfProportions += inProportions[idx];
        }

        for (size_t idx = 0; idx != inProportions.size(); ++idx)
        {
            int proportion = 0;
            if (inProportions[idx] != 0 && sumOfProportions != 0)
            {
                proportion = (int)(0.5 + (float)inLength*(float)inProportions[idx]/(float)sumOfProportions);
            }
            outPortions.push_back(proportion);
        }
    }


    void LinearLayoutManager::GetPortions(int inLength, const std::vector<Portion> & inProportions, std::vector<int> & outPortions)
    {
        assert(outPortions.empty());
        
        int sumOfProportions = 0;
        for (size_t idx = 0; idx != inProportions.size(); ++idx)
        {
            sumOfProportions += inProportions[idx].Flex;
        }

        for (size_t idx = 0; idx != inProportions.size(); ++idx)
        {
            int length = 0;
            if (sumOfProportions != 0)
            {
                length = (int)(0.5 + (float)inLength*(float)inProportions[idx].Flex/(float)sumOfProportions);
            }
            int minLength = inProportions[idx].MinSize;
            if (length < minLength)
            {
                length = minLength;
            }
            outPortions.push_back(length);
        }
    }


    LinearLayoutManager::LinearLayoutManager(Orientation inOrientation) :
        mOrientation(inOrientation)
    {
    }

    
    void LinearLayoutManager::getRects(const Rect & inRect, size_t inItemCount, std::vector<Rect> & outRects)
    {
        std::vector<int> portions;
        GetPortions(mOrientation == HORIZONTAL ? inRect.width() : inRect.height(), inItemCount, portions);
        int xOffset = inRect.x();
        int yOffset = inRect.y();
        for (size_t idx = 0; idx != inItemCount; ++idx)
        {
            int x = mOrientation == HORIZONTAL ? xOffset : inRect.x();
            int y = mOrientation  == VERTICAL ? yOffset : inRect.y();
            int width = mOrientation  == HORIZONTAL ? portions[idx] : inRect.width();
            int height = mOrientation  == VERTICAL ? portions[idx] : inRect.height();
            xOffset += width;
            yOffset += height;
            outRects.push_back(Rect(x, y, width, height));
        }
    }

     
    void LinearLayoutManager::getRects(const Rect & inRect, const std::vector<int> & inProportions, std::vector<Rect> & outRects)
    {
        std::vector<int> portions;
        GetPortions(mOrientation == HORIZONTAL ? inRect.width() : inRect.height(), inProportions, portions);
        int xOffset = inRect.x();
        int yOffset = inRect.y();
        for (size_t idx = 0; idx != portions.size(); ++idx)
        {
            int x = mOrientation == HORIZONTAL ? xOffset : inRect.x();
            int y = mOrientation  == VERTICAL ? yOffset : inRect.y();
            int width = mOrientation  == HORIZONTAL ? portions[idx] : inRect.width();
            int height = mOrientation  == VERTICAL ? portions[idx] : inRect.height();
            xOffset += width;
            yOffset += height;
            outRects.push_back(Rect(x, y, width, height));
        }
    }


} // namespace XulWin
