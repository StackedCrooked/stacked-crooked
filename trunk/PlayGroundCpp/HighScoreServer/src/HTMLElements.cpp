#include "HTMLElements.h"
#include <assert.h>


namespace HTML
{

    static int sIndent(0);
    

    std::stack<CurrentOutStream *> CurrentOutStream::sInstanceStack;
    HTMLElement * HTMLElement::sActiveInstance(0);


    CurrentOutStream::CurrentOutStream(std::ostream & outStream) :
        mOutStream(outStream)
    {
        sInstanceStack.push(this);
    }


    CurrentOutStream::~CurrentOutStream()
    {
        sInstanceStack.pop();
    }


    std::ostream & CurrentOutStream::CurrentlyActive()
    {
        if (sInstanceStack.empty())
        {
            throw std::runtime_error("There is no currenly active HTML output stream.");
        }
        return sInstanceStack.top()->mOutStream;
    }


    void Write(const std::string & inText)
    {
        CurrentOutStream::CurrentlyActive() << inText;
    }


    std::string Whitespace(size_t n)
    {
        std::string result;
        for (size_t idx = 0; idx != cIndent*n; ++idx)
        {
            result += " ";
        }
        return result;
    }


    HTMLElement::HTMLElement(const std::string & inTagName, ElementType inElementType) :
        mTagName(inTagName),
        mElementType(inElementType),
        mOutStream(CurrentOutStream::CurrentlyActive()),
        mClosed(false),
        mPrevInstance(sActiveInstance)
    {
        sActiveInstance = this;
    }


    HTMLElement::~HTMLElement()
    {
        mClosed = true;
        sActiveInstance = mPrevInstance;
    }


    bool HTMLElement::isClosed() const
    {
        return mClosed;
    }


    ElementType HTMLElement::elementType() const
    {
        return mElementType;
    }

    std::string HTMLElement::openingTag()
    {
        return OpeningTag(mTagName);
    }


    std::string HTMLElement::closingTag()
    {
        return ClosingTag(mTagName);
    }

    
    std::string HTMLElement::OpeningTag(const std::string & inTagName)
    {
        return "<" + inTagName + ">";
    }


    std::string HTMLElement::ClosingTag(const std::string & inTagName)
    {
        return "</" + inTagName + ">";
    }


    std::string HTMLElement::SelfClosingTag(const std::string & inTagName)
    {
        return "<" + inTagName + "/>";
    }


    std::string HTMLElement::CompleteNode(const std::string & inTagName, const std::string & inText)
    {
        return OpeningTag(inTagName) + inText + ClosingTag(inTagName);
    }


    HTMLBlockElement::HTMLBlockElement(const std::string & inTagName) :
        HTMLElement(inTagName, ElementType_Block)
    {   
        mOutStream << Whitespace(sIndent) << openingTag() << "\n";
        sIndent++;
    }

    
    HTMLBlockElement::~HTMLBlockElement()
    {
        sIndent--;
        mOutStream << Whitespace(sIndent) << closingTag() << "\n";
    }


    HTMLInlineElement::HTMLInlineElement(const std::string & inTagName, const std::string & inText) :
        HTMLElement(inTagName, ElementType_Inline)
    {
        if (mPrevInstance)
        {
            if (mPrevInstance->elementType() == ElementType_Block || mPrevInstance->isClosed())
            {
                mOutStream << Whitespace(sIndent);
            }
        }

        mOutStream << CompleteNode(inTagName, inText);
        mClosed = true;
        if (mPrevInstance->elementType() != ElementType_Inline || mPrevInstance->isClosed())
        {
            mOutStream << "\n";
        }
    }


    HTMLInlineElement::HTMLInlineElement(const std::string & inTagName) :
        HTMLElement(inTagName, ElementType_Inline)
    {
        if (mPrevInstance)
        {
            if (mPrevInstance->elementType() == ElementType_Block || mPrevInstance->isClosed())
            {
                mOutStream << Whitespace(sIndent);
            }
        }
        mOutStream << openingTag();
    }


    HTMLInlineElement::~HTMLInlineElement()
    {  
        if (!mClosed)
        {
            mOutStream << closingTag();
            mClosed = true;            
            if (mPrevInstance->elementType() != ElementType_Inline || mPrevInstance->isClosed())
            {
                mOutStream << "\n";
            }
        }
    }


    HTMLSelfClosingElement::HTMLSelfClosingElement(const std::string & inTagName) :
        HTMLElement(inTagName, ElementType_SelfClosing)
    {
        if (mPrevInstance && mPrevInstance->isClosed())
        {
            mOutStream << Whitespace(sIndent);
        }
        mOutStream << SelfClosingTag(mTagName);
    }


    HTMLSelfClosingElement::~HTMLSelfClosingElement()
    {
    }

} // namespace HSServer
