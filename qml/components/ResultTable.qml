import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Заголовки таблицы
        HorizontalHeaderView {
            id: header
            syncView: tableView
            Layout.fillWidth: true
            
            delegate: Rectangle {
                implicitWidth: 150
                implicitHeight: 40
                color: "#2a2a2a"
                border.color: "#444444"
                
                Text {
                    anchors.centerIn: parent
                    text: display
                    color: "#00BCD4"
                    font.bold: true
                    font.pixelSize: 14
                }
            }
        }

        // Сама таблица
        TableView {
            id: tableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            // Подключаем модель из Python
            model: backend.tableModel
            
            columnWidthProvider: function(column) {
                return column === 0 ? 80 : 150 // Первый столбец (i) делаем поуже
            }

            delegate: Rectangle {
                implicitHeight: 35
                color: (row % 2 === 0) ? "#1e1e1e" : "#252525" // Полосатая таблица
                border.color: "#333333"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: display
                    color: "white"
                    font.pixelSize: 13
                }
            }
        }
    }
}