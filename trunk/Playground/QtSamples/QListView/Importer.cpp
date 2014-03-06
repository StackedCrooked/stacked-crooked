#include "Importer.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/File.h"
#include "Poco/Mutex.h"
#include "Poco/Stopwatch.h"
#include "Poco/Task.h"
#include "Poco/TaskManager.h"
#include <cassert>
#include <iostream>
#include <set>


namespace Mesmerize {
namespace {


struct ImportTask : Poco::Task
{
    typedef std::function<void(std::string)> Callback;
    typedef std::function<void()> FinishedCallback;

    ImportTask(const std::string & inDirectory,
               const Callback & inCallback,
               const FinishedCallback & inFinishedCallback) :
        Poco::Task("Import"),
        mDirectory(inDirectory),
        mCallback(inCallback),
        mFinishedCallback(inFinishedCallback)
    {
    }

    static bool IsSupportedExtension(const std::string & inExtension)
    {
        static const std::set<std::string> fSupportedExtensions = {{ "jpg", "jpeg", "png" }};
        return fSupportedExtensions.find(inExtension) != fSupportedExtensions.end();
    }

    void import(const std::string & path, unsigned recursion)
    {
        //
        // Protectect against too deep recursion.
        //
        if (recursion >= RecursionLimitExceeded::cMaxRecursion)
        {
            throw RecursionLimitExceeded(recursion);
        }

        Poco::File file(path);

        //
        // Skip hidden or unreadable files.
        //
        if (file.isHidden() || !file.canRead())
        {
            return;
        }

        //
        // Check if directory, if yes => recurse!
        //
        if (file.isDirectory())
        {
            // Recurse directories.
            for (Poco::DirectoryIterator it(path), end; it != end; ++it)
            {
                try
                {
                    import(it->path(), recursion + 1);
                }
                catch (const std::exception & exc)
                {
                    std::cerr << "Failed to import file: " << it->path() << ". Detail: " << exc.what() << std::endl;
                }
            }
            return;
        }

        //
        // Check if the file extension is supported (.jpg, .jpeg, etc...)
        //
        if (IsSupportedExtension(Poco::Path(path).getExtension()))
        {
            mCallback(file.path());
        }
    }

    virtual void runTask()
    {
        import(mDirectory, 0);
        mFinishedCallback();
    }

    const std::string mDirectory;
    Callback mCallback;
    FinishedCallback mFinishedCallback;
};


} // anonymous namespace


struct Importer::Impl
{
    Impl(const std::string & inDirectory) :
        mTaskManager(),
        mFinished(false)
    {
        mTaskManager.start(new ImportTask(inDirectory,
                                          std::bind(&Impl::onImported, this, std::placeholders::_1),
                                          std::bind(&Impl::onFinished, this)
                                          ));
    }

    ~Impl()
    {
        try
        {
            cancel();
        }
        catch (const std::exception & exc)
        {
            std::cerr << "Exception caught in Importer::~Impl. Detail: " << exc.what() << std::endl;
        }
    }

    void cancel()
    {
        mTaskManager.cancelAll();
        mTaskManager.joinAll();
    }

    void onImported(const std::string & inFile)
    {
        Poco::Mutex::ScopedLock lock(mMutex);
        mFiles.push_back(inFile);
    }

    void onFinished()
    {
        Poco::Mutex::ScopedLock lock(mMutex);
        mFinished = true;
    }

    std::vector<std::string> stealImported()
    {
        Poco::Mutex::ScopedLock lock(mMutex);
        std::vector<std::string> result = mFiles;
        mFiles.clear();
        return result;
    }

    bool isFinished() const
    {
        Poco::Mutex::ScopedLock lock(mMutex);
        return !mFinished;
    }

    Poco::TaskManager mTaskManager;

    mutable Poco::Mutex mMutex;
    std::vector<std::string> mFiles;
    bool mFinished;
};


Importer::Importer(const std::string & path) :
    mImpl(new Impl(path))
{
}


Importer::~Importer()
{
    delete mImpl;
}


std::vector<std::string> Importer::stealImported()
{
    return mImpl->stealImported();
}


bool Importer::isFinished() const
{
    return mImpl->isFinished();
}


void Importer::cancel()
{
    mImpl->cancel();
}


} // namespace Charm
