#include "Renderer.h"
#include "Utils.h"
#include <boost/bind.hpp>


using namespace Poco::Data;


namespace HSServer
{

    Renderer::Renderer(const std::string & inCollectionTitle,
                       const Poco::Data::RecordSet & inRecordSet) :
        mCollectionTitle(inCollectionTitle),
        mRecordSet(inRecordSet)
    {
    }


    XMLRenderer::XMLRenderer(const std::string & inCollectionTitle,
                             const std::string & inRecordTitle,
                             Poco::Data::RecordSet & inRecordSet) :
        Renderer(inCollectionTitle, inRecordSet),
        mRecordTitle(inRecordTitle)
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
                std::string value = mRecordSet.value(colIdx, rowIdx).convert<std::string>();
                ostr << " " << name << "=\"" << URIEncode(value) << "\"";
            }
            ostr << "/>\n";
        }
        ostr << "</" << mCollectionTitle << ">\n";
    }


    HTMLRenderer::HTMLRenderer(const std::string & inCollectionTitle,
                               Poco::Data::RecordSet & inRecordSet) :
        Renderer(inCollectionTitle, inRecordSet)
    {
    }


    void HTMLRenderer::renderColumn(size_t inRowIdx, size_t inColIdx, std::ostream & ostr)
    {
        StreamHTML("td", mRecordSet.value(inColIdx, inRowIdx).convert<std::string>(), ostr);
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
        StreamHTML("tr", boost::bind(&HTMLRenderer::renderColumns, this, inRowIdx, _1), ostr);
    }
    
    
    void HTMLRenderer::renderRows(std::ostream & ostr)
    {
        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            renderRow(rowIdx, ostr);
        }
    }


    void HTMLRenderer::renderTable(std::ostream & ostr)
    {
        StreamHTML("table", boost::bind(&HTMLRenderer::renderRows, this, _1), ostr);
    }


    void HTMLRenderer::render(std::ostream & ostr)
    {
        StreamHTML("html", boost::bind(&HTMLRenderer::renderTable, this, _1), ostr);
    }

} // namespace HSServer
