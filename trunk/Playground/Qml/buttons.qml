import QtQuick 2.0


Rectangle {
    id: rect
    width: 640
    height: 200
    color: "lightyellow"
    TextInput{
        id: cmd
        anchors.margins: 10
        anchors.left: rect.left
        anchors.bottom: rect.bottom
        text: "g++ main.cpp"
    }
    Text {
        id: compile
        anchors.margins: 10
        anchors.right: share.left
        anchors.bottom: rect.bottom
        text: "Compile, link and run..."
    }
    Text {
        id: share
        anchors.margins: 10
        anchors.right: parent.right
        anchors.bottom: rect.bottom
        text: "Share!"
    }
}
