#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include "Poco/Data/RecordSet.h"
#include "Poco/Data/SessionFactory.h"
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include <string>
#include <vector>


namespace HSServer
{

    class Renderer
    {
    public:
        Renderer(const std::string & inCollectionTitle,
                 const std::string & inRecordTitle,
                 const Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream) = 0;

    protected:
        std::string mCollectionTitle;
        std::string mRecordTitle;
        Poco::Data::RecordSet mRecordSet;
    };


    class XMLRenderer : public Renderer
    {
    public:
        XMLRenderer(const std::string & inCollectionTitle,
                    const std::string & inRecordTitle,
                    Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream);
    };


    class HTMLRenderer : public Renderer
    {
    public:
        HTMLRenderer(const std::string & inCollectionTitle,
                     const std::string & inRecordTitle,
                     Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream);

    private:
        void renderColumn(size_t inRowIdx, size_t inColIdx, std::ostream & ostr);
        void renderColumns(size_t inRowIdx, std::ostream & ostr);
        void renderRow(size_t inRowIdx, std::ostream & ostr);
        void renderRows(std::ostream & ostr);
        void renderTHead(std::ostream & ostr);
        void renderTBody(std::ostream & ostr);
    };


    class PlainTextRenderer : public Renderer
    {
    public:
        PlainTextRenderer(const std::string & inCollectionTitle,
                          const std::string & inRecordTitle,
                          Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream);

    private:
        void getColumnWidths(std::vector<int> & outColWidths);
        std::string getValue(size_t colIdx, size_t rowIdx);
        void renderHeading(std::ostream & ostr);
        void repeat(char c, int n, std::ostream & ostr);
    };


} // namespace HSServer


#endif // RENDERER_H_INCLUDED
