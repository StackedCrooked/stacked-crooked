#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include <assert.h>


using namespace Utils;


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


    void LinearLayoutManager::GetPortions(int inLength, const std::vector<Proportion> & inProportions, std::vector<int> & outPortions)
    {
        assert(outPortions.empty());
        
        int sumOfProportions = 0;
        int availableLength = inLength;
        std::vector<int> flexValues;
        for (size_t idx = 0; idx != inProportions.size(); ++idx)
        {
            if (inProportions[idx].Flex != 0)
            {
                flexValues.push_back(inProportions[idx].Flex);
            }
            else
            {
                availableLength -= inProportions[idx].MinSize;
            }
        }
        std::vector<int> sizes;
        GetPortions(availableLength, flexValues, sizes);

        int sizesIdx = 0;
        for (size_t idx = 0; idx != inProportions.size(); ++idx)
        {
            if (inProportions[idx].Flex != 0)
            {
                int size = sizes[sizesIdx];
                if (size < inProportions[idx].MinSize)
                {
                    size = inProportions[idx].MinSize;
                }
                outPortions.push_back(size);
                sizesIdx++;
            }
            else
            {
                outPortions.push_back(inProportions[idx].MinSize);
            }
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

    
    void GridLayoutManager::GetRects(const Rect & inRect,
                                     const Utils::GenericGrid<GridProportion> & inProportions,
                                     Utils::GenericGrid<Rect> & outRects)
    {
        if (inProportions.numRows() == 0 || inProportions.numColumns() == 0)
        {
            ReportError("GridLayoutManager: number of columns or rows of inProportions is zero.");
            return;
        }

        assert(inProportions.numRows() == outRects.numRows());
        assert(inProportions.numColumns() == outRects.numColumns());
        
        std::vector<int> horizontalSizes;
        std::vector<int> verticalSizes;

        // Get horizontal sizes
        {   
            std::vector<Proportion> horizontalProportions;         
            for (size_t colIdx = 0; colIdx != inProportions.numColumns(); ++colIdx)
            {
                horizontalProportions.push_back(inProportions.get(0, colIdx).Horizontal);
            }
            LinearLayoutManager::GetPortions(inRect.width(), horizontalProportions, horizontalSizes);
        }

        // Get vertical sizes
        {      
            std::vector<Proportion> verticalProportions;
            for (size_t rowIdx = 0; rowIdx != inProportions.numRows(); ++rowIdx)
            {
                verticalProportions.push_back(inProportions.get(0, rowIdx).Vertical);
            }
            LinearLayoutManager::GetPortions(inRect.height(), verticalProportions, verticalSizes);
        }
        
        int offsetX = 0;
        int offsetY = 0;
        for (size_t colIdx = 0; colIdx != outRects.numColumns(); ++colIdx)
        {
            int width = horizontalSizes[colIdx];
            for (size_t rowIdx = 0; rowIdx != outRects.numRows(); ++rowIdx)
            {
                int height = verticalSizes[rowIdx];
                outRects.set(rowIdx, colIdx, Rect(offsetX, offsetY, width, height));
                offsetY += height;
            }
            offsetX += width;
            offsetY = 0;
        }
    }


} // namespace XulWin
