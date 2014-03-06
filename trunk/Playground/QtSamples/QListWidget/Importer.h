#ifndef MESMERIZE_IMAGEMESMERIZE_H
#define MESMERIZE_IMAGEMESMERIZE_H


#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>


namespace Mesmerize {


class RecursionLimitExceeded;


class Importer
{
public:
    //! In a background thread the Importer recursively searches for images in given path.
    Importer(const std::string & inPath);

    ~Importer();

    //! Moves imported paths out of the imported.
    std::vector<std::string> stealImported();

    bool isFinished() const;

    void cancel();

private:
    struct Impl;
    Impl * mImpl;
};





/**
 * DangerouslyDeepRecursion exception is thrown when the have
 * reached a recursion depth that is unreasonably high.
 *
 * This may occur when symbolic links result in circular hierarchies.
 */
class RecursionLimitExceeded : public std::runtime_error
{
public:
    enum
    {
        // We shouldn't be looking so deep.
        cMaxRecursion = 32
    };

    RecursionLimitExceeded(unsigned inDepth) :
        std::runtime_error("Dangerously deep recursion."),
        mDepth(inDepth)
    {
    }

    ~RecursionLimitExceeded() throw() {}

    unsigned depth() const { return mDepth;  }

private:
    const unsigned mDepth;
};


} // namespace Charm


#endif // MESMERIZE_IMAGEMESMERIZE_H
