#ifndef STREAM_H
#define STREAM_H


#include <cstdint>
#include <chrono>
#include <memory>
#include <string>
#include <boost/asio.hpp>


using Clock = std::chrono::system_clock;


struct StreamConfig
{
    std::string payload;
    int64_t number_of_frames = 0;
    std::chrono::microseconds frame_interval{};
};


class Stream : std::enable_shared_from_this<Stream>
{
public:
    Stream(boost::asio::io_context& context, const std::string& ip, int port, StreamConfig config);

    void start();

    int64_t getNumFramesSent() const
    {
        return mNumFramesSent;
    }

private:
    void send_next();
    void send_next(Clock::time_point current_time);

    boost::asio::io_context& mContext;
    boost::asio::ip::udp::endpoint mRemoteEndPoint;
    boost::asio::ip::udp::socket mSocket;
    boost::asio::system_timer mTimer;
    std::string mPayload;
    int64_t mNumFramesSent = 0;
    int64_t mMaxTransmissions = 0;
    std::chrono::microseconds mTransmitInterval{};

    Clock::time_point mNextTransmitTime{};
    Clock::time_point mStartTime{};
    Clock::time_point mExpectedStopTime{};
    Clock::time_point mForcedStopTime{};
};


#endif // STREAM_H
