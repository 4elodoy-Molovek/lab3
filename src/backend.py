import os
import platform
import numpy as np
from PySide6.QtCore import QObject, Slot, Signal, QPointF, QAbstractTableModel, Qt, QModelIndex, Property
from src.task_manager import TaskManager

class ResultTableModel(QAbstractTableModel):
    """Быстрая модель таблицы, которая читает данные напрямую из numpy-массивов"""
    def __init__(self):
        super().__init__()
        self._x = []
        self._v1 = []
        self._v2 = []
        self._diff = []
        self._count = 0
        self._headers = ["i", "x", "v1(x) / u(x)", "v2(x)", "|Разность|"]

    def update_data(self, x, v1, v2, diff):
        self.beginResetModel()
        self._x = x
        self._v1 = v1
        self._v2 = v2
        self._diff = diff
        self._count = len(x) if x is not None else 0
        self.endResetModel()

    def rowCount(self, parent=QModelIndex()):
        return self._count

    def columnCount(self, parent=QModelIndex()):
        return 5

    def data(self, index, role=Qt.ItemDataRole.DisplayRole):
        if not index.isValid() or role != Qt.ItemDataRole.DisplayRole:
            return None
        r, c = index.row(), index.column()
        
        if c == 0: return str(r)
        elif c == 1: return f"{self._x[r]:.6f}"
        elif c == 2: return f"{self._v1[r]:.6e}"
        elif c == 3: return f"{self._v2[r]:.6e}" if self._v2 is not None else "-"
        elif c == 4: return f"{self._diff[r]:.6e}"
        return None

    def headerData(self, section, orientation, role=Qt.ItemDataRole.DisplayRole):
        if orientation == Qt.Orientation.Horizontal and role == Qt.ItemDataRole.DisplayRole:
            return self._headers[section]
        return None

class SolverBackend(QObject):
    updateStats = Signal(str) 
    # НОВЫЙ СИГНАЛ: передает minY, maxY (для графиков) и maxDiff (для ошибки)
    updateAxes = Signal(float, float, float) 

    def __init__(self):
        super().__init__()
        self.task_manager = TaskManager()
        self._table_model = ResultTableModel()

    @Property(QObject, constant=True)
    def tableModel(self):
        return self._table_model

    @Slot(int, int, QObject, QObject, QObject)
    def calculate(self, n, task_id, series_v1, series_v2, series_diff):
        if not series_v1:
            return

        result = self.task_manager.solve(n, task_id)

        # 1. Добавляем точки на графики
        series_v1.clear()
        for x, y in zip(result["x"], result["v1"]):
            series_v1.append(float(x), float(y))

        series_v2.clear()
        for x, y in zip(result["x"], result["v2"]):
            series_v2.append(float(x), float(y))
        
        series_diff.clear()
        for x, y in zip(result["x"], result["diff"]):
            series_diff.append(float(x), float(y))

        # 2. Высчитываем границы осей
        max_y = float(np.max([np.max(result["v1"]), np.max(result["v2"])]))
        min_y = float(np.min([np.min(result["v1"]), np.min(result["v2"])]))
        margin = (max_y - min_y) * 0.05 if max_y != min_y else 0.1
        
        max_diff = float(np.max(result["diff"]))
        max_diff_scaled = max_diff * 1.1 if max_diff > 0 else 1e-10

        # ОТПРАВЛЯЕМ СИГНАЛ в QML с готовыми цифрами!
        self.updateAxes.emit(min_y - margin, max_y + margin, max_diff_scaled)

        # 3. Обновляем таблицу и текст
        self._table_model.update_data(result["x"], result["v1"], result["v2"], result["diff"])

        eps_name = "ε1 (Погрешность)" if result["is_test"] else "ε2 (Точность)"
        stats_text = (
            f"Для решения задачи использована равномерная сетка с числом разбиений n = {n}.\n\n"
            f"Задача должна быть решена с погрешностью не более ε = 0.5 * 10⁻⁶\n\n"
            f"Задача решена с {eps_name} = {result['max_err']:.4e}\n\n"
            f"Максимум разности наблюдается в точке x = {result['max_err_x']:.4f}"
        )
        self.updateStats.emit(stats_text)