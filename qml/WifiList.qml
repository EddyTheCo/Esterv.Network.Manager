pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Esterv.Styles.Simple
import Esterv.CustomControls.NetworkManager

ListView {
    id: wifiList
    clip: true
    spacing: 4

    model: Manager.wifis

    Connections {
        target: Manager

        function onRequestInputPassphrase(path, name) {
            console.log("Wi-Fi", name, "asks for passphrase at", path);
            passwordRequest.networkName = name;
            passwordRequest.open();
        }

        function onFinishRequestInput(path) {
            passwordRequest.close();
        }
    }

    Popup {
        id: passwordRequest
        property string networkName: ""

        visible: false
        anchors.centerIn: Overlay.overlay
        ColumnLayout {
            Label {
                text: qsTr("Enter Wi-Fi %s passphrase:", passwordRequest.networkName)
            }
            TextField {
                id: passwordInput
            }
            Button {
                text: qsTr("Ok")
                onClicked: {
                    passwordRequest.close();
                    Manager.finishConnect(passwordInput.text);
                }
            }
        }
    }

    delegate: Rectangle {
        id: wifibox
        required property string name
        required property int index
        required property int strength
        required property int state
        required property var object

        width: wifiList.width
        height: 50
        radius: 6
        color: mouseArea.pressed ? Style.backColor1 : Style.backColor2
        border.color: Style.frontColor3

        RowLayout {
            anchors.fill: parent
            Wifiicon {
                Layout.minimumWidth: 30
                Layout.minimumHeight: 30
                strength: wifibox.strength * 0.01
            }
            Text {
                text: wifibox.name
                font.bold: true
                font.pixelSize: 16
                color: Style.frontColor1
            }
            Text {
                text: qsTr("Connected")
                font.pixelSize: 13
                color: Style.frontColor2
                visible: wifibox.state === 2 || wifibox.state === 3
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                if (wifibox.state === 2 || wifibox.state === 3) {
                    wifibox.object.disconnect();
                } else {
                    wifibox.object.connect();
                }
            }
        }
    }

    Timer {
        interval: 5000
        running: true
        repeat: true
        onTriggered: Manager.scan()
    }
}
