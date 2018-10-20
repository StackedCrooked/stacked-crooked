//#ifndef PCAPWRITER_H
//#define PCAPWRITER_H


//#include <fstream>
//#include <string>
//#include <stdint.h>
//#include <mutex>


//class PCAPWriter
//{
//public:
//    PCAPWriter(const std::string & inOutputFile);

//    void setCaputureLength(uint32_t inCapLen)
//    {
//        mCapLen = inCapLen;
//    }

//    void push_back(const uint8_t* bytes, uint32_t len);

//private:
//    void push_back(std::lock_guard<std::mutex>&, timeval tv, const uint8_t* bytes, uint32_t len);

//    std::mutex mMutex;
//    std::ofstream mOutput;
//    uint32_t mCapLen = 0;
//};


//#endif
