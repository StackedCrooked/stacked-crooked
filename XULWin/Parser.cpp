#include "Parser.h"
#include "ElementFactory.h"
#include "Utils/ErrorReporter.h"


using namespace Utils;


namespace XULWin
{

    Parser::Parser()
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
    }


    void Parser::startDocument()
    {
        assert(mStack.empty());
        assert(mIgnores.empty());
    }


    void Parser::endDocument()
    {
        assert(mStack.empty());
        assert(mIgnores.empty());
    }


    void Parser::startElement(const Poco::XML::XMLString& uri,
                              const Poco::XML::XMLString& localName,
                              const Poco::XML::XMLString& qname,
                              const Poco::XML::Attributes& attributes)
    {
        try
        {
            if (mIgnores.empty())
            {
                //
                // Get parent
                //
                ElementPtr parent;
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
                    mStack.push(element);
                }
                else
                {
                    mIgnores.push(true);
                    ReportError("Element is null and will be ignored.");
                    return;
                }
            }
            else
            {
                mIgnores.push(true);
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
        if (mIgnores.empty())
        {
            mStack.pop();
        }
        else
        {
            mIgnores.pop();
        }
    }


} // namespace XULWin
