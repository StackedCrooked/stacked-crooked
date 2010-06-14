#ifndef HTMLELEMENTS_H_INCLUDED
#define HTMLELEMENTS_H_INCLUDED


#include "Poco/Foundation.h"
#include <iostream>
#include <list>
#include <stack>
#include <string>


namespace HTML
{
    static const size_t cIndent = 2;

    /**
     * Provides a scope where a certain output stream is defined.
     * Stack-like behavior in case of multiple instances. The CurrentlyActive() object
     * always refers to the most recently added object on the application stack. Once
     * an object is destroyed CurrentlyActive() will be the previous instance, etc..
     */
    class CurrentOutStream
    {
    public:
        CurrentOutStream(std::ostream & outStream);

        ~CurrentOutStream();

        static std::ostream & CurrentlyActive();

    private:
        std::ostream & mOutStream;
        static std::stack<CurrentOutStream*> sInstanceStack;
    };


    /**
     * Requires that at least one CurrentOutStream object is currently existing.
     */
    void Write(const std::string & inText);


    /**
     * Generates whitespace according to specified number of indents.
     */
    std::string GenWhiteSpace(size_t inIdentCount);


    /**
     * Enum that identifies the Element subclass type.
     */
    enum ElementType
    {
        ElementType_Block,
        ElementType_Inline,
        ElementType_SelfClosing,
    };


    /**
     * Pushes a string to the CurrentOutStream instance.
     */
    class Element
    {
    public:
        virtual ~Element();

        // The current element does not yet have a closing tag.
        bool isClosed() const;

        ElementType elementType() const;

        std::string openingTag();
        std::string closingTag();

        static std::string OpeningTag(const std::string & inTagName);
        static std::string ClosingTag(const std::string & inTagName);
        static std::string SelfClosingTag(const std::string & inTagName);
        static std::string CompleteNode(const std::string & inTagName, const std::string & inText);

    protected:
        Element(const std::string & inTagName, ElementType inElementType);

        bool mClosed;
        std::string mTagName;
        ElementType mElementType;
        std::ostream & mOutStream;
        static Element * sActiveInstance;
        Element * mPrevInstance;
    };

    
    /**
     * Block creates an html element where child elements are added indented and on new line.
     */
    class Block : public Element
    {
    public:
        Block(const std::string & inTagName);

        ~Block();
    };

    
    /**
     * Inline creates a html element where sub elements are added on the same line.
     */
    class Inline : public Element
    {
    public:
        // Inline without line-breaks, may contain child elements.
        Inline(const std::string & inTagName);

        // Contains only text and is immediately closed.
        Inline(const std::string & inTagName, const std::string & inText);

        ~Inline();
    };


    /**
     * SelfClosing adds a closed html element.
     * For example: <br/> <hr/>
     */
    class SelfClosing : public Element
    {
    public:
        SelfClosing(const std::string & inTagName);

        ~SelfClosing();
    };

    // Create a local object of type HTML::Block with a unique variable name.
    #define HTML_Block(TagName) HTML::Block POCO_JOIN(html_, __LINE__)(TagName);

    // Create a local object of type HTML::Inline with a unique variable name.
    #define HTML_Inline(TagName, Text) HTML::Inline POCO_JOIN(html_, __LINE__)(TagName, Text);

    // Create a local object of type HTML::SelfClosing with a unique variable name.
    #define HTML_SelfClosing(TagName) HTML::SelfClosing POCO_JOIN(html_,__LINE__)(TagName);

} // HTML


#endif // HTMLELEMENTS_H_INCLUDED
