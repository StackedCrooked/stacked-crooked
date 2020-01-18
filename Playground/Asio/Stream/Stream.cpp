#include "Stream.h"
#include <iostream>
#include <boost/array.hpp>


Stream::Stream(boost::asio::io_context& context, const std::string& ip, int port, StreamConfig config) :
    mContext(context),
    mRemoteEndPoint(boost::asio::ip::make_address(ip), port),
    mSocket(mContext),
    mTransmitTimer(mContext),
    mPayload(config.payload),
    mNumFramesSent(0),
    mMaxTransmissions(config.number_of_frames),
    mTransmitInterval(config.frame_interval)
{
    mSocket.open(boost::asio::ip::udp::v4());
    mSocket.non_blocking(true);
}


void Stream::start()
{
    auto current_time = Clock::now();
    mNextTransmitTime = current_time;
    mStopTime = current_time + mMaxTransmissions * mTransmitInterval;
    send_next(current_time);
}


void Stream::send_next(Clock::time_point current_time)
{
    for (;;)
    {
        if (current_time >= mStopTime)
        {
            std::cout << "Stopping because timer has expired." << std::endl;
            return;
        }

        if (current_time < mNextTransmitTime)
        {
            //std::cout << "Reschedule in " << (mNextTransmitTime - current_time).count() << "us. mNumFramesSent=" << mNumFramesSent << std::endl;

            mTransmitTimer.expires_at(mNextTransmitTime);
            mTransmitTimer.async_wait([this](const boost::system::error_code& ec) {
                if (ec) {
                    std::cout << "mTransmitTimer: got error in async_await callback: " << ec.message() << std::endl;
                    return;
                }
                send_next(Clock::now());
            });
            return;
        }

        boost::system::error_code ec;
        mSocket.send_to(boost::asio::buffer(mPayload), mRemoteEndPoint, 0, ec);

        if (ec)
        {
            if (ec == boost::asio::error::would_block)
            {
                std::cout << "mNumFramesSent=" << mNumFramesSent << std::endl;
                mContext.post([this] { send_next(Clock::now()); });
                return;
            }

            std::cout << "Stream: transmit error: " << ec.message();
            std::cout << "Stopping transmission!" << std::endl;
            return;
        }

        if (++mNumFramesSent == mMaxTransmissions)
        {
            //std::cout << "Stream is finished! Stopping transmission!" << std::endl;
            return;
        }

        mNextTransmitTime += mTransmitInterval;
    }

}
