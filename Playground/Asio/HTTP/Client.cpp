#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


class Client
{
public:
    Client(boost::asio::io_context& io_context, const std::string& server, const std::string& path) :
        mResolver(io_context),
        mSocket(io_context)
    {
        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        std::ostream request_stream(&mRequest);
        request_stream << "GET " << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << server << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        mResolver.async_resolve(
            server,
            "http",
            boost::bind(&Client::handle_resolve, this, boost::asio::placeholders::error, boost::asio::placeholders::results));
    }

private:
    void handle_resolve(const boost::system::error_code& err, const boost::asio::ip::tcp::resolver::results_type& endpoints)
    {
        if (!err)
        {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            boost::asio::async_connect(
                mSocket,
                endpoints,
                boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            // The connection was successful. Send the request.
            boost::asio::async_write(
                mSocket,
                mRequest,
                boost::bind(&Client::handle_write_request, this, boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_write_request(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Read the response status line. The response_ streambuf will
            // automatically grow to accommodate the entire line. The growth may be
            // limited by passing a maximum size to the streambuf constructor.
            boost::asio::async_read_until(
                mSocket,
                mResponse,
                "\r\n",
                boost::bind(&Client::handle_read_status_line, this, boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err.message() << "\n";
        }
    }

    void handle_read_status_line(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Check that response is OK.
            std::istream response_stream(&mResponse);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                std::cout << "Invalid response\n";
                return;
            }
            if (status_code != 200)
            {
                std::cout << "Response returned with status code ";
                std::cout << status_code << "\n";
                return;
            }

            // Read the response headers, which are terminated by a blank line.
            boost::asio::async_read_until(
                mSocket,
                mResponse,
                "\r\n\r\n",
                boost::bind(&Client::handle_read_headers, this, boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    void handle_read_headers(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Process the response headers.
            std::istream response_stream(&mResponse);
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
                std::cout << header << "\n";
            std::cout << "\n";

            // Write whatever content we already have to output.
            if (mResponse.size() > 0)
                std::cout << &mResponse;

            // Start reading remaining data until EOF.
            boost::asio::async_read(
                mSocket,
                mResponse,
                boost::asio::transfer_at_least(1),
                boost::bind(&Client::handle_read_content, this, boost::asio::placeholders::error));
        }
        else
        {
            std::cout << "Error: " << err << "\n";
        }
    }

    void handle_read_content(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Write all of the data that has been read so far.
            std::cout << &mResponse;

            // Continue reading remaining data until EOF.
            boost::asio::async_read(
                mSocket,
                mResponse,
                boost::asio::transfer_at_least(1),
                boost::bind(&Client::handle_read_content, this, boost::asio::placeholders::error));
        }
        else if (err != boost::asio::error::eof)
        {
            std::cout << "Error: " << err << "\n";
        }
        else
        {
            std::cout << "EOF received\n";
        }
    }

    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::ip::tcp::socket mSocket;
    boost::asio::streambuf mRequest;
    boost::asio::streambuf mResponse;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cout << "Usage: async_client <server> <path>\n";
            std::cout << "Example:\n";
            std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
            return 1;
        }

        boost::asio::io_context io_context;
        Client c(io_context, argv[1], argv[2]);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}
