#include "Stream.h"
#include <iostream>


void run(const std::string& ip, int port)
{
    std::cout << "Starting stream" << std::endl;
    boost::asio::io_context context;

    auto start_time = Clock::now();
    StreamConfig config;
    config.payload.resize(4, 'a');
    config.number_of_frames = 100 * 1000;
    config.frame_interval = std::chrono::microseconds(10);
    Stream stream(context, ip, port, config);
    stream.start();

    // This will block until the flow is finished.
    context.run();
    std::cout << "io_context.run() returned." << std::endl;

    auto elapsed_time = Clock::now() - start_time;
    auto num_frames_sent = stream.getNumFramesSent();
    if (num_frames_sent < config.number_of_frames)
    {
        std::cout << "Not all frames were sent: " << num_frames_sent << "/" << config.number_of_frames << std::endl;
    }
    else
    {
        std::cout << "All " << num_frames_sent << " frames sent in time!" << std::endl;
    }

    std::cout << "Finished!" << std::endl;
    std::cout << "Elapsed time: " << elapsed_time.count() << " microseconds" << std::endl;
}


int main()
{
    try
    {
        run("127.0.0.1", 12345);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
