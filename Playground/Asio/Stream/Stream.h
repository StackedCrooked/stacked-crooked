#ifndef STREAM_H
#define STREAM_H


#include <cstdint>
#include <chrono>
#include <string>
#include <boost/asio.hpp>


using Clock = std::chrono::system_clock;


struct StreamConfig
{
    std::string payload;
    int64_t number_of_frames = 0;
    std::chrono::microseconds frame_interval{};
};


class Stream
{
public:
    Stream(boost::asio::io_context& context, const std::string& ip, int port, StreamConfig config) :
        mRemoteEndPoint(boost::asio::ip::make_address(ip), port),
        mSocket(context),
        mTimer(context),
        mFrame(config.payload),
        mNumberOfFramesSent(0),
        mNumberOfFrames(config.number_of_frames),
        mFrameInterval(config.frame_interval)
    {
        mSocket.open(boost::asio::ip::udp::v4());

        // open socket in non-blocking mode!
        mSocket.non_blocking(true);
    }

    void start()
    {
        auto current_time = Clock::now();
        mNextTransmitTime = current_time;
        mStopTime = current_time + mNumberOfFrames * mFrameInterval;
        send_burst(current_time);
    }

    int64_t getNumFramesSent() const
    {
        return mNumberOfFramesSent;
    }

private:
    void send_burst(Clock::time_point current_time)
    {
        if (current_time >= mStopTime)
        {
            return;
        }

        for (auto i = 0; i != 128; ++i)
        {
            if (!try_send())
            {
                break;
            }

            mNumberOfFramesSent++;
            mNextTransmitTime += mFrameInterval;

            if (mNumberOfFramesSent >= mNumberOfFrames)
            {
                return;
            }

            if (current_time < mNextTransmitTime)
            {
                if (mNextTransmitTime >= mStopTime)
                {
                    // Don't schedule a transmission to happen after the stop time.
                    return;
                }

                mTimer.expires_at(mNextTransmitTime);
                mTimer.async_wait([this](boost::system::error_code ec) {
                    if (!ec) {
                        send_burst(Clock::now());
                    }
                });
                return;
            }
        }

        // Reschedule immediately using post().
        mSocket.get_io_context().post([this] {
            send_burst(Clock::now());
        });
    }

    bool try_send()
    {
        boost::system::error_code ec;

        mSocket.send_to(boost::asio::buffer(mFrame), mRemoteEndPoint, 0, ec);

        if (ec)
        {
            if (ec == boost::asio::error::would_block)
            {
                return false;
            }

            throw boost::system::system_error(ec);
        }

        return true;
    }

    boost::asio::ip::udp::endpoint mRemoteEndPoint;
    boost::asio::ip::udp::socket mSocket;
    boost::asio::system_timer mTimer;
    std::string mFrame;
    int64_t mNumberOfFramesSent = 0;
    int64_t mNumberOfFrames = 0;
    std::chrono::microseconds mFrameInterval{};

    Clock::time_point mNextTransmitTime{};
    Clock::time_point mStopTime{};
};


#endif // STREAM_H
