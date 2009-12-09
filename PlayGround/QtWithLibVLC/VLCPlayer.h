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
    QSlider *_positionSlider;
    QSlider *_volumeSlider;
    QFrame *_videoWidget;
    QTimer *poller;
    bool _isPlaying;
    libvlc_exception_t _vlcexcep;
    libvlc_instance_t *_vlcinstance;
    libvlc_media_player_t *_mp;
    libvlc_media_t *_m;

public:
    VLCPlayer();
    ~VLCPlayer();
    void raise(libvlc_exception_t * ex);

public slots:
    void playFile(QString file);
    void updateInterface();
    void changeVolume(int newVolume);
    void changePosition(int newPosition);

};


#endif // VLCPLAYER_H
