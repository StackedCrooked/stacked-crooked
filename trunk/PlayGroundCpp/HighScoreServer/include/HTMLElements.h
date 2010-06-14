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

    // Requires that a CurrentOutStream object is currently existing.
    void Write(const std::string & inText);

    // Requires that a CurrentOutStream object is currently existing.
    std::string Whitespace(size_t n);


    enum ElementType
    {
        ElementType_Block,
        ElementType_Inline,
        ElementType_SelfClosing,
    };


    // Requires that a CurrentOutStream object is currently existing.
    class HTMLElement
    {
    public:
        virtual ~HTMLElement();

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
        HTMLElement(const std::string & inTagName, ElementType inElementType);

        bool mClosed;
        std::string mTagName;
        ElementType mElementType;
        std::ostream & mOutStream;
        static HTMLElement * sActiveInstance;
        HTMLElement * mPrevInstance;
    };

    
    class HTMLBlockElement : public HTMLElement
    {
    public:
        // Contains child elements
        HTMLBlockElement(const std::string & inTagName);

        ~HTMLBlockElement();
    };

    
    class HTMLInlineElement : public HTMLElement
    {
    public:
        // Inline without line-breaks, may contain child elements.
        HTMLInlineElement(const std::string & inTagName);

        // Contains only text and is immediately closed.
        HTMLInlineElement(const std::string & inTagName, const std::string & inText);

        ~HTMLInlineElement();
    };


    class HTMLSelfClosingElement : public HTMLElement
    {
    public:
        // Contains nothing and is immediately closed.
        // For example: <br/>.
        HTMLSelfClosingElement(const std::string & inTagName);

        ~HTMLSelfClosingElement();
    };


    #define DECLARE_BLOCK_ELEMENT(CLASS, TAGNAME) \
        class POCO_JOIN(HTML_, CLASS) : public HTMLBlockElement { \
        public: \
            POCO_JOIN(HTML_, CLASS)(); \
        };


    #define IMPLEMENT_BLOCK_ELEMENT(CLASS, TAGNAME) \
        POCO_JOIN(HTML_, CLASS)::POCO_JOIN(HTML_, CLASS)() : HTMLBlockElement(TAGNAME) {} 


    #define DECLARE_INLINE_ELEMENT(CLASS, TAGNAME) \
        class POCO_JOIN(HTML_, CLASS) : public HTMLInlineElement { \
        public: \
            POCO_JOIN(HTML_, CLASS)(); \
            POCO_JOIN(HTML_, CLASS)(const std::string & inText); \
        };


    #define IMPLEMENT_INLINE_ELEMENT(CLASS, TAGNAME) \
        POCO_JOIN(HTML_, CLASS)::POCO_JOIN(HTML_, CLASS)() : HTMLInlineElement(TAGNAME) {} \
        POCO_JOIN(HTML_, CLASS)::POCO_JOIN(HTML_, CLASS)(const std::string & inText) : HTMLInlineElement(TAGNAME, inText) {}


    #define DECLARE_SELFCLOSING_ELEMENT(CLASS, TAGNAME) \
        class POCO_JOIN(HTML_, CLASS) : public HTMLSelfClosingElement { \
        public: \
            POCO_JOIN(HTML_, CLASS)(); \
        };

    #define IMPLEMENT_SELFCLOSING_ELEMENT(CLASS, TAGNAME) \
        POCO_JOIN(HTML_, CLASS)::POCO_JOIN(HTML_, CLASS)() : HTMLSelfClosingElement(TAGNAME) {}


    DECLARE_BLOCK_ELEMENT(blockquote, "blockquote")    
    DECLARE_BLOCK_ELEMENT(body, "body")
    DECLARE_BLOCK_ELEMENT(div, "div")
    DECLARE_BLOCK_ELEMENT(head, "head")
    DECLARE_BLOCK_ELEMENT(html, "html")
    DECLARE_BLOCK_ELEMENT(table, "table")
    DECLARE_BLOCK_ELEMENT(thead, "thead")
    DECLARE_BLOCK_ELEMENT(tr, "tr")
    DECLARE_BLOCK_ELEMENT(ul, "ul")

    DECLARE_INLINE_ELEMENT(a, "a")
    DECLARE_INLINE_ELEMENT(b, "b")
    DECLARE_INLINE_ELEMENT(h1, "h1")
    DECLARE_INLINE_ELEMENT(h2, "h2")
    DECLARE_INLINE_ELEMENT(h3, "h3")
    DECLARE_INLINE_ELEMENT(h4, "h4")
    DECLARE_INLINE_ELEMENT(h5, "h5")
    DECLARE_INLINE_ELEMENT(h6, "h6")
    DECLARE_INLINE_ELEMENT(i, "i")
    DECLARE_INLINE_ELEMENT(img, "img")
    DECLARE_INLINE_ELEMENT(li, "li")    
    DECLARE_INLINE_ELEMENT(nobr, "nobr")
    DECLARE_INLINE_ELEMENT(p, "p")
    DECLARE_INLINE_ELEMENT(pre, "pre")
    DECLARE_INLINE_ELEMENT(span, "span")    
    DECLARE_INLINE_ELEMENT(strong, "strong")
    DECLARE_INLINE_ELEMENT(td, "td")
    DECLARE_INLINE_ELEMENT(th, "th")
    DECLARE_INLINE_ELEMENT(title, "title")
    DECLARE_INLINE_ELEMENT(u, "u")

    DECLARE_SELFCLOSING_ELEMENT(br, "br")
    DECLARE_SELFCLOSING_ELEMENT(hr, "hr")
    DECLARE_SELFCLOSING_ELEMENT(meta, "meta")

} // HTML


#endif // HTMLELEMENTS_H_INCLUDED
