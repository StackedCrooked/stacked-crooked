#include "HTMLElements.h"
#include "Poco/String.h"
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


    std::string GenWhiteSpace(size_t inIndentCount)
    {
        std::string result;
        for (size_t idx = 0; idx != cIndent*inIndentCount; ++idx)
        {
            result += " ";
        }
        return result;
    }


    Element::Element(const std::string & inTagName, ElementType inElementType, const Args & inArgs) :
        mTagName(inTagName),
        mElementType(inElementType),
        mArgs(inArgs),
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
        return OpeningTag(mTagName, mArgs);
    }


    std::string Element::closingTag()
    {
        return ClosingTag(mTagName);
    }


    std::string BuildArgList(const Args & inArgs)
    {
        std::string result;
        Args::const_iterator it = inArgs.begin(), end = inArgs.end();
        for (; it != end; ++it)
        {
            result += it->first + "=\"" + it->second + "\"";
        }
        return result;
    }

    
    std::string Element::OpeningTag(const std::string & inTagName, const Args & inArgs)
    {
        return "<" + inTagName
                   + (inArgs.empty() ? "" : (" " + BuildArgList(inArgs)))
                   + ">";
    }


    std::string Element::ClosingTag(const std::string & inTagName)
    {
        return "</" + inTagName + ">";
    }


    std::string Element::SelfClosingTag(const std::string & inTagName, const Args & inArgs)
    {
        return "<" + inTagName
                   + (inArgs.empty() ? "" : (" " + BuildArgList(inArgs)))
                   + "/>";
    }


    std::string Element::CompleteNode(const std::string & inTagName, const Args & inArgs, const std::string & inText)
    {
        return OpeningTag(inTagName, inArgs) + inText + ClosingTag(inTagName);
    }


    Block::Block(const std::string & inTagName, const Args & inArgs) :
        Element(inTagName, ElementType_Block, inArgs)
    {   
        mOutStream << GenWhiteSpace(sIndent) << openingTag() << "\n";
        sIndent++;
    }

    
    Block::~Block()
    {
        sIndent--;
        mOutStream << GenWhiteSpace(sIndent) << closingTag() << "\n";
    }


    Inline::Inline(const std::string & inTagName, const std::string & inText, const Args & inArgs) :
        Element(inTagName, ElementType_Inline, inArgs)
    {
        if (mPrevInstance)
        {
            if (mPrevInstance->elementType() == ElementType_Block || mPrevInstance->isClosed())
            {
                mOutStream << GenWhiteSpace(sIndent);
            }
        }

        mOutStream << CompleteNode(inTagName, mArgs, inText);
        mClosed = true;
        if (mPrevInstance->elementType() != ElementType_Inline || mPrevInstance->isClosed())
        {
            mOutStream << "\n";
        }
    }


    Inline::Inline(const std::string & inTagName, const Args & inArgs) :
        Element(inTagName, ElementType_Inline, inArgs)
    {
        if (mPrevInstance)
        {
            if (mPrevInstance->elementType() == ElementType_Block || mPrevInstance->isClosed())
            {
                mOutStream << GenWhiteSpace(sIndent);
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


    SelfClosing::SelfClosing(const std::string & inTagName, const Args & inArgs) :
        Element(inTagName, ElementType_SelfClosing, inArgs)
    {
        if (mPrevInstance && mPrevInstance->isClosed())
        {
            mOutStream << GenWhiteSpace(sIndent);
        }
        mOutStream << SelfClosingTag(mTagName, inArgs);
    }


    SelfClosing::~SelfClosing()
    {
    }


} // namespace HSServer
