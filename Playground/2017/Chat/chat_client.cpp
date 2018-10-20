//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <atomic>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(boost::asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  bool write(const chat_message& msg)
  {
    if (quit_)
    {
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": can't write because quit flag is set." << std::endl;
        return false;
    }

    boost::asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
    return true;
  }

  void close()
  {
    boost::asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    boost::asio::async_connect(socket_, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
          if (!ec)
          {
            do_read_header();
          }
          else
          {
            quit_ = true;
            std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": error=" << ec.message() << std::endl;
          }
        });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            quit_ = true;
            std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": error=" << ec.message() << std::endl;
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            quit_ = true;
            std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": error=" << ec.message() << std::endl;
            socket_.close();
          }
        });
  }

  void do_write()
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            quit_ = true;
            std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": error=" << ec.message() << std::endl;
            socket_.close();
          }
        });
  }

private:
  boost::asio::io_context& io_context_;
  std::atomic<bool> quit_{false};
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client c(io_context, endpoints);

    std::thread t([&io_context](){
        io_context.run();
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": AFTER io_context.run()" << std::endl;
    });

    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      chat_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      if (!c.write(msg))
      {
        break;
      }
    }

    std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": BEFORE c.close()" << std::endl;
    c.close();
    std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": AFTER c.close()" << std::endl;
    std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": BEFORE t.join()" << std::endl;
    t.join();
    std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": AFTER t.join()" << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

