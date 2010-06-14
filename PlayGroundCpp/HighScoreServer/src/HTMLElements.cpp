#include "HTMLElements.h"
#include <stdexcept>
#include <assert.h>


namespace HTML
{

    static int sIndent(0);
    

    std::stack<CurrentOutStream *> CurrentOutStream::sInstanceStack;
    Element * Element::sActiveInstance(0);


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


    Element::Element(const std::string & inTagName, ElementType inElementType) :
        mTagName(inTagName),
        mElementType(inElementType),
        mOutStream(CurrentOutStream::CurrentlyActive()),
        mClosed(false),
        mPrevInstance(sActiveInstance)
    {
        sActiveInstance = this;
    }


    Element::~Element()
    {
        mClosed = true;
        sActiveInstance = mPrevInstance;
    }


    bool Element::isClosed() const
    {
        return mClosed;
    }


    ElementType Element::elementType() const
    {
        return mElementType;
    }

    std::string Element::openingTag()
    {
        return OpeningTag(mTagName);
    }


    std::string Element::closingTag()
    {
        return ClosingTag(mTagName);
    }

    
    std::string Element::OpeningTag(const std::string & inTagName)
    {
        return "<" + inTagName + ">";
    }


    std::string Element::ClosingTag(const std::string & inTagName)
    {
        return "</" + inTagName + ">";
    }


    std::string Element::SelfClosingTag(const std::string & inTagName)
    {
        return "<" + inTagName + "/>";
    }


    std::string Element::CompleteNode(const std::string & inTagName, const std::string & inText)
    {
        return OpeningTag(inTagName) + inText + ClosingTag(inTagName);
    }


    Block::Block(const std::string & inTagName) :
        Element(inTagName, ElementType_Block)
    {   
        mOutStream << Whitespace(sIndent) << openingTag() << "\n";
        sIndent++;
    }

    
    Block::~Block()
    {
        sIndent--;
        mOutStream << Whitespace(sIndent) << closingTag() << "\n";
    }


    Inline::Inline(const std::string & inTagName, const std::string & inText) :
        Element(inTagName, ElementType_Inline)
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


    Inline::Inline(const std::string & inTagName) :
        Element(inTagName, ElementType_Inline)
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


    Inline::~Inline()
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


    SelfClosing::SelfClosing(const std::string & inTagName) :
        Element(inTagName, ElementType_SelfClosing)
    {
        if (mPrevInstance && mPrevInstance->isClosed())
        {
            mOutStream << Whitespace(sIndent);
        }
        mOutStream << SelfClosingTag(mTagName);
    }


    SelfClosing::~SelfClosing()
    {
    }


} // namespace HSServer
