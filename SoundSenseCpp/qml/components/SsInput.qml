import QtQuick
import QtQuick.Controls

TextField {
    id: control
    
    color: "#FFFFFF"
    font.pixelSize: 14
    
    background: Rectangle {
        implicitWidth: 150
        implicitHeight: 30
        color: control.activeFocus ? "#2A2A2A" : "#1A1A1A"
        border.color: control.activeFocus ? "#0078D7" : "#333333"
        border.width: 1
        radius: 4
        
        Behavior on border.color { ColorAnimation { duration: 150 } }
        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
