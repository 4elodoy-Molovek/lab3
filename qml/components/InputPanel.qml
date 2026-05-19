import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item {
    id: root
    
    // Сигнал, который ловит main.qml
    signal calculateRequested(int n, int taskId)
    
    // Свойство для обновления текста справки из Python
    property string statsText: "Введите параметры и нажмите «Рассчитать»"

    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        Label {
            text: "Параметры задачи"
            font.pixelSize: 20
            font.bold: true
            color: Material.accent
        }

        TextField {
            id: inputN
            Layout.fillWidth: true
            placeholderText: "Число разбиений (N)"
            text: "10"
            validator: IntValidator { bottom: 2; top: 1000000 }
            selectByMouse: true
        }

        ComboBox {
            id: comboTask
            Layout.fillWidth: true
            model: [
                "1. Первая краевая (Тестовая)",
                "2. Первая краевая (Основная)",
                "3. Смешанная (Тестовая)",
                "4. Смешанная (Основная)"
            ]
        }

        Button {
            text: "РАССЧИТАТЬ"
            Layout.fillWidth: true
            highlighted: true
            font.bold: true
            onClicked: {
                var n = parseInt(inputN.text)
                var taskId = comboTask.currentIndex + 1
                root.calculateRequested(n, taskId)
            }
        }

        // Разделитель
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#444444"
        }

        Label {
            text: "Справка по расчету"
            font.pixelSize: 16
            font.bold: true
        }

        // Текстовый блок для вывода epsilon_1 или epsilon_2
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            TextArea {
                text: root.statsText
                readOnly: true
                wrapMode: Text.WordWrap
                color: "#cccccc"
                font.pixelSize: 14
                background: null // Убираем рамку
            }
        }
    }
}