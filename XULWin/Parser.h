#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED


#include "Element.h"
#include "Poco/SAX/SAXParser.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/SAX/Attributes.h"
#include "Poco/SAX/Locator.h"
#include "Poco/Exception.h"
#include <string>


namespace XULWin
{
    
    class Parser : public Poco::XML::ContentHandler
    {
    public:
        Parser(const std::string & inFileName);
	
	    // ContentHandler
        virtual void setDocumentLocator(const Poco::XML::Locator * inLocator);
    	
	    virtual void startDocument();
    	
	    virtual void endDocument();
    	
	    virtual void startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes);
    	
	    virtual void endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname);
	
    private:
        const Poco::XML::Locator* mLocator;
        ElementPtr mRootElement;
        ElementPtr mCurrentElement;

    };

} // namespace XULWin


#endif // PARSER_H_INCLUDED
