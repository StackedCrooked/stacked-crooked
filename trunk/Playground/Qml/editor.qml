import QtQuick 2.0

Rectangle {
    id: toolbar
    color: "lightblue"
    width: 640
    height: 480
    Row {
        id: donate
        anchors.top: parent.top
        anchors.left: parent.left
        spacing: 8
        Column {
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Donate your monies"
                color: "yellow"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: 'OKAY'
                color: "blue"
                font { underline: true }
            }
        }
        Column {
            id: help
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Help me pelase"
                color: "yellow"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "NO WAY"
                color: "blue"
                font { underline: true }
            }
        }
    }
    Column {
        id: feedback
        anchors.right: parent.right
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Feedback everything"
            color: "yellow"
        }
        Row {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: "Read"
                color: "blue"
                font { underline: true }
            }
            Text {
                text: "Write"
                color: "blue"
                font { underline: true }
            }
        }
    }
}
