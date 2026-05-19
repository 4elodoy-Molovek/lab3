import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "components"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200
    height: 800
    title: "Лабораторная №2: Краевые задачи для ОДУ"
    minimumWidth: 900
    minimumHeight: 600

    // Настраиваем строгую темную тему с бирюзовым акцентом
    Material.theme: Material.Dark
    Material.accent: Material.Teal
    Material.primary: Material.BlueGrey

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Левая панель с настройками
        InputPanel {
            id: inputPanel
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            
            // Обработчик нажатия на кнопку "Рассчитать"
            onCalculateRequested: function(n, taskId) {
                // Вызываем Python-метод, передаем туда N, ID задачи и объекты графиков
                backend.calculate(n, taskId, chartPanel.seriesV1, chartPanel.seriesV2, chartPanel.seriesDiff)
            }
        }

        // Правая часть (Графики / Таблица)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            radius: 8
            border.color: "#333333"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8

                // Главный переключатель Режимов
                TabBar {
                    id: mainTabBar
                    Layout.fillWidth: true
                    TabButton { text: "Визуализация (Графики)" }
                    TabButton { text: "Сырые данные (Таблица)" }
                }

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: mainTabBar.currentIndex

                    // Индекс 0 - Графики
                    ChartPanel {
                        id: chartPanel
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    // Индекс 1 - Таблица
                    ResultTable {
                        id: resultTable
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
    
    // Связываем Python-сигнал обновления текста с панелью ввода
    Connections {
        target: backend
        function onUpdateStats(statsText) {
            inputPanel.statsText = statsText
        }

        function onUpdateAxes(minY, maxY, maxDiff) {
            chartPanel.resetZoom() 
            
            chartPanel.axisY1.min = minY
            chartPanel.axisY1.max = maxY
            chartPanel.axisY2.max = maxDiff
        }
    }
}