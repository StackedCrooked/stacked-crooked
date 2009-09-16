#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED


#include <set>
#include <string>
#include <windows.h>


namespace XULWin
{

	class ElementFactory;
	class NativeComponent;

	/**
	 * Represents a XUL element.
	 */
	class Element
	{
	public:
		class Type;
		class ID;

		Element(Element * inParent, const Type & inType, const ID & inID, NativeComponent * inNativeWindow);

		const Type & type() const;

		const ID & id() const;

		NativeComponent * window() const;


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
		friend class ElementFactory;

		void add(Element * inChild);

		Element * mParent;
		Type mType;
		ID mID;
		NativeComponent * mNativeWindow;
		std::set<Element*> mChildren;
	};


	class Window : public Element
	{
	public:
		Window(const ID & inID);
	};


	class Button : public Element
	{
	public:
		Button(Element * inParent, const ID & inID);
	};


} // XULWin


#endif // ELEMENT_H_INCLUDED
