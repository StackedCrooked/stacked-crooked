#include <QtGui>
#include <QtNetwork>

#include "Plugin.h"

Plugin::Plugin(QWidget *parent)
    : QDialog(parent)
{
    hostLabel = new QLabel(tr("&Server name:"));
    portLabel = new QLabel(tr("S&erver port:"));
    messageLabel = new QLabel(tr("Message:"));
    logLabel = new QLabel(tr("Log:"));

    hostLineEdit = new QLineEdit("Localhost");
    portLineEdit = new QLineEdit;
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostLineEdit);
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));

    messageLineEdit = new QTextEdit();
    logTextEdit = new QTextEdit();
    sendMessageButton = new QPushButton(tr("Send message"));
    sendMessageButton->setDefault(true);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(sendMessageButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    tcpSocket = new QTcpSocket(this);

    connect(hostLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableGetFortuneButton()));
    connect(portLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableGetFortuneButton()));
    connect(sendMessageButton, SIGNAL(clicked()),
            this, SLOT(SendMessage()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel,        0, 0);
    mainLayout->addWidget(hostLineEdit,     0, 1);
    mainLayout->addWidget(portLabel,        1, 0);
    mainLayout->addWidget(portLineEdit,     1, 1);
    mainLayout->addWidget(messageLabel,     2, 0);
    mainLayout->addWidget(messageLineEdit,  2, 1);
    mainLayout->addWidget(logLabel,         3, 0);
    mainLayout->addWidget(logTextEdit,      3, 1);
    mainLayout->addWidget(statusLabel,      4, 0, 1, 2);
    mainLayout->addWidget(buttonBox,        4, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Editor-Plugin"));
    portLineEdit->setFocus();
    Log( "End of constructor" );
}

void
Plugin::Log( const QString& inString )
{
    logTextEdit->insertPlainText( QString( "\n" ) + inString );
}

void Plugin::Connect( int inPort )
{
    switch( tcpSocket->state() )
    {
        case QAbstractSocket::UnconnectedState:
        {
            Log
            (
                QString( "Not yet connected. Attempting to connect to " ) +
                hostLineEdit->text() +
                QString( " using port: " ) + QVariant( inPort ).toString()
            );
            tcpSocket->connectToHost( hostLineEdit->text(), inPort );
            break;
        }
        case QAbstractSocket::ConnectedState:
        {
            Log( "Already connected" );
            break;
        }
        default:
        {
            Log( "Plugin::Connect: Unexpected connection state." );
            break;
        }
    }
    blockSize = 0;
}

void Plugin::SendMessage()
{
    Log( "SendMessage()" );
    Connect( portLineEdit->text().toInt() );
    int theNumberOfCharsWritten =
            tcpSocket->write( messageLineEdit->toPlainText().toAscii().constData(),
                              messageLineEdit->toPlainText().size() );
    Log( QVariant( theNumberOfCharsWritten ).toString() + " characters were written." );
}

void Plugin::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
        {
            Log( "QAbstractSocket::RemoteHostClosedError" );
            break;
        }
        case QAbstractSocket::HostNotFoundError:
        {
            Log( "QAbstractSocket::HostNotFoundError: The host was not found."
                    "Please check the host name and port settings." );
            break;
        }
        case QAbstractSocket::ConnectionRefusedError:
        {
            Log( "QAbstractSocket::ConnectionRefusedError: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.");
            break;
        }
        default:
        {
            Log( "The following error occurred: " + tcpSocket->errorString() );
            break;
        }
    }

}


