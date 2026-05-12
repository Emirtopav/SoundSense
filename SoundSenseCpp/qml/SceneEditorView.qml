import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models
import SoundSense 1.0
import "components"

Item {
    id: root

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Left Panel: Layer List
        ColumnLayout {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            spacing: 15

            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: "Layers"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                    Layout.fillWidth: true
                }
                
                SsButton { 
                    text: "Load"
                    Layout.preferredWidth: 60
                    onClicked: backend ? backend.loadScene("active_scene.json") : null
                }
                SsButton { 
                    text: "Save"
                    Layout.preferredWidth: 60
                    onClicked: backend ? backend.saveScene("active_scene.json") : null
                }
            }

            // Layer Add Buttons
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                contentWidth: buttonRow.width
                clip: true
                
                RowLayout {
                    id: buttonRow
                    spacing: 8
                    SsButton { text: "+ Rect"; onClicked: backend.addRect() }
                    SsButton { text: "+ Image"; onClicked: backend.addImage() }
                    SsButton { text: "+ Text"; onClicked: backend.addText() }
                    SsButton { text: "+ Clock"; onClicked: backend.addClock() }
                    SsButton { text: "+ FFT"; onClicked: backend.addVisualizer() }
                    SsButton { text: "+ Wave"; onClicked: backend.addWaveform() }
                }
            }

            ListView {
                id: sceneList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                spacing: 5
                
                model: DelegateModel {
                    id: visualModel
                    model: backend.sceneModel
                    delegate: DropArea {
                        id: delegateRoot
                        width: sceneList.width
                        height: 54
                        
                        keys: ["layer"]
                        
                        onEntered: (drag) => {
                            let from = drag.source.dndIndex;
                            let to = index;
                            if (from !== to && from >= 0) {
                                visualModel.items.move(from, to);
                                backend.sceneModel.move(from, to);
                            }
                        }

                        property int visualIndex: DelegateModel.itemsIndex

                        Rectangle {
                            id: layerCard
                            anchors.fill: parent
                            anchors.margins: 2
                            radius: 8
                            color: (backend && backend.selectedLayerIndex === index) ? "#1A2533" : (layerHover.containsMouse ? "#202020" : "#1A1A1A")
                            border.color: (backend && backend.selectedLayerIndex === index) ? "#0078D7" : (layerHover.containsMouse ? "#333333" : "#222222")
                            border.width: 1
                            opacity: dragHandleMouse.drag.active ? 0.6 : 1.0

                            MouseArea {
                                id: layerHover
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: backend.selectedLayerIndex = index
                            }

                            // Drag support
                            property int dndIndex: index
                            
                            Drag.active: dragHandleMouse.drag.active
                            Drag.source: delegateRoot
                            Drag.keys: ["layer"]
                            Drag.hotSpot.x: 15
                            Drag.hotSpot.y: 25

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 8

                                // Drag Handle
                                Text {
                                    id: dragHandle
                                    text: "≡"
                                    color: dragHandleMouse.drag.active ? "#0078D7" : "#555555"
                                    font.pixelSize: 16
                                    MouseArea {
                                        id: dragHandleMouse
                                        anchors.fill: parent
                                        drag.target: layerCard
                                        drag.axis: Drag.YAxis
                                        cursorShape: Qt.SizeVerCursor
                                    }
                                }

                                Text {
                                    text: model.name
                                    color: "#ffffff"
                                    font.pixelSize: 13
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    font.strikeout: !model.visible
                                }

                                // Visibility Toggle
                                Rectangle {
                                    width: 26; height: 26; radius: 5
                                    color: visHover.containsMouse ? "#2A2A2A" : "transparent"
                                    Text { text: model.visible ? "👁" : "⊝"; color: model.visible ? "#AAAAAA" : "#555555"; anchors.centerIn: parent; font.pixelSize: 14 }
                                    MouseArea { id: visHover; anchors.fill: parent; hoverEnabled: true; onClicked: model.visible = !model.visible }
                                }
                                
                                // Lock Toggle
                                Rectangle {
                                    width: 26; height: 26; radius: 5
                                    color: lockHover.containsMouse ? "#2A2A2A" : "transparent"
                                    Text { text: model.locked ? "🔒" : "🔓"; color: model.locked ? "#E81123" : "#555555"; anchors.centerIn: parent; font.pixelSize: 12 }
                                    MouseArea { id: lockHover; anchors.fill: parent; hoverEnabled: true; onClicked: model.locked = !model.locked }
                                }

                                // Duplicate
                                Rectangle {
                                    width: 26; height: 26; radius: 5
                                    color: dupHover.containsMouse ? "#2A2A2A" : "transparent"
                                    Text { text: "⧉"; color: "#AAAAAA"; anchors.centerIn: parent; font.pixelSize: 14 }
                                    MouseArea { id: dupHover; anchors.fill: parent; hoverEnabled: true; onClicked: backend.sceneModel.duplicate(index) }
                                }

                                // Delete
                                Rectangle {
                                    width: 26; height: 26; radius: 5
                                    color: delHover.containsMouse ? "#3A1010" : "transparent"
                                    Text { text: "✕"; color: "#E81123"; anchors.centerIn: parent; font.pixelSize: 13 }
                                    MouseArea { 
                                        id: delHover
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            if (backend && backend.selectedLayerIndex === index) backend.selectedLayerIndex = -1;
                                            if (backend) backend.sceneModel.remove(index);
                                        }
                                    }
                                }
                            }

                            states: State {
                                when: dragHandleMouse.drag.active
                                PropertyChanges { target: layerCard; parent: sceneList; z: 10 }
                            }
                        }
                    }
                }
            }
        }

        // Divider
        Rectangle {
            Layout.fillHeight: true
            width: 1
            color: "#222222"
        }

        // Middle Panel: Canvas
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10
            
            RowLayout {
                Layout.fillWidth: true
                Text {
                    text: "Canvas (128x40)"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                    Layout.fillWidth: true
                }
                
                SsSwitch {
                    id: gridSwitch
                    text: "Grid"
                    checked: true
                }
                
                Text { text: "Zoom:"; color: "#AAAAAA" }
                SsSlider {
                    id: zoomSlider
                    from: 1.0; to: 10.0; value: 4.0
                    Layout.preferredWidth: 100
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#050505"
                border.color: "#333333"
                border.width: 1
                clip: true

                Flickable {
                    id: flickable
                    anchors.fill: parent
                    contentWidth: canvasContainer.width
                    contentHeight: canvasContainer.height
                    boundsBehavior: Flickable.StopAtBounds

                    Item {
                        id: canvasContainer
                        width: Math.max(flickable.width, 128 * zoomSlider.value + 100)
                        height: Math.max(flickable.height, 40 * zoomSlider.value + 100)

                        Rectangle {
                            id: canvasRect
                            width: 128 * zoomSlider.value
                            height: 40 * zoomSlider.value
                            anchors.centerIn: parent
                            color: "black"

                            LivePreviewItem {
                                anchors.fill: parent
                            }

                            // Grid Overlay
                            Canvas {
                                id: gridCanvas
                                anchors.fill: parent
                                visible: gridSwitch.checked
                                onPaint: {
                                    var ctx = getContext("2d");
                                    ctx.clearRect(0, 0, width, height);
                                    ctx.strokeStyle = "rgba(255, 255, 255, 0.1)";
                                    ctx.lineWidth = 1;
                                    ctx.beginPath();
                                    
                                    var step = zoomSlider.value;
                                    if (step < 2) return; // Don't draw grid if too small
                                    
                                    for (var x = 0; x <= width; x += step) {
                                        ctx.moveTo(x, 0); ctx.lineTo(x, height);
                                    }
                                    for (var y = 0; y <= height; y += step) {
                                        ctx.moveTo(0, y); ctx.lineTo(width, y);
                                    }
                                    ctx.stroke();
                                }
                                
                                Connections {
                                    target: zoomSlider
                                    function onValueChanged() { gridCanvas.requestPaint(); }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Divider
        Rectangle {
            Layout.fillHeight: true
            width: 1
            color: "#222222"
        }

        // Right Panel: Properties
        ColumnLayout {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            spacing: 20
            
            Text {
                text: "Properties"
                font.pixelSize: 18
                font.bold: true
                color: "#ffffff"
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true

                ColumnLayout {
                    width: parent.width
                    spacing: 15
                    
                    Text {
                        text: "Select a layer to edit its properties."
                        color: "#555555"
                        font.pixelSize: 14
                        visible: backend.selectedLayerIndex === -1
                    }

                    // Only show properties if a layer is selected
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 15
                        visible: backend.selectedLayerIndex !== -1

                         SsCard {
                             Layout.fillWidth: true
                             implicitHeight: 200

                             ColumnLayout {
                                 anchors.fill: parent
                                 anchors.margins: 20
                                 spacing: 15

                                 Text { text: "Transform"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16 }

                                 RowLayout {
                                     Text { text: "Name"; color: "#CCCCCC"; Layout.preferredWidth: 80 }
                                     SsInput {
                                         id: nameInput
                                         Layout.fillWidth: true
                                         text: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 1) : ""
                                         onEditingFinished: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.rename(backend.selectedLayerIndex, text)
                                         }
                                     }
                                 }

                                 RowLayout {
                                     Text { text: "X Pos"; color: "#CCCCCC"; Layout.preferredWidth: 80 }
                                     SsInput {
                                         id: xInput
                                         Layout.fillWidth: true
                                         text: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 4).toString() : "0"
                                         validator: IntValidator { bottom: -500; top: 500 }
                                         onEditingFinished: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), parseInt(text) || 0, Qt.UserRole + 4)
                                         }
                                     }
                                 }

                                 RowLayout {
                                     Text { text: "Y Pos"; color: "#CCCCCC"; Layout.preferredWidth: 80 }
                                     SsInput {
                                         id: yInput
                                         Layout.fillWidth: true
                                         text: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 5).toString() : "0"
                                         validator: IntValidator { bottom: -500; top: 500 }
                                         onEditingFinished: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), parseInt(text) || 0, Qt.UserRole + 5)
                                         }
                                     }
                                 }
                             }
                         }

                         SsCard {
                             Layout.fillWidth: true
                             implicitHeight: 350
                             visible: backend.selectedLayerIndex !== -1

                             ColumnLayout {
                                 anchors.fill: parent
                                 anchors.margins: 20
                                 spacing: 12

                                 RowLayout {
                                     Text { text: "Audio Reactivity"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16; Layout.fillWidth: true }
                                     SsSwitch {
                                         checked: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 7) : false
                                         onToggled: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), checked, Qt.UserRole + 7)
                                         }
                                     }
                                 }

                                 ColumnLayout {
                                     Layout.fillWidth: true
                                     spacing: 10
                                     enabled: backend.selectedLayerIndex !== -1 && backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 7)
                                     opacity: enabled ? 1.0 : 0.5

                                     Text { text: "React To"; color: "#CCCCCC"; font.pixelSize: 14 }
                                     ComboBox {
                                         Layout.fillWidth: true
                                         model: ["None", "Move Up", "Move Right", "Scale Height", "Scale Width", "Pulse Visibility"]
                                         currentIndex: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 12) : 0
                                         onActivated: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), index, Qt.UserRole + 12)
                                         }
                                     }

                                     Text { text: "Sensitivity: " + sensSlider.value.toFixed(1) + "x"; color: "#CCCCCC"; font.pixelSize: 14 }
                                     SsSlider {
                                         id: sensSlider
                                         Layout.fillWidth: true
                                         from: 0.1; to: 10.0; stepSize: 0.1
                                         value: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 10) : 1.0
                                         onMoved: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), value, Qt.UserRole + 10)
                                         }
                                     }

                                     Text { text: "Smoothing: " + lerpSlider.value.toFixed(2); color: "#CCCCCC"; font.pixelSize: 14 }
                                     SsSlider {
                                         id: lerpSlider
                                         Layout.fillWidth: true
                                         from: 0.01; to: 1.0; stepSize: 0.01
                                         value: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 11) : 0.2
                                         onMoved: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), value, Qt.UserRole + 11)
                                         }
                                     }

                                     Text { text: "Band Range: " + Math.round(bandSlider.first.value) + " - " + Math.round(bandSlider.second.value); color: "#CCCCCC"; font.pixelSize: 14 }
                                     RangeSlider {
                                         id: bandSlider
                                         Layout.fillWidth: true
                                         from: 0; to: 63; stepSize: 1
                                         first.value: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 8) : 0
                                         second.value: backend.selectedLayerIndex !== -1 ? backend.sceneModel.data(backend.sceneModel.index(backend.selectedLayerIndex, 0), Qt.UserRole + 9) : 11
                                         
                                         first.onMoved: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), Math.round(first.value), Qt.UserRole + 8)
                                         }
                                         second.onMoved: {
                                             if (backend.selectedLayerIndex !== -1)
                                                 backend.sceneModel.setData(backend.sceneModel.index(backend.selectedLayerIndex, 0), Math.round(second.value), Qt.UserRole + 9)
                                         }
                                     }
                                 }
                             }
                         }
                     }
                 }
             }
         }
     }
 }
