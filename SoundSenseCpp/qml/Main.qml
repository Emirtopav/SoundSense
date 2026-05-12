import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import SoundSense 1.0
import "components"

Window {
    id: mainWindow
    width: 900
    height: 600
    visible: true
    title: "SoundSense Dashboard"
    color: "#111111" // Dark background
    
    flags: Qt.Window | Qt.FramelessWindowHint

    property int currentTab: 0

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Custom Title Bar
        Rectangle {
            id: titleBar
            Layout.fillWidth: true
            height: 32
            color: "#1A1A1A"
            
            TapHandler {
                onTapped: if (tapCount === 2) mainWindow.visibility = mainWindow.visibility === Window.Maximized ? Window.Windowed : Window.Maximized
            }
            DragHandler {
                onActiveChanged: if (active) mainWindow.startSystemMove()
            }

            RowLayout {
                anchors.fill: parent
                spacing: 0

                Item { width: 10 } // Spacer

                Text {
                    text: "SOUNDSENSE"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                    font.letterSpacing: 2
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }

                Item { Layout.fillWidth: true } // pushes buttons to right

                // Minimize
                Rectangle {
                    width: 46
                    height: 32
                    color: minHover.hovered ? "#333333" : "transparent"
                    Text { text: "—"; color: "#fff"; anchors.centerIn: parent }
                    HoverHandler { id: minHover }
                    TapHandler { onTapped: mainWindow.showMinimized() }
                }

                // Close
                Rectangle {
                    width: 46
                    height: 32
                    color: closeHover.hovered ? "#E81123" : "transparent"
                    Text { text: "✕"; color: "#fff"; anchors.centerIn: parent }
                    HoverHandler { id: closeHover }
                    TapHandler { onTapped: Qt.quit() }
                }
            }
        }

        // Main Content Area
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Sidebar
            Rectangle {
                Layout.fillHeight: true
                width: 220
                color: "#1A1A1A"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.topMargin: 20
                    spacing: 5

                    // Tab Buttons
                    Rectangle {
                        Layout.fillWidth: true
                        height: 44
                        color: mainWindow.currentTab === 0 ? "#1E1E1E" : (tab1Hover.hovered ? "#161616" : "transparent")
                        HoverHandler { id: tab1Hover }
                        TapHandler { onTapped: mainWindow.currentTab = 0 }
                        
                        // Accent line
                        Rectangle {
                            width: 3; height: 16
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#0078D7"
                            visible: mainWindow.currentTab === 0
                            radius: 2
                        }

                        Text {
                            text: "Audio Settings"
                            color: mainWindow.currentTab === 0 ? "#FFFFFF" : "#888888"
                            font.pixelSize: 13
                            font.bold: mainWindow.currentTab === 0
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 24
                        }
                    }

                    Item { Layout.fillHeight: true } // pushes preview to bottom
                    
                    // Live Preview at the bottom of the sidebar
                    Item {
                        Layout.fillWidth: true
                        height: 120
                        
                        Text {
                            text: "OLED Preview"
                            color: "#888888"
                            font.pixelSize: 11
                            font.bold: true
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Rectangle {
                            anchors.centerIn: parent
                            width: 128 * 1.6
                            height: 40 * 1.6
                            color: "black"
                            radius: 4
                            border.color: "#333333"
                            border.width: 1
                            clip: true

                            LivePreviewItem {
                                anchors.fill: parent
                            }
                            
                            // Signature metadata overlay
                            Rectangle {
                                anchors.fill: parent
                                color: "transparent"
                                
                                Text {
                                    text: (backend ? (backend.mode === 0 ? "STREAK" : backend.mode === 1 ? "SYMMETRIC" : "DOTS") : "N/A")
                                    color: "#444444"
                                    font.pixelSize: 8
                                    font.weight: Font.Bold
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    anchors.margins: 4
                                }

                                Text {
                                    text: "64-BAND REALTIME"
                                    color: "#444444"
                                    font.pixelSize: 8
                                    font.weight: Font.Bold
                                    anchors.bottom: parent.bottom
                                    anchors.left: parent.left
                                    anchors.margins: 4
                                }

                                Text {
                                    text: (backend ? backend.fps : 0) + "Hz"
                                    color: "#00FF00"
                                    font.pixelSize: 9
                                    font.weight: Font.Black
                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    anchors.margins: 4
                                }
                            }
                        }
                    }
                    
                    Item { Layout.preferredHeight: 20 }
                }
            }

            // Divider
            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: "#222222"
            }

            // Right View (StackLayout for tabs)
            StackLayout {
                currentIndex: mainWindow.currentTab
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Tab 0: Audio Settings
                ScrollView {
                    contentWidth: availableWidth
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 40
                        spacing: 20

                        Text {
                            text: "Audio Visualization Settings"
                            color: "#FFFFFF"
                            font.pixelSize: 28
                            font.weight: Font.Black
                            Layout.bottomMargin: 2
                        }
                        
                        Text {
                            text: "Fine-tune the DSP engine parameters for optimal visual responsiveness."
                            color: "#888888"
                            font.pixelSize: 13
                            Layout.bottomMargin: 15
                        }

                        SsCard {
                            Layout.fillWidth: true
                            implicitHeight: 220
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 15

                                RowLayout {
                                    Layout.fillWidth: true
                                    Text { text: "Sensitivity & Gain"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16; Layout.fillWidth: true }
                                    
                                    Rectangle {
                                        width: 70; height: 24; radius: 4
                                        color: resetAllHover.hovered ? "#333333" : "#222222"
                                        Text { text: "Reset All"; color: "#888888"; anchors.centerIn: parent; font.pixelSize: 10; font.bold: true }
                                        HoverHandler { id: resetAllHover }
                                        TapHandler { onTapped: if (backend) backend.resetSettings() }
                                    }
                                }

                                RowLayout {
                                    Text { text: "Global Sens"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0.1; to: 5.0
                                        value: backend ? backend.globalSens : 1.0
                                        onValueChanged: if (backend) backend.globalSens = value
                                    }
                                    Text { text: backend ? Number(backend.globalSens).toFixed(2) : "1.00"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40; horizontalAlignment: Text.AlignRight }
                                    
                                    Rectangle {
                                        width: 24; height: 24; radius: 4
                                        color: resetGlobalHover.hovered ? "#333333" : "transparent"
                                        Text { text: "↺"; color: "#888888"; anchors.centerIn: parent; font.pixelSize: 14 }
                                        HoverHandler { id: resetGlobalHover }
                                        TapHandler { onTapped: if (backend) backend.globalSens = 1.0 }
                                    }
                                }

                                RowLayout {
                                    Text { text: "Bass Gain"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0.1; to: 5.0
                                        value: backend ? backend.bassGain : 1.0
                                        onValueChanged: if (backend) backend.bassGain = value
                                    }
                                    Text { text: backend ? Number(backend.bassGain).toFixed(2) : "1.00"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40; horizontalAlignment: Text.AlignRight }
                                    
                                    Rectangle {
                                        width: 24; height: 24; radius: 4
                                        color: resetBassHover.hovered ? "#333333" : "transparent"
                                        Text { text: "↺"; color: "#888888"; anchors.centerIn: parent; font.pixelSize: 14 }
                                        HoverHandler { id: resetBassHover }
                                        TapHandler { onTapped: if (backend) backend.bassGain = 1.0 }
                                    }
                                }

                                RowLayout {
                                    Text { text: "Mid Gain"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0.1; to: 5.0
                                        value: backend ? backend.midGain : 1.0
                                        onValueChanged: if (backend) backend.midGain = value
                                    }
                                    Text { text: backend ? Number(backend.midGain).toFixed(2) : "1.00"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40; horizontalAlignment: Text.AlignRight }
                                    
                                    Rectangle {
                                        width: 24; height: 24; radius: 4
                                        color: resetMidHover.hovered ? "#333333" : "transparent"
                                        Text { text: "↺"; color: "#888888"; anchors.centerIn: parent; font.pixelSize: 14 }
                                        HoverHandler { id: resetMidHover }
                                        TapHandler { onTapped: if (backend) backend.midGain = 1.0 }
                                    }
                                }

                                RowLayout {
                                    Text { text: "Treble Gain"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0.1; to: 5.0
                                        value: backend ? backend.trebleGain : 1.0
                                        onValueChanged: if (backend) backend.trebleGain = value
                                    }
                                    Text { text: backend ? Number(backend.trebleGain).toFixed(2) : "1.00"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40; horizontalAlignment: Text.AlignRight }
                                    
                                    Rectangle {
                                        width: 24; height: 24; radius: 4
                                        color: resetTrebleHover.hovered ? "#333333" : "transparent"
                                        Text { text: "↺"; color: "#888888"; anchors.centerIn: parent; font.pixelSize: 14 }
                                        HoverHandler { id: resetTrebleHover }
                                        TapHandler { onTapped: if (backend) backend.trebleGain = 1.0 }
                                    }
                                }
                            }
                        }

                        SsCard {
                            Layout.fillWidth: true
                            implicitHeight: 220
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 15

                                RowLayout {
                                    Layout.fillWidth: true
                                    Text { text: "DSP Presets"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16; Layout.fillWidth: true }
                                    
                                    SsTextField {
                                        id: presetNameInput
                                        placeholderText: "Preset Name..."
                                        Layout.preferredWidth: 150
                                    }
                                    
                                    SsButton {
                                        text: "Save New"
                                        Layout.preferredHeight: 30
                                        onClicked: {
                                            if (presetNameInput.text !== "") {
                                                backend.savePreset(presetNameInput.text)
                                                presetNameInput.text = ""
                                            }
                                        }
                                    }
                                }
                                
                                GridView {
                                    id: presetGrid
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    cellWidth: 140
                                    cellHeight: 50
                                    clip: true
                                    model: backend ? backend.presetModel : null
                                    
                                    delegate: Rectangle {
                                        width: 130; height: 40
                                        color: presetHover.hovered ? "#222222" : "#1A1A1A"
                                        radius: 6
                                        border.color: "#333333"
                                        
                                        Text {
                                            anchors.centerIn: parent
                                            text: model.name
                                            color: "#FFFFFF"
                                            font.pixelSize: 12
                                            font.bold: true
                                        }
                                        
                                        HoverHandler { id: presetHover }
                                        TapHandler { onTapped: backend.applyPreset(model.name) }
                                    }
                                }
                            }
                        }

                        SsCard {
                            Layout.fillWidth: true
                            implicitHeight: 280
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 15

                                Text { text: "Hardware Sync (ESP32)"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16 }
                                
                                RowLayout {
                                    spacing: 10
                                    Text { text: "Serial Port"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    SsComboBox {
                                        id: portSelector
                                        model: backend ? backend.availablePorts : []
                                        Layout.fillWidth: true
                                    }
                                    SsButton {
                                        text: "↻"
                                        Layout.preferredWidth: 30
                                        onClicked: if (backend) backend.refreshPorts()
                                    }
                                    SsButton {
                                        text: (backend && backend.espConnected) ? "Disconnect" : "Connect"
                                        Layout.preferredWidth: 100
                                        onClicked: {
                                            if (backend && backend.espConnected) backend.disconnectSerial()
                                            else if (backend) backend.connectSerial(portSelector.currentText)
                                        }
                                    }
                                }

                                RowLayout {
                                    Text { text: "Brightness"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0; to: 255
                                        value: backend ? backend.brightness : 128
                                        onValueChanged: if (backend) backend.brightness = value
                                    }
                                    Text { text: backend ? backend.brightness : "128"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40; horizontalAlignment: Text.AlignRight }
                                }

                                RowLayout {
                                    Text { text: "Rotation"; color: "#888888"; font.pixelSize: 13; Layout.preferredWidth: 100 }
                                    RowLayout {
                                        spacing: 5
                                        Layout.fillWidth: true
                                        SsButton { 
                                            text: "0°"; Layout.fillWidth: true; 
                                            highlighted: backend && backend.rotation === 0
                                            onClicked: if (backend) backend.rotation = 0 
                                        }
                                        SsButton { 
                                            text: "90°"; Layout.fillWidth: true; 
                                            highlighted: backend && backend.rotation === 90
                                            onClicked: if (backend) backend.rotation = 90 
                                        }
                                        SsButton { 
                                            text: "180°"; Layout.fillWidth: true; 
                                            highlighted: backend && backend.rotation === 180
                                            onClicked: if (backend) backend.rotation = 180 
                                        }
                                        SsButton { 
                                            text: "270°"; Layout.fillWidth: true; 
                                            highlighted: backend && backend.rotation === 270
                                            onClicked: if (backend) backend.rotation = 270 
                                        }
                                    }
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    height: 1
                                    color: "#222222"
                                }

                                RowLayout {
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        Text { text: "Firmware Update"; color: "#FFFFFF"; font.pixelSize: 12; font.bold: true }
                                        Text { text: "Flash the latest SoundSense firmware to your ESP32."; color: "#666666"; font.pixelSize: 10 }
                                    }
                                    SsButton {
                                        text: "Flash Firmware"
                                        onClicked: if (backend) backend.flashFirmware()
                                    }
                                }
                            }
                        }

                        SsCard {
                            Layout.fillWidth: true
                            implicitHeight: 180
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 15

                                Text { text: "Creative Engine (Modulation)"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16 }
                                
                                RowLayout {
                                    spacing: 20
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        Text { text: "Sensitivity Pulse (LFO)"; color: "#888888"; font.pixelSize: 13 }
                                        RowLayout {
                                            Text { text: "Freq"; color: "#666666"; font.pixelSize: 10; Layout.preferredWidth: 30 }
                                            SsSlider {
                                                Layout.fillWidth: true
                                                from: 0.1; to: 10.0
                                                value: backend ? backend.lfoFreq : 1.0
                                                onValueChanged: if (backend) backend.lfoFreq = value
                                            }
                                        }
                                        RowLayout {
                                            Text { text: "Amp"; color: "#666666"; font.pixelSize: 10; Layout.preferredWidth: 30 }
                                            SsSlider {
                                                Layout.fillWidth: true
                                                from: 0.0; to: 2.0
                                                value: backend ? backend.lfoAmp : 0.0
                                                onValueChanged: if (backend) backend.lfoAmp = value
                                            }
                                        }
                                    }

                                    Rectangle {
                                        width: 1; Layout.fillHeight: true; color: "#222222"
                                    }

                                    ColumnLayout {
                                        Layout.preferredWidth: 100
                                        Text { text: "Live Triggers"; color: "#888888"; font.pixelSize: 13; Layout.alignment: Qt.AlignHCenter }
                                        RowLayout {
                                            Layout.alignment: Qt.AlignHCenter
                                            spacing: 15
                                            ColumnLayout {
                                                spacing: 5
                                                Rectangle { 
                                                    width: 12; height: 12; radius: 6; 
                                                    color: (backend && backend.audioActive) ? "#FF0000" : "#222222"
                                                    opacity: (backend && backend.audioActive) ? 1.0 : 0.3
                                                }
                                                Text { text: "BASS"; color: "#666666"; font.pixelSize: 9; Layout.alignment: Qt.AlignHCenter }
                                            }
                                            ColumnLayout {
                                                spacing: 5
                                                Rectangle { 
                                                    width: 12; height: 12; radius: 6; 
                                                    color: (backend && backend.audioActive) ? "#00FF00" : "#222222"
                                                    opacity: (backend && backend.audioActive) ? 0.8 : 0.3
                                                }
                                                Text { text: "MID"; color: "#666666"; font.pixelSize: 9; Layout.alignment: Qt.AlignHCenter }
                                            }
                                            ColumnLayout {
                                                spacing: 5
                                                Rectangle { 
                                                    width: 12; height: 12; radius: 6; 
                                                    color: (backend && backend.audioActive) ? "#0078D7" : "#222222"
                                                    opacity: (backend && backend.audioActive) ? 0.8 : 0.3
                                                }
                                                Text { text: "HIGH"; color: "#666666"; font.pixelSize: 9; Layout.alignment: Qt.AlignHCenter }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        SsCard {
                            Layout.fillWidth: true
                            implicitHeight: 320

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 15

                                Text { text: "Rendering Pipeline & Plugins"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16 }
                                
                                Text { 
                                    text: "Visualizers are rendered top-to-bottom. Filters (like Invert) should be at the end."
                                    color: "#888888"; font.pixelSize: 11 
                                }

                                ListView {
                                    id: pluginList
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    model: backend ? backend.pluginModel : null
                                    spacing: 8
                                    
                                    delegate: Rectangle {
                                        width: pluginList.width
                                        height: pluginExpanded.hovered ? 60 : 44
                                        color: "#1A1A1A"
                                        radius: 6
                                        border.color: model.enabled ? "#333333" : "#222222"
                                        
                                        Behavior on height { NumberAnimation { duration: 150 } }

                                        HoverHandler { id: pluginExpanded }

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 10
                                            spacing: 12
                                            
                                            SsSwitch {
                                                checked: model.enabled
                                                onCheckedChanged: model.enabled = checked
                                            }
                                            
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 2
                                                Text {
                                                    text: model.name
                                                    color: model.enabled ? "#FFFFFF" : "#555555"
                                                    font.bold: true
                                                    font.pixelSize: 13
                                                }
                                                Text {
                                                    text: model.version + " by " + model.author
                                                    color: "#444444"
                                                    font.pixelSize: 9
                                                    visible: pluginExpanded.hovered
                                                }
                                            }
                                            
                                            RowLayout {
                                                spacing: 4
                                                SsButton {
                                                    text: "↑"
                                                    Layout.preferredWidth: 26
                                                    Layout.preferredHeight: 26
                                                    onClicked: backend.pluginModel.moveUp(index)
                                                }
                                                SsButton {
                                                    text: "↓"
                                                    Layout.preferredWidth: 26
                                                    Layout.preferredHeight: 26
                                                    onClicked: backend.pluginModel.moveDown(index)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        SsCard {
                            Layout.fillWidth: true
                            implicitHeight: 200

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                spacing: 15

                                Text { text: "Behavior"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 16 }

                                RowLayout {
                                    Text { text: "Falloff Speed"; color: "#CCCCCC"; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0.01; to: 1.0
                                        value: backend ? backend.falloff : 0.4
                                        onValueChanged: if (backend) backend.falloff = value
                                    }
                                    Text { text: backend ? Number(backend.falloff).toFixed(2) : "0.00"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40 }
                                }

                                RowLayout {
                                    Text { text: "Smoothing"; color: "#CCCCCC"; Layout.preferredWidth: 100 }
                                    SsSlider {
                                        Layout.fillWidth: true
                                        from: 0.0; to: 1.0
                                        value: backend ? backend.smoothing : 0.5
                                        onValueChanged: if (backend) backend.smoothing = value
                                    }
                                    Text { text: backend ? Number(backend.smoothing).toFixed(2) : "0.00"; color: "#0078D7"; font.bold: true; Layout.preferredWidth: 40 }
                                }

                                RowLayout {
                                    SsSwitch {
                                        text: "Show Peaks"
                                        checked: backend ? backend.showPeaks : true
                                        onCheckedChanged: if (backend) backend.showPeaks = checked
                                    }
                                }
                            }
                        }
                        
                        Item { Layout.fillHeight: true } // Push content up
                    }
                }
            }
        }
        
        // Global Status Bar
        Rectangle {
            Layout.fillWidth: true
            height: 24
            color: "#0D0D0D"
            border.color: "#222222"
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 15
                
                Text {
                    text: backend ? (backend.ssConnected ? "● SteelSeries" : "○ SteelSeries") : "○ SteelSeries"
                    color: (backend && backend.ssConnected) ? "#00FF00" : "#E81123"
                    font.pixelSize: 11
                    font.bold: true
                }
                
                Text {
                    text: backend ? (backend.espConnected ? "● ESP32" : "○ ESP32") : "○ ESP32"
                    color: (backend && backend.espConnected) ? "#00FF00" : "#E81123"
                    font.pixelSize: 11
                    font.bold: true
                }
                
                Text {
                    text: backend ? (backend.audioActive ? "🔊 ACTIVE" : "🔇 IDLE") : "🔇 IDLE"
                    color: (backend && backend.audioActive) ? "#0078D7" : "#555555"
                    font.pixelSize: 11
                    font.bold: true
                }
                
                Item { Layout.fillWidth: true } // Spacer
                
                Text {
                    text: "Render: " + (backend ? backend.renderTimeMs : 0) + "ms"
                    color: "#888888"
                    font.pixelSize: 10
                }

                Text {
                    text: "Dropped: " + (backend ? backend.droppedFrames : 0)
                    color: (backend && backend.droppedFrames > 0) ? "#E81123" : "#444444"
                    font.pixelSize: 10
                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (backend) backend.resetDroppedFrames()
                    }
                }

                Text {
                    text: "Latency: " + (backend ? backend.latency : 0) + "ms"
                    color: (backend && backend.latency > 20) ? "#FFA500" : "#888888"
                    font.pixelSize: 10
                }
                
                Text {
                    text: (backend ? backend.fps : 0) + " FPS"
                    color: (backend && backend.fps >= 55) ? "#00FF00" : "#FFA500"
                    font.pixelSize: 11
                    font.bold: true
                }
            }
        }
    }
}
