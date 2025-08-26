import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

Window {
    id: root
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Tool
    color: "transparent"
    width: Screen.width
    height: Screen.height

    Rectangle {
        id: container
        width: Math.min(parent.width * 0.8, 5 * 220 + 4 * 20) // 5 images + 4 spaces
        height: 180
        x: (parent.width - width)/2
        y: parent.height - height - 40
        radius: 14
        color: Qt.rgba(1,1,1,0.00)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        opacity: 0.98

        ListView {
            id: carousel
            anchors.fill: parent
            model: infiniteModel
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            snapMode: ListView.SnapToItem
            highlightRangeMode: ListView.StrictlyEnforceRange
            spacing: 20
            focus: true
            clip: true
            currentIndex: 100000

            preferredHighlightBegin: width / 2 - 110
            preferredHighlightEnd: width / 2 - 110

            delegate: Item {
                width: 220
                height: carousel.height

                Image {
                    anchors.fill: parent
                    source: (cachedUrl ? cachedUrl : url)
                    fillMode: Image.PreserveAspectCrop
                    smooth: true
                    id: img
                    property bool rounded: true

                    layer.enabled: rounded
                    layer.effect: OpacityMask {
                        maskSource: Item {
                            width: img.width
                            height: img.height
                            Rectangle {
                                anchors.centerIn: parent
                                width: 220
                                height: carousel.height
                                radius: Math.min(width/10, height/10)
                            }
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        carousel.currentIndex = index
                    }
                }
            }

            Keys.onLeftPressed: { currentIndex-- }
            Keys.onRightPressed: { currentIndex++ }
            Keys.onReturnPressed: {
                var realUrl = infiniteModel.urlAt(currentIndex)
                if (realUrl) {
                    controller.applyWallpaperForScreen(screenHelper.currentScreenIndex, realUrl);
                }
                root.close()
            }
            Keys.onEscapePressed: { root.close() }
        }
    }
}

