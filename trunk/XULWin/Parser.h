#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED


#include "Element.h"
#include "Poco/SAX/SAXParser.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/SAX/Attributes.h"
#include "Poco/SAX/Locator.h"
#include "Poco/Exception.h"
#include <stack>
#include <string>


namespace XULWin
{
    
    class Parser : public Poco::XML::SAXParser,
                   public Poco::XML::ContentHandler
    {
    public:
        Parser();

        // after parsing
        ElementPtr rootElement() const;
	
	    // ContentHandler
        virtual void setDocumentLocator(const Poco::XML::Locator * inLocator);
    	
	    virtual void startDocument();
    	
	    virtual void endDocument();
    	
	    virtual void startElement(const Poco::XML::XMLString& uri,
                                  const Poco::XML::XMLString& localName,
                                  const Poco::XML::XMLString& qname,
                                  const Poco::XML::Attributes& attributes);
    	
	    virtual void endElement(const Poco::XML::XMLString& uri,
                                const Poco::XML::XMLString& localName,
                                const Poco::XML::XMLString& qname);

        virtual void characters(const Poco::XML::XMLChar ch[], int start, int length) {}
	
	    virtual void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length) {}
	
	    virtual void processingInstruction(const Poco::XML::XMLString& target, const Poco::XML::XMLString& data) {}
	
	    virtual void startPrefixMapping(const Poco::XML::XMLString& prefix, const Poco::XML::XMLString& uri) {}
	
        virtual void endPrefixMapping(const Poco::XML::XMLString& prefix) {}
	
        virtual void skippedEntity(const Poco::XML::XMLString& name) {}
	
    private:
        const Poco::XML::Locator* mLocator;
        
        // a stack is needed for storing the elements
        // that we are currently dealing with
        std::stack<ElementPtr> mStack; 
        
        // unknown elements are simply ignored
        // this stack helps to remember that we are
        // currently ignoring an item
        std::stack<bool> mIgnores;
        ElementPtr mRootElement;

    };

} // namespace XULWin


#endif // PARSER_H_INCLUDED
