import QtQuick
import QtQuick.Controls

Switch {
    id: control
    
    property color activeColor: "#0078D7"
    property color inactiveColor: "#333333"
    property color handleColor: "#ffffff"

    indicator: Rectangle {
        implicitWidth: 40
        implicitHeight: 20
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 10
        color: control.checked ? control.activeColor : control.inactiveColor
        
        Behavior on color { ColorAnimation { duration: 200 } }

        Rectangle {
            x: control.checked ? parent.width - width - 2 : 2
            y: 2
            width: 16
            height: 16
            radius: 8
            color: control.handleColor
            
            Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutQuad } }
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
