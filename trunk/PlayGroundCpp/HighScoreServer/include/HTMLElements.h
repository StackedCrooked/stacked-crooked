#ifndef HTMLELEMENTS_H_INCLUDED
#define HTMLELEMENTS_H_INCLUDED


#include <iostream>
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

    void Write(const std::string & inText);

    std::string Whitespace(size_t n);

    // Requires that a CurrentOutStream object is currently existing.
    class HTMLElement
    {
    public:
        
        HTMLElement(const std::string & inTagName);

        HTMLElement(const std::string & inTagName, const std::string & inText);

        virtual ~HTMLElement();

    private:

        std::string openTag();
        std::string closeTag();
        std::string selfClosingTag();
        std::string completeNode();

        static std::string OpenTag(const std::string & inTagName);
        static std::string CloseTag(const std::string & inTagName);
        static std::string OpenCloseTag(const std::string & inTagName);
        static std::string Surround(const std::string & inTagName, const std::string & inText);

        std::string mTagName;
        std::string mText;
        std::ostream & mOutStream;
    };


    #define DECLARE_HTML_ELEMENT(ELEMENTNAME, TAGNAME) \
        class ELEMENTNAME : public HTMLElement                                                                  \
        {                                                                                                       \
        public:                                                                                                 \
            ELEMENTNAME() : HTMLElement(TAGNAME) {}                                                             \
            ELEMENTNAME(const std::string & inText) : HTMLElement(TAGNAME, inText) {}                           \
        };


    DECLARE_HTML_ELEMENT(Html, "html");
    DECLARE_HTML_ELEMENT(Head, "head");
    DECLARE_HTML_ELEMENT(Body, "body");
    DECLARE_HTML_ELEMENT(H1, "h1");
    DECLARE_HTML_ELEMENT(H2, "h2");
    DECLARE_HTML_ELEMENT(H3, "h3");
    DECLARE_HTML_ELEMENT(H4, "h4");
    DECLARE_HTML_ELEMENT(P, "p");
    DECLARE_HTML_ELEMENT(B, "b");
    DECLARE_HTML_ELEMENT(I, "i");
    DECLARE_HTML_ELEMENT(U, "u");
    DECLARE_HTML_ELEMENT(Blockquote, "blockquote");
    DECLARE_HTML_ELEMENT(Br, "br");
    DECLARE_HTML_ELEMENT(Table, "table");
    DECLARE_HTML_ELEMENT(Tr, "tr");
    DECLARE_HTML_ELEMENT(Td, "td");

} // HTML


#endif // HTMLELEMENTS_H_INCLUDED
