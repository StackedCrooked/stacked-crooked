#include "Parser.h"
#include "ElementFactory.h"


namespace XULWin
{

    void Parser::setDocumentLocator(const Poco::XML::Locator * inLocator)
    {
        mLocator = inLocator;
    }

    void Parser::startDocument()
    {
        assert(!mRootElement);        
    }

    void Parser::endDocument()
    {
        // ... ok
    }

    void Parser::startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes)
    {
        ElementPtr nullParent;
        ElementPtr element = ElementFactory::Instance().createElement(eltype(localName), nullParent);
        for (int idx = 0; idx != attributes.getLength(); ++idx)
        {
            element->Attributes[attributes.getLocalName(idx)] = attributes.getValue(idx);
        }

        if (!mRootElement)
        {
            assert(!mCurrentElement);
            mRootElement = element;
        }
        else
        {
            mCurrentElement = element;
        }
    }

    void Parser::endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname)
    {
        assert(mRootElement);
        if (mCurrentElement)
        {
            mRootElement->children().push_back(mCurrentElement);
            mCurrentElement.reset();
        }
    }


} // namespace XULWin
