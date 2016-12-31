//#include "PCAPWriter.h"
//#include "pcap.h"


//PCAPWriter::PCAPWriter(const std::string& inOutputFile) :
//    mOutput(inOutputFile)
//{
//    pcap_file_header fileHeader;
//    fileHeader.magic = 0xa1b2c3d4;
//    fileHeader.version_major = PCAP_VERSION_MAJOR;
//    fileHeader.version_minor = PCAP_VERSION_MINOR;
//    fileHeader.thiszone = 0;
//    fileHeader.sigfigs = 0;
//    fileHeader.snaplen = 65535;
//    fileHeader.linktype = 1;
//    static_assert(sizeof(fileHeader) == 24, "");

//    mOutput.write(reinterpret_cast<char*>(&fileHeader) ,sizeof(fileHeader));
//    mOutput.flush();
//}


//void PCAPWriter::push_back(const uint8_t* bytes, uint32_t len)
//{
//    struct Header
//    {
//        uint32_t sec;
//        uint32_t usec;
//        uint32_t caplen;
//        uint32_t len;
//    };

//    auto caplen = mCapLen ? std::min(len, mCapLen) : len;

//    std::chrono::nanoseconds now_ns = std::chrono::system_clock::now().time_since_epoch();
//    auto now_s = std::chrono::duration_cast<std::chrono::seconds>(now_ns);
//    auto now_us = std::chrono::duration_cast<std::chrono::microseconds>(now_ns) - now_s;


//    auto header = Header();
//    header.sec = now_s.count();
//    header.usec = now_us.count();
//    header.caplen = caplen;
//    header.len = len;

//    std::lock_guard<std::mutex> lock(mMutex);
//    mOutput.write(static_cast<const char*>(static_cast<const void*>(&header)), sizeof(header));
//    mOutput.write(static_cast<const char*>(static_cast<const void*>(bytes)), caplen);
//    mOutput.flush();
//}
