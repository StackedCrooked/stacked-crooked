#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include "Poco/Data/RecordSet.h"
#include <string>
#include <vector>


namespace HSServer
{

    class Renderer
    {
    public:
        Renderer(const std::string & inCollectionTitle,
                 const Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream) = 0;

    protected:
        std::string mCollectionTitle;
        Poco::Data::RecordSet mRecordSet;
    };

    class XMLRenderer : public Renderer
    {
    public:
        XMLRenderer(const std::string & inCollectionTitle,
                    const std::string & inRecordTitle,
                    Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream);

    private:
        std::string mRecordTitle;
    };

    class HTMLRenderer : public Renderer
    {
    public:
        HTMLRenderer(const std::string & inCollectionTitle,
                     Poco::Data::RecordSet & inRecordSet);

        virtual void render(std::ostream & outStream);

    private:
        void renderColumn(size_t inRowIdx, size_t inColIdx, std::ostream & ostr);
        void renderColumns(size_t inRowIdx, std::ostream & ostr);
        void renderRow(size_t inRowIdx, std::ostream & ostr);
        void renderRows(std::ostream & ostr);
        void renderHead(std::ostream & ostr);
        void renderBody(std::ostream & ostr);
    };


} // HighScoreServer


#endif // RENDERER_H_INCLUDED
