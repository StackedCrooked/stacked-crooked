#include "Parser.h"
#include "ElementFactory.h"
#include "ErrorHandler/ErrorStack.h"


using namespace CppToys;


namespace XULWin
{

    Parser::Parser()
    {
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

    void Parser::startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes)
    {
        if (mIgnores.empty())
        {
            ErrorCatcher errorCatcher;
            ElementPtr parent;
            if (!mStack.empty())
            {
                parent = mStack.top();
            }
            ElementPtr element = ElementFactory::Instance().createElement(eltype(localName), parent);
            if (element)
            {
                if (mStack.empty())
                {
                    assert(!mRootElement);
                    mRootElement = element;
                }
                mStack.push(element);
                for (int idx = 0; idx != attributes.getLength(); ++idx)
                {
                    mStack.top()->Attributes[attributes.getLocalName(idx)] = attributes.getValue(idx);
                }
            }
            else
            {
                mIgnores.push(true);
                ThrowError("Null element created and ignored! Reason: " + errorCatcher.message());
                errorCatcher.rethrow();
                return;
            }
        }
        else
        {
            mIgnores.push(true);
        }
    }

    void Parser::endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname)
    {
        if (mIgnores.empty())
        {
            bool match = std::string(mStack.top()->type()) == localName;
            assert (match);
            if (match)
            {
                mStack.pop();
            }
        }
        else
        {
            mIgnores.pop();
        }
    }


} // namespace XULWin
