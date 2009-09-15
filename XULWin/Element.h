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
		class Type;

		class ID;

		/**
		 * Elements with no parent element are top-level elements.
		 */
		Element(const Type & inType, const ID & inID);

		Element(Element * inParent, const Type & inType, const ID & inID);

		const Type & type() const;

		const ID & id() const;

		class Type
		{
		public:
			explicit Type(const std::string & inType) : mType(inType) {}

			operator const std::string & () const { return mType; }

			bool operator < (const Type & rhs) const { return this->mType < rhs.mType; }
		private:
			std::string mType;
		};

		class ID
		{
		public:
			explicit ID(const std::string & inID) : mID(inID) {}

			operator const std::string & () const { return mID; }

			bool operator < (const ID & rhs) const { return this->mID < rhs.mID; }
		private:
			std::string mID;
		};

	private:
		Element * mParent;
		std::set<Element*> mChildren;
		Type mType;
		ID mID;
	};


} // XULWin


#endif // ELEMENT_H_INCLUDED
