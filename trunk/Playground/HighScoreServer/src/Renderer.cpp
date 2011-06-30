#include "Renderer.h"
#include "Utils.h"
#include "Poco/Util/Application.h"
#include "Poco/Types.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>


using namespace Poco::Data;


namespace HSServer
{

    Renderer::Renderer(const std::string & inCollectionTitle,
                       const std::string & inRecordTitle,
                       const Poco::Data::RecordSet & inRecordSet) :
        mCollectionTitle(inCollectionTitle),
        mRecordTitle(inRecordTitle),
        mRecordSet(inRecordSet)
    {
    }


    XMLRenderer::XMLRenderer(const std::string & inCollectionTitle,
                             const std::string & inRecordTitle,
                             Poco::Data::RecordSet & inRecordSet) :
        Renderer(inCollectionTitle, inRecordTitle, inRecordSet)
    {
    }


    void XMLRenderer::render(std::ostream & ostr)
    {
        ostr << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        ostr << "<" << mCollectionTitle << ">\n";
        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            ostr << "<" << mRecordTitle;
            for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
            {
                // I choose to make all XML attribute names lower-case
                std::string name = MakeLowerCase(mRecordSet.columnName(colIdx));

                std::string value = mRecordSet.value(colIdx, rowIdx).convert<std::string>();
                ostr << " " << name << "=\"" << URIEncode(value) << "\"";
            }
            ostr << "/>\n";
        }
        ostr << "</" << mCollectionTitle << ">\n";
    }


    HTMLRenderer::HTMLRenderer(const std::string & inCollectionTitle,
                               const std::string & inRecordTitle,
                               Poco::Data::RecordSet & inRecordSet) :
        Renderer(inCollectionTitle, inRecordTitle, inRecordSet)
    {
    }


    void HTMLRenderer::renderColumn(size_t inRowIdx, size_t inColIdx, std::ostream & ostr)
    {
        StreamHTML("td", mRecordSet.value(inColIdx, inRowIdx).convert<std::string>(), HTMLFormatting_NoBreaks, ostr);
    }


    void HTMLRenderer::renderColumns(size_t inRowIdx, std::ostream & ostr)
    {
        for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
        {
            renderColumn(inRowIdx, colIdx, ostr);
        }
    }


    void HTMLRenderer::renderRow(size_t inRowIdx, std::ostream & ostr)
    {
        StreamHTML("tr", boost::bind(&HTMLRenderer::renderColumns, this, inRowIdx, _1), HTMLFormatting_OneLiner, ostr);
    }


    void HTMLRenderer::renderRows(std::ostream & ostr)
    {
        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            renderRow(rowIdx, ostr);
        }
    }


    void HTMLRenderer::renderTBody(std::ostream & ostr)
    {
        StreamHTML("tbody", boost::bind(&HTMLRenderer::renderRows, this, _1), HTMLFormatting_ThreeLiner, ostr);
    }


    void HTMLRenderer::renderTHead(std::ostream & ostr)
    {
        ostr << "<thead>\n";
        ostr << "<tr>";
        for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
        {
            StreamHTML("th", mRecordSet.columnName(colIdx), HTMLFormatting_NoBreaks, ostr);
        }
        ostr << "</tr>";
        ostr << "</thead>\n";
    }


    void HTMLRenderer::render(std::ostream & ostr)
    {
        ostr << "<html>\n";
        ostr << "<body>\n";
        StreamHTML("h1", mCollectionTitle, HTMLFormatting_OneLiner, ostr);
        ostr << "<table>\n";
        renderTHead(ostr);
        renderTBody(ostr);
        ostr << "</table>\n";
        ostr << "</body>\n";
        ostr << "</html>\n";
    }


    PlainTextRenderer::PlainTextRenderer(const std::string & inCollectionTitle,
                                         const std::string & inRecordTitle,
                                         Poco::Data::RecordSet & inRecordSet) :
        Renderer(inCollectionTitle, inRecordTitle, inRecordSet)
    {
    }


    void PlainTextRenderer::getColumnWidths(std::vector<int> & outColWidths)
    {
        outColWidths.resize(mRecordSet.columnCount(), 0);

        for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
        {
            std::string name = mRecordSet.columnName(colIdx);
            outColWidths[colIdx] = name.size();
        }

        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
            {
                std::string value = getValue(colIdx, rowIdx);
                outColWidths[colIdx] = std::max<int>(outColWidths[colIdx], value.size());
            }
        }
    }


    std::string PlainTextRenderer::getValue(size_t colIdx, size_t rowIdx)
    {
        static const bool sFormatTimeStamps(true);

        if (sFormatTimeStamps)
        {
            std::string value = mRecordSet.value(colIdx, rowIdx).convert<std::string>();
            std::string name = mRecordSet.columnName(colIdx);
            if (MakeLowerCase(name).find("time") != std::string::npos)
            {
                std::string asTime = FormatTime(value);
                if (!asTime.empty())
                {
                    value = asTime;
                }
            }
            return value;
        }
        else
        {
            return mRecordSet.value(colIdx, rowIdx).convert<std::string>();
        }
    }


    void PlainTextRenderer::renderHeading(std::ostream & ostr)
    {
        for (size_t idx = 0; idx < mCollectionTitle.size() + 4; ++idx)
        {
            ostr << "*";
        }
        ostr << std::endl << "* " << mCollectionTitle << " *" << std::endl;
        for (size_t idx = 0; idx < mCollectionTitle.size() + 4; ++idx)
        {
            ostr << "*";
        }
        ostr << std::endl;
    }


    void PlainTextRenderer::repeat(char c, int n, std::ostream & ostr)
    {
        for (int idx = 0; idx < n; ++idx)
        {
            ostr << c;
        }
    }


    void PlainTextRenderer::render(std::ostream & ostr)
    {
        static const char * cSeparator = "\t";
        // Print the heading
        renderHeading(ostr);
        ostr << std::endl;

        // Get the column widths
        std::vector<int> columnWidths;
        getColumnWidths(columnWidths);

        for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
        {
            std::string colName = mRecordSet.columnName(colIdx);
            repeat(' ', columnWidths[colIdx] - static_cast<int>(colName.size()), ostr);
            ostr << colName;

            if (colIdx + 1 != mRecordSet.columnCount())
            {
                ostr << cSeparator; // separator
            }
        }

        ostr << std::endl;

        for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
        {
            std::string colName = mRecordSet.columnName(colIdx);
            repeat(' ', columnWidths[colIdx] - static_cast<int>(colName.size()), ostr);
            repeat('-', colName.size(), ostr);

            if (colIdx + 1 != mRecordSet.columnCount())
            {
                ostr << cSeparator; // separator
            }
        }

        ostr << std::endl;

        // Print as table
        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
            {
                std::string value = getValue(colIdx, rowIdx);
                repeat(' ', columnWidths[colIdx] - static_cast<int>(value.size()), ostr);
                ostr << value;

                if (colIdx + 1 != mRecordSet.columnCount())
                {
                    ostr << cSeparator; // separator
                }
            }
            ostr << std::endl;
        }
    }

} // namespace HSServer
