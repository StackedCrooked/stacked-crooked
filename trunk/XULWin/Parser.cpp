#include "Parser.h"
#include "ElementFactory.h"
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
            mStack.pop();
        }
        else
        {
            mIgnores--;
        }
    }


    InputSource* Parser::resolveEntity(const XMLString* publicId, const XMLString& systemId)
    {
        const std::string cChrome = "chrome://";
        const std::string cLocale = "locale";
        std::string::size_type chromeIdx = systemId.find(cChrome);
        if (chromeIdx == std::string::npos)
        {
            return 0;
        }
        std::string path = systemId;
        path.replace(chromeIdx, cChrome.size(), "chrome/");

        std::string::size_type localeIdx = path.find(cLocale);
        if (localeIdx != std::string::npos)
        {
            path.insert(localeIdx + cLocale.size(), "/" + mLanguage);
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
