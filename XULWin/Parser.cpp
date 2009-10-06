#include "Parser.h"
#include "ElementFactory.h"
#include "ChromeURL.h"
#include "Defaults.h"
#include "Utils/ErrorReporter.h"
#include "Poco/SAX/Attributes.h"
#include "Poco/SAX/EntityResolverImpl.h"


using namespace Utils;
using namespace Poco;
using namespace Poco::XML;


namespace XULWin
{

    Parser::Parser() :
        mIgnores(0),
        mLanguage("en")
    {
        setFeature(FEATURE_EXTERNAL_GENERAL_ENTITIES, true);
        setFeature(FEATURE_EXTERNAL_PARAMETER_ENTITIES, true);
        
        setContentHandler(this);
        setEntityResolver(this);
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
            mStack.top()->init();
            mStack.pop();
        }
        else
        {
            mIgnores--;
        }
    }
    
    
    void Parser::characters(const Poco::XML::XMLChar ch[], int start, int length)
    {
        if (!mStack.empty())
        {
            std::string innerText = std::string(ch + start, length);
            mStack.top()->setInnerText(mStack.top()->innerText() + innerText);
        }
    }


    InputSource* Parser::resolveEntity(const XMLString* publicId, const XMLString& systemId)
    {
        size_t idx = systemId.find("chrome://");
        if (idx != std::string::npos)
        {
            ChromeURL url(systemId, Defaults::locale());
            std::string path = url.convertToLocalPath();
            EntityResolverImpl entityResolverImpl;
            return entityResolverImpl.resolveEntity(publicId, path);
        }
        return 0;
    }


    void Parser::releaseInputSource(InputSource* pSource)
    {
        EntityResolverImpl entityResolverImpl;
        return entityResolverImpl.releaseInputSource(pSource);
    }


} // namespace XULWin
