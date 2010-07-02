#include "Renderer.h"
#include "Utils.h"
#include <boost/bind.hpp>


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
                std::string name = mRecordSet.columnName(colIdx);                
                
                // I choose to make all XML attribute names lower-case
                MakeLowerCase(name);

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
        ostr << "</body>\n";
        ostr << "</table>\n";
        ostr << "</html>\n";
    }


    PlainTextRenderer::PlainTextRenderer(const std::string & inCollectionTitle,
                                         const std::string & inRecordTitle,
                                         Poco::Data::RecordSet & inRecordSet) :
        Renderer(inCollectionTitle, inRecordTitle, inRecordSet)
    {
    }


    void PlainTextRenderer::render(std::ostream & ostr)
    {
        ostr << mCollectionTitle << std::endl;
        for (size_t idx = 0; idx < mCollectionTitle.size(); ++idx)
        {
            ostr << "-";
        }
        ostr << std::endl;
        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
            {
                std::string name = mRecordSet.columnName(colIdx);
                std::string value = mRecordSet.value(colIdx, rowIdx).convert<std::string>();
                ostr << name << ": " << value << std::endl;
            }
            ostr << std::endl;
        }
    }

} // namespace HSServer
