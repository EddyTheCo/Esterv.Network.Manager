import QtQuick
import QtQuick.Controls
import Esterv.AboutEstervNetworkManager
import Esterv.Styles.Simple
import Esterv.CustomControls
import Esterv.CustomControls.NetworkManager

ApplicationWindow {
    id: window
    visible: true

    background: Rectangle {
        color: Style.backColor1
    }

    Row {
        id: buttons
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        ThemeSwitch {}

        Button {
            text: qsTr("About")
            onClicked: popup.open()
        }
    }
    Popup {
        id: popup
        anchors.centerIn: Overlay.overlay

        modal: true
        About {
            logo: "qrc:/esterVtech.com/imports/ExamplesWifilist/logo.png"
            description: "QML example of using the custom control for list/connect/disconnect to wifis"
        }
    }

    WifiList {
        anchors.top: buttons.bottom
        anchors.topMargin: 10
        width: parent.width
        height: parent.height - buttons.height
    }
}
