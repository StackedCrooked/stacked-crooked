//
// async_client.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


class TCPClient
{
public:
    TCPClient(boost::asio::io_service& io_service, const std::string& server, const std::string& path):
        resolver_(io_service),
        socket_(io_service)
    {
        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        std::ostream request_stream(&request_);
        request_stream << "GET " << path << " HTTP/1.0\r\n";
        request_stream << "Host: " << server << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Start an asynchronous resolve to translate the server and service names
        // into a list of endpoints.
        boost::asio::ip::tcp::resolver::query query(server, "http");
        resolver_.async_resolve(
            query,
            boost::bind(
                &TCPClient::handle_resolve,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
    }

private:
    void handle_resolve(const boost::system::error_code& err,
                        boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            boost::asio::async_connect(
                socket_,
                endpoint_iterator,
                boost::bind(&TCPClient::handle_connect, this, boost::asio::placeholders::error));
        }
        else
        {
            std::cerr << "Resolve: " << err.message() << std::endl;
        }
    }

    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            // The connection was successful. Send the request.
            boost::asio::async_write(
                socket_,
                request_,
                boost::bind(
                    &TCPClient::handle_write_request,
                    this,
                    boost::asio::placeholders::error));
        }
        else
        {
            std::cerr << "Connect: " << err.message() << std::endl;
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
                socket_,
                response_,
                "\r\n",
                boost::bind(
                    &TCPClient::handle_read_status_line,
                    this,
                    boost::asio::placeholders::error));
        }
        else
        {
            std::cerr << "Write: " << err.message() << std::endl;
        }
    }

    void handle_read_status_line(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Check that response is OK.
            std::istream response_stream(&response_);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                std::cerr << "Invalid response" << std::endl;
                return;
            }
            if (status_code != 200)
            {
                std::cerr << "Response returned with status code ";
                std::cerr << status_code << std::endl;
                return;
            }

            // Read the response headers, which are terminated by a blank line.
            boost::asio::async_read_until(
                socket_,
                response_,
                "\r\n\r\n",
                boost::bind(
                    &TCPClient::handle_read_headers,
                    this,
                    boost::asio::placeholders::error));
        }
        else
        {
            std::cerr << "ReadStatusLine: " << err.message() << std::endl;
        }
    }

    void handle_read_headers(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Process the response headers.
            std::istream response_stream(&response_);
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
            {
                std::cout << header << "\n";
            }
            std::cout << "\n";

            // Write whatever content we already have to output.
            if (response_.size() > 0)
            {
                std::cout << &response_;
			}

            // Start reading remaining data until EOF.
            boost::asio::async_read(
                socket_,
                response_,
                boost::asio::transfer_at_least(1),
                boost::bind(
                    &TCPClient::handle_read_content,
                    this, boost::asio::placeholders::error));
        }
        else
        {
            std::cerr << "ReadHeaders: " << err.message() << std::endl;
        }
    }

    void handle_read_content(const boost::system::error_code& err)
    {
        if (!err)
        {
            // Write all of the data that has been read so far.
            std::cout << &response_;

            // Continue reading remaining data until EOF.
            boost::asio::async_read(
                socket_,
                response_,
                boost::asio::transfer_at_least(1),
                boost::bind(
                    &TCPClient::handle_read_content,
                    this,
                    boost::asio::placeholders::error));
        }
        else if (err != boost::asio::error::eof)
        {
            std::cerr << "ReadContent: " << err.message() << std::endl;
        }
        else
        {
            std::cerr << "Error: " << err.message() << std::endl;
        }
    }

    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: async_client <server> <path>\n";
            std::cerr << "Example:\n";
            std::cerr << "  async_client www.boost.org /LICENSE_1_0.txt\n";
            return 1;
        }


        std::cerr << "TEST CERR" << std::endl;
        std::cout << "TEST COUT" << std::endl;

        boost::asio::io_service io_service;
        TCPClient c(io_service, argv[1], argv[2]);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
