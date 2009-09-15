#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include <set>
#include <string>


namespace XULWin
{

	/**
	 * Represents a XUL element.
	 */
	class Element
	{
	public:
		/**
		 * Elements with no parent element are top-level elements.
		 */
		Element(const std::string & inType, const std::string & inId);

		Element(Element * inParent, const std::string & inType, const std::string & inId);

		const std::string & type() const;

		const std::string & id() const;

	private:
		Element * mParent;
		std::set<Element*> mChildren;
		std::string mType;
		std::string mId;
	};


} // XULWin


#endif // ELEMENT_H_INCLUDED
