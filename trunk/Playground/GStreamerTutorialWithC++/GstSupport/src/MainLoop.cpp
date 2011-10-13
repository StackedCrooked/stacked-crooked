#include "Gst/MainLoop.h"


namespace Gst
{

    
    MainLoop::MainLoop() : 
        mMainLoop(0)
    {
        mMainLoop = g_main_loop_new(NULL, FALSE);
    }
    
    
    MainLoop::~MainLoop()
    {
        quit();
    }
    
    
    void MainLoop::run()
    {
        g_main_loop_run(mMainLoop);
    }
    
    
    void MainLoop::quit()
    {
        if (mMainLoop)
        {
            g_main_loop_quit(mMainLoop);
            mMainLoop = 0;
        }
    }


} // namespace XULWin
