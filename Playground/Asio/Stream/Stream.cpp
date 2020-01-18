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
    send_next_packet();
}


void Stream::send_next_packet()
{
    auto current_time = Clock::now();

    if (current_time >= mForcedStopTime)
    {
        return;
    }

    // Send a burst of at most 8 packets.
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
            mTimer.expires_at(std::min(mForcedStopTime, mNextTransmitTime));
            mTimer.async_wait([this](boost::system::error_code ec) {
                if (!ec) {
                    send_next_packet();
                }
            });
            return;
        }
    }

    // Reschedule immediately.
    mContext.post([this] {
        send_next_packet();
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
            return false;
        }
        else
        {
            throw boost::system::system_error(ec);
        }
    }

    return true;
}
