#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include <assert.h>


using namespace Utils;


namespace XULWin
{

    void LinearLayoutManager::GetSizes(int inLength, size_t inItemCount, std::vector<int> & outSizes)
    {
        assert(outSizes.empty());
        for (size_t idx = 0; idx != inItemCount; ++idx)
        {
            outSizes.push_back((int)(0.5 + (float)inLength/(float)inItemCount));
        }
    }


    void LinearLayoutManager::GetSizes(int inLength, const std::vector<int> & inFlexValues, std::vector<int> & outSizes)
    {
        assert(outSizes.empty());
        
        int sumOfProportions = 0;
        for (size_t idx = 0; idx != inFlexValues.size(); ++idx)
        {
            sumOfProportions += inFlexValues[idx];
        }

        for (size_t idx = 0; idx != inFlexValues.size(); ++idx)
        {
            int proportion = 0;
            if (inFlexValues[idx] != 0 && sumOfProportions != 0)
            {
                proportion = (int)(0.5 + (float)inLength*(float)inFlexValues[idx]/(float)sumOfProportions);
            }
            outSizes.push_back(proportion);
        }
    }


    void LinearLayoutManager::GetSizes(int inLength, const std::vector<SizeInfo> & inFlexValues, std::vector<int> & outSizes)
    {
        assert(outSizes.empty());
        
        int sumOfProportions = 0;
        int availableLength = inLength;
        std::vector<int> flexValues;
        for (size_t idx = 0; idx != inFlexValues.size(); ++idx)
        {
            if (inFlexValues[idx].Flex != 0)
            {
                flexValues.push_back(inFlexValues[idx].Flex);
            }
            else
            {
                availableLength -= inFlexValues[idx].MinSize;
            }
        }
        std::vector<int> sizes;
        GetSizes(availableLength, flexValues, sizes);

        int sizesIdx = 0;
        for (size_t idx = 0; idx != inFlexValues.size(); ++idx)
        {
            if (inFlexValues[idx].Flex != 0)
            {
                int size = sizes[sizesIdx];
                if (size < inFlexValues[idx].MinSize)
                {
                    size = inFlexValues[idx].MinSize;
                }
                outSizes.push_back(size);
                sizesIdx++;
            }
            else
            {
                outSizes.push_back(inFlexValues[idx].MinSize);
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
        GetSizes(mOrientation == HORIZONTAL ? inRect.width() : inRect.height(), inItemCount, portions);
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

     
    void LinearLayoutManager::getRects(const Rect & inRect, const std::vector<int> & inFlexValues, std::vector<Rect> & outRects)
    {
        std::vector<int> portions;
        GetSizes(mOrientation == HORIZONTAL ? inRect.width() : inRect.height(), inFlexValues, portions);
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
                                     const Utils::GenericGrid<GridProportion> & inFlexValues,
                                     Utils::GenericGrid<Rect> & outRects)
    {
        if (inFlexValues.numRows() == 0 || inFlexValues.numColumns() == 0)
        {
            ReportError("GridLayoutManager: number of columns or rows of inFlexValues is zero.");
            return;
        }

        assert(inFlexValues.numRows() == outRects.numRows());
        assert(inFlexValues.numColumns() == outRects.numColumns());
        
        std::vector<int> horizontalSizes;
        std::vector<int> verticalSizes;

        // Get horizontal sizes
        {   
            std::vector<SizeInfo> horizontalProportions;         
            for (size_t colIdx = 0; colIdx != inFlexValues.numColumns(); ++colIdx)
            {
                horizontalProportions.push_back(inFlexValues.get(0, colIdx).Horizontal);
            }
            LinearLayoutManager::GetSizes(inRect.width(), horizontalProportions, horizontalSizes);
        }

        // Get vertical sizes
        {      
            std::vector<SizeInfo> verticalProportions;
            for (size_t rowIdx = 0; rowIdx != inFlexValues.numRows(); ++rowIdx)
            {
                verticalProportions.push_back(inFlexValues.get(rowIdx, 0).Vertical);
            }
            LinearLayoutManager::GetSizes(inRect.height(), verticalProportions, verticalSizes);
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
