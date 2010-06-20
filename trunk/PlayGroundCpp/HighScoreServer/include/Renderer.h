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
        virtual void renderTable(std::ostream & outRow);
        void renderRow(size_t inRowIdx, std::ostream & outRow);
    };


} // HighScoreServer


#endif // RENDERER_H_INCLUDED
