#include "Stream.h"
#include <iostream>
#include <boost/array.hpp>


Stream::Stream(boost::asio::io_context& context, const std::string& ip, int port, StreamConfig config) :
    mContext(context),
    mRemoteEndPoint(boost::asio::ip::make_address(ip), port),
    mSocket(mContext),
    mTimer(mContext),
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
    mNextTransmitTime = mStartTime;
    mNextTransmitTime = current_time;
    mExpectedStopTime = current_time + mMaxTransmissions * mTransmitInterval;
    mForcedStopTime = mExpectedStopTime + std::chrono::milliseconds(500); // extra time
    enter_transmit_loop();
}


void Stream::enter_transmit_loop()
{
    auto current_time = Clock::now();

    if (current_time >= mForcedStopTime)
    {
        return;
    }

    send_next(current_time);
}


void Stream::send_next(Clock::time_point current_time)
{
    for (auto i = 0; i != 8; ++i)
    {
        if (!try_send())
        {
            // Socket is blocked. Reschedule immediately using post().
            break;
        }

        mNextTransmitTime += mTransmitInterval;
        mNumFramesSent++;

        if (mNumFramesSent >= mMaxTransmissions)
        {
            assert(mNumFramesSent == mMaxTransmissions);
            return;
        }

        if (current_time < mNextTransmitTime)
        {
            mTimer.expires_at(mNextTransmitTime);
            mTimer.async_wait([this](boost::system::error_code ec) {
                if (!ec) {
                    enter_transmit_loop();
                }
            });
            return;
        }
    }

    // The burst was sent without catching up.
    // Reschedule immediately.
    mContext.post([this] {
        enter_transmit_loop();
    });

}


bool Stream::try_send()
{
    boost::system::error_code ec;
    mSocket.send_to(boost::asio::buffer(mPayload), mRemoteEndPoint, 0, ec);

    if (ec)
    {
        if (ec == boost::asio::error::would_block)
        {
            // We failed to transmit the packet.
            return 0;
        }
        else
        {
            throw std::runtime_error("Failed to transmit frame. Stopping flow.");
        }
    }

    return mPayload.size();
}
