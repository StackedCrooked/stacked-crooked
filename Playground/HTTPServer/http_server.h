#include <memory>
#include <string>


namespace http {


class server
{
public:
    server(const std::string & host, unsigned short port = 80);

    ~server();

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};


} // namespace http
