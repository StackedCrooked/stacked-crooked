#include "Renderer.h"
#include "Utils.h"


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


    void HTMLRenderer::renderTable(std::ostream & outRow)
    {
    }
    
    
    void HTMLRenderer::renderRow(size_t inRowIdx, std::ostream & ostr)
    {
        ostr << "<tr>";
        for (size_t colIdx = 0; colIdx != mRecordSet.columnCount(); ++colIdx)
        {
            ostr << "<td>";
            ostr << mRecordSet.value(colIdx, inRowIdx).convert<std::string>();            
            ostr << "</td>";
        }
        
        ostr << "</tr>\n";
    }


    void HTMLRenderer::render(std::ostream & ostr)
    {
        ostr << "<html>\n";
        ostr << WrapHTML("h1", mCollectionTitle);
        ostr << "<table>";
        for (size_t rowIdx = 0; rowIdx != mRecordSet.rowCount(); ++rowIdx)
        {
            renderRow(rowIdx, ostr);
        }
        ostr << "</table>";
        ostr << "</html>";
    }

} // namespace HSServer
