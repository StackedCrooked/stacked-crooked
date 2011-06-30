#include "VLCPlayer.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFrame>
#include <iostream>


VLCPlayer::VLCPlayer(QWidget * parent, int inWidth, int inHeight) :
    QWidget(parent),
    mWidth(inWidth),
    mHeight(inHeight)
{
    //preparation of the vlc command
    const char * const vlc_args[] =
    {
        "-I", "dummy",
        "--ignore-config", // Don't use VLC's config.
        "--extraintf=logger", // Log anything.
        "--verbose=2"
        //,"--plugin-path=/usr/lib/vlc"
    };

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::Frame));
    setMinimumSize(mWidth, mHeight);

    mVideoWidget=new QFrame(this);

    mVolumeSlider=new QSlider(Qt::Horizontal,this);
    mVolumeSlider->setMaximum(100); //the volume is between 0 and 100
    mVolumeSlider->setToolTip("Audio slider");

    // Note: if you use streaming, there is no ability to use the position slider
    mPositionSlider=new QSlider(Qt::Horizontal,this);
    mPositionSlider->setMaximum(POSITION_RESOLUTION);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mVideoWidget);
    layout->addWidget(mPositionSlider);
    layout->addWidget(mVolumeSlider);
    setLayout(layout);

    mIsPlaying=false;
    mPoller = new QTimer(this);

    //Initialize an instance of vlc
    //a structure for the exception is neede for this initalization
    libvlc_exception_init(&mVLCException);

    //create a new libvlc instance
    mVLCInstance=libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args,&mVLCException);  //tricky calculation of the char space used
    raise (&mVLCException);

    // Create a media player playing environement
    mVLCMediaPlayer = libvlc_media_player_new (mVLCInstance, &mVLCException);
    raise (&mVLCException);

    //connect the two sliders to the corresponding slots (uses Qt's signal / slots technology)
    connect(mPoller, SIGNAL(timeout()), this, SLOT(updateInterface()));
    connect(mPositionSlider, SIGNAL(sliderMoved(int)), this, SLOT(changePosition(int)));
    connect(mVolumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(changeVolume(int)));

    mPoller->start(100); //start timer to trigger every 100 ms the updateInterface slot

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::Frame));
    setMinimumSize(mWidth, mHeight);
    updateGeometry();
}


VLCPlayer::~VLCPlayer()
{
    /* Stop playing */
    libvlc_media_player_stop (mVLCMediaPlayer, &mVLCException);

    /* Free the media_player */
    libvlc_media_player_release (mVLCMediaPlayer);

    libvlc_release (mVLCInstance);
    raise (&mVLCException);
}


QSize VLCPlayer::sizeHint() const
{
    return QSize(mWidth, mHeight);
}


QSize VLCPlayer::minimumSizeHint() const
{
    return QSize(mWidth, mHeight);
}


void VLCPlayer::playFile(QString file)
{
    //the file has to be in one of the following formats /perhaps a little bit outdated)
    /*
    [file://]filename              Plain media file
    http://ip:port/file            HTTP URL
    ftp://ip:port/file             FTP URL
    mms://ip:port/file             MMS URL
    screen://                      Screen capture
    [dvd://][device][@raw_device]  DVD device
    [vcd://][device]               VCD device
    [cdda://][device]              Audio CD device
    udp:[[<source address>]@[<bind address>][:<bind port>]]
    */

    /* Create a new LibVLC media descriptor */
    mMedia = libvlc_media_new (mVLCInstance, file.toAscii(), &mVLCException);
    raise(&mVLCException);

    libvlc_media_player_set_media (mVLCMediaPlayer, mMedia, &mVLCException);
    raise(&mVLCException);

    // /!\ Please note
    //
    // passing the widget to the lib shows vlc at which position it should show up
    // vlc automatically resizes the video to the ´given size of the widget
    // and it even resizes it, if the size changes at the playing

    /* Get our media instance to use our window */
    #if defined(Q_OS_WIN)
        libvlc_media_player_set_drawable(mVLCMediaPlayer, reinterpret_cast<unsigned int>(mVideoWidget->winId()), &mVLCException );
        //libvlc_media_player_set_hwnd(mVLCMediaPlayer, mVideoWidget->winId(), &mVLCException ); // for vlc 1.0
    #elif defined(Q_OS_MAC)
        libvlc_media_player_set_drawable(mVLCMediaPlayer, mVideoWidget->winId(), &mVLCException );
        //libvlc_media_player_set_agl (mVLCMediaPlayer, mVideoWidget->winId(), &mVLCException); // for vlc 1.0
    #else //Linux
        libvlc_media_player_set_drawable(mVLCMediaPlayer, mVideoWidget->winId(), &mVLCException );
        //libvlc_media_player_set_xwindow(mVLCMediaPlayer, mVideoWidget->winId(), &mVLCException ); // for vlc 1.0
    #endif
    raise(&mVLCException);

    /* Play */
    libvlc_media_player_play (mVLCMediaPlayer, &mVLCException );
    raise(&mVLCException);

    mIsPlaying=true;
}


void VLCPlayer::changeVolume(int newVolume)
{
    libvlc_exception_clear(&mVLCException);
    libvlc_audio_set_volume (mVLCInstance,newVolume , &mVLCException);
    raise(&mVLCException);
}


void VLCPlayer::changePosition(int newPosition)
{
    libvlc_exception_clear(&mVLCException);
    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (mVLCMediaPlayer, &mVLCException);
    libvlc_exception_clear(&mVLCException);
    if (curMedia == NULL)
        return;

    float pos=(float)(newPosition)/(float)POSITION_RESOLUTION;
    libvlc_media_player_set_position (mVLCMediaPlayer, pos, &mVLCException);
    raise(&mVLCException);
    updateGeometry();
}


void VLCPlayer::updateInterface()
{
    if(!mIsPlaying)
        return;

    // It's possible that the vlc doesn't play anything
    // so check before
    libvlc_media_t *curMedia = libvlc_media_player_get_media (mVLCMediaPlayer, &mVLCException);
    libvlc_exception_clear(&mVLCException);
    if (curMedia == NULL)
        return;

    float pos=libvlc_media_player_get_position (mVLCMediaPlayer, &mVLCException);
    int siderPos=(int)(pos*(float)(POSITION_RESOLUTION));
    mPositionSlider->setValue(siderPos);
    int volume=libvlc_audio_get_volume (mVLCInstance,&mVLCException);
    mVolumeSlider->setValue(volume);
}


void VLCPlayer::raise(libvlc_exception_t * ex)
{
    if (libvlc_exception_raised (ex))
    {
        std::cout << "Error: " << libvlc_exception_get_message(ex) << "\n";
        exit (-1);
    }
}
