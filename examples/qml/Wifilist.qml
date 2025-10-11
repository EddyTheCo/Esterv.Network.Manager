import QtQuick
import QtQuick.Controls
import Esterv.Styles.Simple
import Esterv.CustomControls
import Esterv.CustomControls.NetworkManager

ApplicationWindow {
    id: window
    visible: true

    background: Rectangle {
        color: Style.backColor1
    }

    ThemeSwitch {
        id: themeswitch
    }

    WifiList {
        anchors.top: themeswitch.bottom
        width: parent.width
        height: parent.height - themeswitch.height
    }
}
