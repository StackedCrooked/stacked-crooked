#ifndef SCOPEDMUTER_H
#define SCOPEDMUTER_H


#include <ostream>


namespace Crooked {

	
class ScopedMuter
{
public:
	ScopedMuter(std::ostream & inOutStream);
	
	~ScopedMuter();
	
private:
	ScopedMuter(const ScopedMuter&);
	ScopedMuter& operator=(const ScopedMuter&);
	
	struct FakeSink;
	
	struct Impl;
	Impl * mImpl;
};


} // namespace Crooked


#endif // SCOPEDMUTER_H
