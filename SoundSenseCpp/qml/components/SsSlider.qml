import QtQuick
import QtQuick.Controls

Slider {
    id: control
    
    // Customization properties
    property color activeColor: "#0078D7" // Default blue/SteelSeries style
    property color trackColor: "#333333"
    
    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 6
        width: control.availableWidth
        height: implicitHeight
        radius: 3
        color: control.trackColor

        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            color: control.activeColor
            radius: 3
            
            Behavior on width {
                NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
            }
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 16
        implicitHeight: 16
        radius: 8
        color: control.pressed ? Qt.lighter(control.activeColor, 1.2) : (control.hovered ? Qt.lighter(control.activeColor, 1.1) : control.activeColor)
        
        // Subtle drop shadow for depth
        border.color: "#111111"
        border.width: 1
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
    }
}
