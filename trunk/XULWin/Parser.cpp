#include "Parser.h"
#include "ElementFactory.h"
#include "Utils/ErrorReporter.h"


using namespace Utils;


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
            ElementPtr element = ElementFactory::Instance().createElement(eltype(localName), parent, attr);
            if (element)
            {
                if (mStack.empty())
                {
                    assert(!mRootElement);
                    mRootElement = element;
                }
                element->onStart();
                mStack.push(element);
            }
            else
            {
                mIgnores.push(true);
                ReportError("Element is null => Ignore");
                errorCatcher.propagate();
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
                mStack.top()->onEnd();
                mStack.pop();
            }
        }
        else
        {
            mIgnores.pop();
        }
    }


} // namespace XULWin
