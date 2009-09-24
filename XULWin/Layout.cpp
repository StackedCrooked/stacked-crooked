#include "Layout.h"
#include "Utils/ErrorReporter.h"
#include <assert.h>


using namespace Utils;


namespace XULWin
{

    //void LinearLayoutManager::GetSizes(int inLength, size_t inItemCount, std::vector<int> & outSizes)
    //{
    //    assert(outSizes.empty());
    //    for (size_t idx = 0; idx != inItemCount; ++idx)
    //    {
    //        outSizes.push_back((int)(0.5 + (float)inLength/(float)inItemCount));
    //    }
    //}


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

    
    void LinearLayoutManager::getRects(const Rect & inRect,
                                       Alignment inAlign, 
                                       const std::vector<SizeInfo> & inSizeInfos,
                                       std::vector<Rect> & outRects)
    {
        std::vector<int> sizes;
        GetSizes(mOrientation == HORIZONTAL ? inRect.width() : inRect.height(),
                 inSizeInfos,
                 sizes);
        int xOffset = inRect.x();
        int yOffset = inRect.y();
        bool horizontal = mOrientation == HORIZONTAL;
        bool vertical = mOrientation == VERTICAL;
        for (size_t idx = 0; idx != inSizeInfos.size(); ++idx)
        {
            int x = horizontal ? xOffset : inRect.x();
            int y = vertical   ? yOffset : inRect.y();
            int w = horizontal ? sizes[idx] : inSizeInfos[idx].MinSizeOpposite;
            int h = vertical   ? sizes[idx] : inSizeInfos[idx].MinSizeOpposite;
            if (inAlign == Start)
            {
                if (horizontal)
                {
                    y = inRect.y();
                }
                else if (vertical)
                {
                    x = inRect.x();
                }
            }
            if (inAlign == Center)
            {
                if (horizontal)
                {
                    y = (inRect.height() - inSizeInfos[idx].MinSizeOpposite)/2;
                }
                else if (vertical)
                {
                    x = (inRect.width() - inSizeInfos[idx].MinSizeOpposite)/2;
                }
            }
            if (inAlign == End)
            {
                if (horizontal)
                {
                    y = inRect.height() - inSizeInfos[idx].MinSizeOpposite;
                }
                else if (vertical)
                {
                    x = inRect.width() - inSizeInfos[idx].MinSizeOpposite;
                }
            }
            else if (inAlign == Stretch)
            {
                if (horizontal)
                {
                    h = inRect.height();
                }
                else if (vertical)
                {
                    w = inRect.width();
                }
            }
            outRects.push_back(Rect(x, y, w, h));
            xOffset += w;
            yOffset += h;
        }
    }

    // 
    //void LinearLayoutManager::getRects(const Rect & inRect, const std::vector<int> & inFlexValues, std::vector<Rect> & outRects)
    //{
    //    std::vector<int> portions;
    //    GetSizes(mOrientation == HORIZONTAL ? inRect.width() : inRect.height(), inFlexValues, portions);
    //    int xOffset = inRect.x();
    //    int yOffset = inRect.y();
    //    for (size_t idx = 0; idx != portions.size(); ++idx)
    //    {
    //        int x = mOrientation == HORIZONTAL ? xOffset : inRect.x();
    //        int y = mOrientation  == VERTICAL ? yOffset : inRect.y();
    //        int width = mOrientation  == HORIZONTAL ? portions[idx] : inRect.width();
    //        int height = mOrientation  == VERTICAL ? portions[idx] : inRect.height();
    //        xOffset += width;
    //        yOffset += height;
    //        outRects.push_back(Rect(x, y, width, height));
    //    }
    //}

    
    void GridLayoutManager::GetRects(const Rect & inRect,
                                     const Utils::GenericGrid<GridSizeInfo> & inSizeInfos,
                                     Utils::GenericGrid<Rect> & outRects)
    {
        std::vector<int> horizontalSizes;
        std::vector<int> verticalSizes;

        // Get horizontal sizes
        {   
            std::vector<SizeInfo> horizontalProportions;         
            for (size_t colIdx = 0; colIdx != inSizeInfos.numColumns(); ++colIdx)
            {
                horizontalProportions.push_back(inSizeInfos.get(0, colIdx).Horizontal);
            }
            LinearLayoutManager::GetSizes(inRect.width(), horizontalProportions, horizontalSizes);
        }

        // Get vertical sizes
        {      
            std::vector<SizeInfo> verticalProportions;
            for (size_t rowIdx = 0; rowIdx != inSizeInfos.numRows(); ++rowIdx)
            {
                verticalProportions.push_back(inSizeInfos.get(rowIdx, 0).Vertical);
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
