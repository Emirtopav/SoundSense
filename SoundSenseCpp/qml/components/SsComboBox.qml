import QtQuick
import QtQuick.Controls

ComboBox {
    id: control
    
    delegate: ItemDelegate {
        width: control.width
        contentItem: Text {
            text: modelData
            color: "#FFFFFF"
            font.pixelSize: 12
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            color: highlighted ? "#333333" : "#1A1A1A"
        }
    }

    contentItem: Text {
        leftPadding: 10
        text: control.displayText
        font.pixelSize: 12
        color: "#FFFFFF"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 30
        color: "#111111"
        radius: 4
        border.color: "#333333"
        border.width: 1
    }
    
    popup: Popup {
        y: control.height
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex
        }

        background: Rectangle {
            color: "#1A1A1A"
            radius: 4
            border.color: "#333333"
        }
    }
}
