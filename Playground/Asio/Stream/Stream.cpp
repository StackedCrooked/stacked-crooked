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
    send_next();
}


void Stream::send_next()
{
    auto current_time = Clock::now();

    if (current_time >= mForcedStopTime)
    {
        return;
    }

    // Send a burst of at most 8 packets.
    for (auto i = 0; i != 8; ++i)
    {
        boost::system::error_code ec;
        mSocket.send_to(boost::asio::buffer(mPayload), mRemoteEndPoint, 0, ec);
        if (ec)
        {
            if (ec == boost::asio::error::would_block)
            {
                // End the current burst.
                break;
            }

            throw boost::system::system_error(ec);
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
            mTimer.expires_at(std::min(mForcedStopTime, mNextTransmitTime));
            mTimer.async_wait([this](boost::system::error_code ec) {
                if (!ec) {
                    send_next();
                }
            });
            return;
        }
    }

    // End of burst was reached without catching up with the timer => reschedule immediately.
    mContext.post([this] {
        send_next();
    });

}
