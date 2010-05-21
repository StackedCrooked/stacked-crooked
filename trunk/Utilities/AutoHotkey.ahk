::gst-launch::gst-launch-0.10
::gst-inspect::gst-inspect-0.10
::hot-lc::c:\Development\Repositories\lecture-capture
::hot-sd::vlc -I rc screen:// --screen-fps=10 :screen-caching=100 --sout={#}transcode{{}vcodec=h264,venc=x264{{}bframes=0,keyint=40{}},vb=512{}}:rtp{{}mux=ts,dst=127.0.0.1,port=4444{}}
