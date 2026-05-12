import QtQuick
import QtQuick.Controls

Button {
    id: control
    
    property color bgColor: "#2D2D2D"
    property color hoverColor: "#3D3D3D"
    property color pressColor: "#1D1D1D"
    property color textColor: "#FFFFFF"
    
    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 36
        radius: 4
        color: control.down ? control.pressColor : (control.hovered ? control.hoverColor : control.bgColor)
        border.color: "#444444"
        border.width: 1
        
        Behavior on color { ColorAnimation { duration: 150 } }
    }
    
    contentItem: Text {
        text: control.text
        font: control.font
        color: control.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
