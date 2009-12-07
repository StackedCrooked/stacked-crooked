#include "ControlCenterWindow.h"
#include "Poco/Types.h"
#include <assert.h>


ControlCenterWindow::ControlCenterWindow() :
    mControlCenter(new ControlCenter),
    mRTPStream(new RTPStream(mControlCenter)),
    mMediaSource(new Phonon::MediaSource(mRTPStream)),
    mVideoPlayer(0),
    mStartStop(0),
    mLogField(0)
{
    mStartStop = new QPushButton("Play");
    QObject::connect(mStartStop, SIGNAL(clicked()), this, SLOT(startStop()));

    mLogField = new QTextEdit(this);
    mLogField->append("Log Field Created.\n");

    mVideoPlayer = new Phonon::VideoPlayer(Phonon::VideoCategory,this);

    QObject::connect(mVideoPlayer->mediaObject(),
                     SIGNAL(stateChanged(Phonon::State, Phonon::State)),
                     this,
                     SLOT(onStateChanged(Phonon::State, Phonon::State)));

    QVBoxLayout * vbox = new QVBoxLayout;
    vbox->addWidget(mVideoPlayer, 1);
    vbox->addWidget(mStartStop, 0);
    vbox->addWidget(mLogField, 0);
    setLayout(vbox);
    mVideoPlayer->load(*mMediaSource);
}


ControlCenterWindow::~ControlCenterWindow()
{
    delete mRTPStream;
    delete mMediaSource;
    delete mControlCenter;
}


void ControlCenterWindow::onStateChanged(Phonon::State inNewState, Phonon::State /*inOldState*/)
{    
    mStartStop->setEnabled(false);
    switch (inNewState)
    {
        case Phonon::PlayingState:
        {
            mStartStop->setEnabled(true);
            mLogField->append("Media state is now: PlayingState.\n");
            break;
        }
        case Phonon::StoppedState:
        {
            mStartStop->setEnabled(true);
            mLogField->append("Media state is now: StoppedState.\n");
            break;
        }
        case Phonon::LoadingState:
        {
            mLogField->append("Media state is now: LoadingState.\n");
            break;
        }
        case Phonon::BufferingState:
        {
            mLogField->append("Media state is now: BufferingState.\n");
            break;
        }
        case Phonon::PausedState:
        {
            mStartStop->setEnabled(true);
            mLogField->append("Media state is now: PausedState.\n");
            break;
        }
        case Phonon::ErrorState:
        {
            mLogField->append("Media state is now: ErrorState.");
            if (mVideoPlayer->mediaObject()->errorType() == Phonon::NormalError)
            {
                mLogField->append(" ErrorType: NormalError.");
            }
            else if (mVideoPlayer->mediaObject()->errorType() == Phonon::FatalError)
            {
                mLogField->append(" ErrorType: FatalError.");
            }
            break;
        }
        default:
        {
            mLogField->append("Media state is now: Invalid.\n");
            break;
        }
    }
    mVideoPlayer->setEnabled(inNewState == Phonon::StoppedState);
}


void ControlCenterWindow::startStop()
{
    if (mVideoPlayer->mediaObject()->state() == Phonon::PlayingState)
    {
        mVideoPlayer->stop();
        mStartStop->setText("Stop");
    }
    else if (mVideoPlayer->mediaObject()->state() == Phonon::LoadingState)
    {
        mStartStop->setText("Stop");
        mVideoPlayer->play();
    }
    else if (mVideoPlayer->mediaObject()->state() == Phonon::StoppedState)
    {
        mVideoPlayer->load(*mMediaSource);
        mStartStop->setText("Play");
    }
}



























