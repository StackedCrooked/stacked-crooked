#include "Logger.h"
#include <iostream>
#include <mutex>


Log::~Log()
{
    static std::mutex mutex;
    auto s = str();
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << s;
}
