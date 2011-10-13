#ifndef GST_THREADING_H_INCLUDED
#define GST_THREADING_H_INCLUDED


#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <memory>


namespace Gst
{

    template<class Variable>
    struct WithMutex
    {
        WithMutex(std::auto_ptr<Variable> inVariable) :
            mVariable(inVariable.release())
        {
        }

        boost::scoped_ptr<Variable> mVariable;
        mutable boost::shared_mutex mMutex;
    };

    // Forward declaration.
    template<class Variable>
    class ScopedReader;

    // Forward declaration.
    template<class Variable>
    class ScopedReaderAndWriter;


    template<class Variable>
    class Protected
    {
    public:
        Protected(std::auto_ptr<Variable> inVariable) :
            mVariableWithMutex(new WithMutex<Variable>(inVariable))
        {
        }

        // Default constructor can only be used if Variable has a default constructor.
        Protected() :
            mVariableWithMutex(new WithMutex<Variable>(std::auto_ptr<Variable>(new Variable)))
        {
        }

        boost::timed_mutex & getMutex() const
        { return mVariableWithMutex->mMutex; }

    private:
        friend class ScopedReaderAndWriter<Variable>;
        friend class ScopedReader<Variable>;
        boost::shared_ptr<WithMutex<Variable> > mVariableWithMutex;
    };


    template<class Variable>
    class ScopedReaderAndWriter
    {
    public:
        ScopedReaderAndWriter(Protected<Variable> & inProtectedVariable) :
            mSharedLock(inProtectedVariable.mVariableWithMutex->mMutex),
            mUpgradeLock(mSharedLock),
            mVariable(inProtectedVariable.mVariableWithMutex->mVariable.get())
        {

        }

        ~ScopedReaderAndWriter()
        {
        }

        Variable * get()
        { return mVariable; }

        Variable * operator->()
        { return mVariable; }
    private:
        ScopedReaderAndWriter(const ScopedReaderAndWriter&);
        ScopedReaderAndWriter& operator=(const ScopedReaderAndWriter&);

        boost::upgrade_lock<boost::shared_mutex> mSharedLock;
        boost::upgrade_to_unique_lock<boost::shared_mutex> mUpgradeLock;

        Variable * mVariable;
    };


    template<class Variable>
    class ScopedReader
    {
    public:
        ScopedReader(const Protected<Variable> & inProtectedVariable) :
            mSharedLock(inProtectedVariable.mVariableWithMutex->mMutex),
            mVariable(inProtectedVariable.mVariableWithMutex->mVariable.get())
        {
        }

        ~ScopedReader()
        {
        }

        const Variable * get() const
        { return mVariable; }

        const Variable * operator->() const
        { return mVariable; }
    private:
        ScopedReader(const ScopedReader&);
        ScopedReader& operator=(const ScopedReader&);

        boost::upgrade_lock<boost::shared_mutex> mSharedLock;
        const Variable * mVariable;
    };

} // namespace Gst


#endif // GST_THREADING_H_INCLUDED
