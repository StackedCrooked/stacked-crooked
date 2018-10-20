#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <pcap.h>
#include <arpa/inet.h>



struct PCAPFileHeader
{
    uint32_t magic;
    uint16_t version_major; // 2
    uint16_t version_minor; // 4
    uint32_t thiszone;	/* gmt to local correction */
    uint32_t sigfigs;	/* accuracy of timestamps */
    uint32_t snaplen;	/* max length saved portion of each pkt */
    uint32_t linktype;	/* data link type (LINKTYPE_*) */
};



struct PCAPHeader
{
    uint32_t sec;
    uint32_t usec;
    uint32_t caplen;
    uint32_t len;
};


template<typename F>
void pcap_for_each(std::string pcap, F&& f)
{
    std::ifstream input(pcap);

    if (!input)
    {
        throw std::runtime_error("Failed to open " + pcap);
    }


    auto file_header = PCAPFileHeader();

    if (!input.read(reinterpret_cast<char*>(&file_header), sizeof(file_header)))
    {
        if (input.eof()) return;
        std::cout << "errno=" << errno << " strerror=" << strerror(errno) << std::endl;
        throw std::runtime_error("read failed.");
    }



//    std::cout << "fileHeader.magic=" << std::hex << std::setw(2) << std::setfill('0') << file_header.magic << std::dec << std::endl;
//    std::cout << "fileHeader.version_major=" << file_header.version_major << std::endl;
//    std::cout << "fileHeader.version_minor=" << file_header.version_minor << std::endl;
//    std::cout << "fileHeader.thiszone=" << file_header.thiszone << std::endl;
//    std::cout << "fileHeader.sigfigs=" << file_header.sigfigs << std::endl;
//    std::cout << "fileHeader.snaplen=" << file_header.snaplen << std::endl;
//    std::cout << "fileHeader.linktype=" << file_header.linktype << std::endl;

    bool bigendian = false;

    if (file_header.magic == 0xa1b2c3d4)
    {
        bigendian = false;
    }
    else if (file_header.magic == htonl(0xa1b2c3d4))
    {
        bigendian = true;
    }
    else
    {
        throw std::runtime_error("Invalid magic: " + std::to_string(file_header.magic));
    }

    if (file_header.linktype != 1 /*DLT_EN10MB*/)
    {
        std::cout << "Invalid linktype: " << file_header.linktype << std::endl;
        //throw std::runtime_error("Invalid linktype: " + std::to_string(file_header.linktype));
    }

    for (;;)
    {
        PCAPHeader h;
        if (!input.read(reinterpret_cast<char*>(&h), sizeof(h)))
        {
            if (input.eof())
            {
                //std::cout << "EOF" << std::endl;
                return;
            }

            std::cout << "errno=" << errno << " strerror=" << strerror(errno) << std::endl;
            throw std::runtime_error("read failed.");
        }

        if (bigendian)
        {
            h.sec    = ntohl(h.sec);
            h.usec   = ntohl(h.usec);
            h.caplen = ntohl(h.caplen);
            h.len    = ntohl(h.len);
        }

        char buffer[128 * 1024];
        if (h.caplen > sizeof(buffer))
        {
            std::cout << "Skip huge buffer of size caplen=" << h.caplen << std::endl;
            continue;
        }
        input.read(&buffer[0], h.caplen);
        f(h, buffer, buffer + std::min(h.len, h.caplen));
    }
}


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " PCAPFile" << std::endl;
        return 1;
    }
    auto count = 0UL;
    auto byte_count = 0UL;
    pcap_for_each(argv[1], [&](PCAPHeader h, char* b, char* e) {
        count++;
        byte_count += e - b;

        (void)h;
        //std::cout << h.sec << "." << h.usec << ": caplen=" << h.caplen << " len=" << h.len << " (e-b)=" << (e-b) << '\n';
    });

    std::cout << "count=" << count << " byte_count=" << byte_count << std::endl;
}



