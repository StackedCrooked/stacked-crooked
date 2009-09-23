#include "Parser.h"
#include "ElementFactory.h"
#include "Utils/ErrorReporter.h"


using namespace Utils;
using namespace Poco;
using namespace Poco::XML;


namespace XULWin
{

    Parser::Parser() :
        mIgnores(0),
        mLexicalHandler(0)
    {
        setContentHandler(this);
    }
    
    
    ElementPtr Parser::rootElement() const
    {
        return mRootElement;
    }


    void Parser::setDocumentLocator(const Poco::XML::Locator * inLocator)
    {
        mLocator = inLocator;
        if (const Poco::XML::ParserEngine * parserEngine = dynamic_cast<const Poco::XML::ParserEngine*>(inLocator))
        {
            mLexicalHandler = parserEngine->getLexicalHandler();
            const_cast<Poco::XML::ParserEngine*>(parserEngine)->setLexicalHandler(this);
        }
    }


    void Parser::startDocument()
    {
        assert(mStack.empty());
        assert(mIgnores == 0);
    }


    void Parser::endDocument()
    {
        assert(mStack.empty());
        assert(mIgnores == 0);
    }


    void Parser::startElement(const Poco::XML::XMLString& uri,
                              const Poco::XML::XMLString& localName,
                              const Poco::XML::XMLString& qname,
                              const Poco::XML::Attributes& attributes)
    {
        ErrorCatcher errorCatcher;
        try
        {
            if (mIgnores == 0)
            {
                //
                // Get parent
                //
                Element * parent(0);
                if (!mStack.empty())
                {
                    parent = mStack.top();
                }

                //
                // Get attributes
                //
                XULWin::AttributesMapping attr;
                for (int idx = 0; idx != attributes.getLength(); ++idx)
                {
                    const Poco::XML::XMLString & name = attributes.getLocalName(idx);
                    const Poco::XML::XMLString & value = attributes.getValue(idx);
                    attr[name] = value;
                }

                //
                // Create the element
                //
                ElementPtr element = ElementFactory::Instance().createElement(localName, parent, attr);
                if (element)
                {
                    if (mStack.empty())
                    {
                        assert(!mRootElement);
                        mRootElement = element;
                    }
                    mStack.push(element.get());
                }
                else
                {
                    mIgnores++;
                    ReportError("Element is null and will be ignored.");
                    return;
                }
            }
            else
            {
                mIgnores++;
            }
        }
        catch (const Poco::Exception & inExc)
        {
            ReportError(inExc.displayText());
        }
    }


    void Parser::endElement(const Poco::XML::XMLString& uri,
                            const Poco::XML::XMLString& localName,
                            const Poco::XML::XMLString& qname)
    {
        if (mIgnores == 0)
        {
            mStack.pop();
        }
        else
        {
            mIgnores--;
        }
    }


    void Parser::startDTD(const XMLString& name, const XMLString& publicId, const XMLString& systemId)
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->startDTD(name, publicId, systemId);
        }
    }


    void Parser::endDTD()
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->endDTD();
        }
    }


    void Parser::startEntity(const XMLString& name)
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->startEntity(name);
        }
    }


    void Parser::endEntity(const XMLString& name)
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->endEntity(name);
        }
    }


    void Parser::startCDATA()
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->startCDATA();
        }
    }


    void Parser::endCDATA()
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->endCDATA();
        }
    }


    void Parser::comment(const XMLChar ch[], int start, int length)
    {
        if (mLexicalHandler)
        {
            mLexicalHandler->comment(ch, start, length);
        }
    }




} // namespace XULWin
