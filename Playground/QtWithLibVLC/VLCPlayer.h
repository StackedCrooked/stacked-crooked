#ifndef VLCPLAYER_H
#define VLCPLAYER_H


#include <vlc/vlc.h>
#include <QWidget>


class QVBoxLayout;
class QPushButton;
class QTimer;
class QFrame;
class QSlider;

#define POSITION_RESOLUTION 10000

class VLCPlayer : public QWidget
{
    Q_OBJECT

public:
    VLCPlayer(QWidget * parent, int inWidth, int inHeight);

    ~VLCPlayer();

    virtual QSize sizeHint() const;

    virtual QSize minimumSizeHint() const;

public slots:
    void playFile(QString file);
    void updateInterface();
    void changeVolume(int newVolume);
    void changePosition(int newPosition);

private:
    void raise(libvlc_exception_t * ex);
    QSlider * mPositionSlider;
    QSlider * mVolumeSlider;
    QFrame * mVideoWidget;
    QTimer * mPoller;
    int mWidth;
    int mHeight;
    bool mIsPlaying;
    libvlc_exception_t mVLCException;
    libvlc_instance_t * mVLCInstance;
    libvlc_media_player_t * mVLCMediaPlayer;
    libvlc_media_t * mMedia;
};


#endif // VLCPLAYER_H
