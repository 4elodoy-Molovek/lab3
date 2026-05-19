import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

Item {
    id: root
    
    // Экспортируем элементы наружу
    property alias seriesV1: lineV1
    property alias seriesV2: lineV2
    property alias seriesDiff: lineDiff
    property alias axisY1: axisY1
    property alias axisY2: axisY2

    // Текущий масштаб (100% = начальный)
    property int zoomLevel: 100

    // Функция для сброса масштаба
    function resetZoom() {
        zoomLevel = 100
        chart1.zoomReset()
        chart2.zoomReset()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: chartTabBar
            Layout.fillWidth: true
            TabButton { text: "Решения u(x) и v(x)" }
            TabButton { text: "Погрешность |v1 - v2|" }
        }

        // Контейнер для графиков и оверлея
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            StackLayout {
                id: stack
                anchors.fill: parent
                currentIndex: chartTabBar.currentIndex

                // ================= ГРАФИК 1 =================
                ChartView {
                    id: chart1
                    title: "Сравнение решений"
                    titleColor: "white"
                    backgroundColor: "transparent"
                    legend.labelColor: "white"
                    antialiasing: true

                    ValueAxis { id: axisX1; min: 0; max: 1; titleText: "x"; labelsColor: "white"; gridLineColor: "#333333" }
                    ValueAxis { id: axisY1; min: 0; max: 1; titleText: "v(x) / u(x)"; labelsColor: "white"; gridLineColor: "#333333" }

                    LineSeries {
                        id: lineV1
                        name: "v(x) [Сетка n]"
                        axisX: axisX1; axisY: axisY1
                        color: "#00BCD4"
                        width: 2
                    }
                    LineSeries {
                        id: lineV2
                        name: "u(x) [или Сетка 2n]"
                        axisX: axisX1; axisY: axisY1
                        color: "#FF5252"
                        width: 2
                    }
                }

                // ================= ГРАФИК 2 =================
                ChartView {
                    id: chart2
                    title: "Распределение погрешности"
                    titleColor: "white"
                    backgroundColor: "transparent"
                    legend.labelColor: "white"
                    antialiasing: true

                    ValueAxis { id: axisX2; min: 0; max: 1; titleText: "x"; labelsColor: "white"; gridLineColor: "#333333" }
                    ValueAxis { id: axisY2; min: 0; max: 0.1; titleText: "Ошибка"; labelsColor: "white"; gridLineColor: "#333333" }

                    LineSeries {
                        id: lineDiff
                        name: "|v1 - v2|"
                        axisX: axisX2; axisY: axisY2
                        color: "#FFC107"
                        width: 2
                    }
                }
            }

            // ================= ИНТЕРАКТИВ (ПЕРЕТАСКИВАНИЕ) =================
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true // Нужно для отображения открытой ладони при наведении
                cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                
                property real lastX: 0
                property real lastY: 0

                onPressed: function(mouse) {
                    lastX = mouse.x
                    lastY = mouse.y
                }

                onPositionChanged: function(mouse) {
                    if (pressed) {
                        var dx = mouse.x - lastX
                        var dy = mouse.y - lastY
                        
                        var currentChart = (stack.currentIndex === 0) ? chart1 : chart2
                        currentChart.scrollLeft(dx)
                        currentChart.scrollUp(dy)
                        
                        lastX = mouse.x
                        lastY = mouse.y
                    }
                }

                // Зум на колесико мыши
                onWheel: function(wheel) {
                    var currentChart = (stack.currentIndex === 0) ? chart1 : chart2
                    if (wheel.angleDelta.y > 0) {
                        currentChart.zoomIn()
                        root.zoomLevel *= 2
                    } else {
                        if (root.zoomLevel > 25) { // Ограничиваем сильное отдаление
                            currentChart.zoomOut()
                            root.zoomLevel /= 2
                        }
                    }
                }
            }

            // ================= ПАНЕЛЬ МАСШТАБА =================
            RowLayout {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 16
                spacing: 6

                Button {
                    text: "—" // Минус
                    implicitWidth: 40
                    background: Rectangle { color: "#2d2d2d"; radius: 4; border.color: "#444" }
                    contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    onClicked: {
                        if (root.zoomLevel > 25) {
                            var currentChart = (stack.currentIndex === 0) ? chart1 : chart2
                            currentChart.zoomOut()
                            root.zoomLevel /= 2
                        }
                    }
                }

                Rectangle {
                    width: 70
                    height: 36
                    color: "#1e1e1e"
                    radius: 4
                    border.color: "#444"
                    Text {
                        anchors.centerIn: parent
                        text: root.zoomLevel + "%"
                        color: "#00BCD4"
                        font.bold: true
                        font.pixelSize: 14
                    }
                }

                Button {
                    text: "+"
                    implicitWidth: 40
                    background: Rectangle { color: "#2d2d2d"; radius: 4; border.color: "#444" }
                    contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    onClicked: {
                        var currentChart = (stack.currentIndex === 0) ? chart1 : chart2
                        currentChart.zoomIn()
                        root.zoomLevel *= 2
                    }
                }

                Button {
                    text: "100%"
                    implicitWidth: 50
                    background: Rectangle { color: "#00BCD4"; radius: 4 }
                    contentItem: Text { text: parent.text; color: "black"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    onClicked: root.resetZoom()
                }
            }
        }
    }
}