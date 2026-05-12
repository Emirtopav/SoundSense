import QtQuick

Rectangle {
    id: card
    color: "#1A1A1A"
    radius: 8
    border.color: hoverHandler.hovered ? "#333333" : "#222222"
    border.width: 1

    property bool interactive: false

    HoverHandler {
        id: hoverHandler
        enabled: card.interactive
    }

    Behavior on border.color {
        ColorAnimation { duration: 150 }
    }
}
