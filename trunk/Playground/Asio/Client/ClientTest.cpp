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


std::string SendUDPMessage(const std::string & inURL,
                           short inPort,
                           const std::string & inMessage)
{
    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), inURL.c_str(), boost::lexical_cast<std::string>(inPort).c_str());
    udp::resolver::iterator iterator = resolver.resolve(query);

    socket.send_to(boost::asio::buffer(inMessage.c_str(), inMessage.size()), *iterator);

    static const unsigned cMaxLength = 1024;
    char reply[cMaxLength];
    udp::endpoint sender_endpoint;
    size_t reply_length = socket.receive_from(boost::asio::buffer(reply, cMaxLength), sender_endpoint);
    return std::string(reply, reply_length);
}


int main()
{
    std::cout << SendUDPMessage("127.0.0.1", 9001, "Hello") << std::endl;
}
