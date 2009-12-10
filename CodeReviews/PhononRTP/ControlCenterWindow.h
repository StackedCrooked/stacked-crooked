#ifndef CONTROLCENTERWINDOW_H
#define CONTROLCENTERWINDOW_H


#include <QtGui>
#include <Phonon/MediaObject>
#include <Phonon/MediaSource>
#include <Phonon/VideoWidget>
#include <Phonon/VideoPlayer>
#include "ControlCenter.h"


class RTPStream : public QIODevice
{
public:
    RTPStream(ControlCenter * inControlCenter) :
        mControlCenter(inControlCenter)
    {
    }

protected:

    /**
     * Reads up to maxSize bytes from the device into data, and returns the
     * number of bytes read or -1 if an error occurred. If there are no bytes
     * to be read, this function should return -1 if there can never be more
     * bytes available (for example: socket closed, pipe closed, sub-process
     * finished).
     */
    virtual qint64 readData(char *data, qint64 maxlen)
    {
        return mControlCenter->getRTPDataFetcher().readData(data, maxlen);
    }

    /**
     * Writes up to maxSize bytes from data to the device. Returns the number
     * of bytes written, or -1 if an error occurred.
     */
    virtual qint64 writeData(const char *, qint64)
    {
        return -1;
    }

    virtual bool isSequential() const
    {
        return true;
    }

private:
    ControlCenter * mControlCenter;
};


class ControlCenterWindow : public QWidget
{
    Q_OBJECT

public:
    ControlCenterWindow();

    ~ControlCenterWindow();

public slots:
    void onStateChanged(Phonon::State inNewState, Phonon::State inOldState);
    void startStop();

private:
    ControlCenter * mControlCenter;
    RTPStream * mRTPStream;
    Phonon::MediaSource * mMediaSource;
    Phonon::VideoPlayer * mVideoPlayer;
    QPushButton * mStartStop;
    QTextEdit * mLogField;
};


#endif // CONTROLCENTERWINDOW_H
