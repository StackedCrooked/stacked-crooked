//
// blocking_udp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

using boost::asio::ip::udp;


class UDPClient
{
public:
    UDPClient(const std::string & inURL, short inPort) :
        io_service(),
        socket(io_service, udp::endpoint(udp::v4(), 0)),
        resolver(io_service),
        query(udp::v4(), inURL.c_str(), boost::lexical_cast<std::string>(inPort).c_str()),
        iterator(resolver.resolve(query))
    {
    }

    std::string send(const std::string & inMessage)
    {
        socket.send_to(boost::asio::buffer(inMessage.c_str(), inMessage.size()), *iterator);

        static const unsigned cMaxLength = 1024;
        char reply[cMaxLength];
        udp::endpoint sender_endpoint;
        size_t reply_length = socket.receive_from(boost::asio::buffer(reply, cMaxLength), sender_endpoint);
        return std::string(reply, reply_length);
    }

    boost::asio::io_service io_service;
    udp::socket socket;
    udp::resolver resolver;
    udp::resolver::query query;
    udp::resolver::iterator iterator;
};


int main()
{
    UDPClient client("127.0.0.1", 9001);
    std::cout << client.send("Hello") << std::endl;
}
