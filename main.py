import sys
import os
from PySide6.QtWidgets import QApplication
from PySide6.QtQml import QQmlApplicationEngine
from src.backend import SolverBackend

def main():
    os.environ["QT_QUICK_CONTROLS_STYLE"] = "Material"

    # Инициализация полноценного приложения
    app = QApplication(sys.argv)

    backend = SolverBackend()

    engine = QQmlApplicationEngine()
    engine.rootContext().setContextProperty("backend", backend)

    # Строим абсолютный путь до main.qml
    qml_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), "qml", "main.qml")
    
    if not os.path.exists(qml_file):
        print(f"КРИТИЧЕСКАЯ ОШИБКА: Файл не найден по пути:\n{qml_file}")
        sys.exit(-1)

    # Функция для отлова скрытых ошибок QML
    def handle_warnings(warnings):
        for w in warnings:
            print("ОШИБКА QML:", w.toString())
            
    engine.warnings.connect(handle_warnings)

    # Загружаем интерфейс
    engine.load(qml_file)

    if not engine.rootObjects():
        print("ОШИБКА: Интерфейс не загрузился. Проверь ошибки QML выше.")
        sys.exit(-1)

    # Запуск основного цикла
    sys.exit(app.exec())

if __name__ == "__main__":
    main()