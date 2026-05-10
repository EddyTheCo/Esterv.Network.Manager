import QtQuick 2.0
import Esterv.Styles.Simple

Item {
    id: wifiicon
    property alias strength: shader.strength
    property alias fcolor: shader.fcolor
    ShaderEffect {
        id: shader
        property var src: wifiicon
        property color fcolor: Style.frontColor1
        property real strength: 0.0
        property var pixelStep: Qt.vector2d(1 / src.width, 1 / src.height)
        fragmentShader: "qrc:/esterVtech.com/imports/Designs/frag/wifi.frag.qsb"
        anchors.fill: parent
    }
}
