import QtQuick
import QtQuick.Controls

TextField {
    id: control
    
    background: Rectangle {
        implicitWidth: 150
        implicitHeight: 30
        color: "#111111"
        radius: 4
        border.color: control.activeFocus ? "#0078D7" : "#333333"
        border.width: 1
    }
    
    color: "#FFFFFF"
    placeholderTextColor: "#555555"
    font.pixelSize: 12
    leftPadding: 10
    rightPadding: 10
    
    selectionColor: "#0078D7"
    selectedTextColor: "#FFFFFF"
}
