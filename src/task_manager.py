import ctypes
import os
import platform
import numpy as np

class TaskManager:
    def __init__(self):
        self.lib = None
        self._load_library()

    def _load_library(self):
        """Загрузка C++ ядра в зависимости от ОС"""
        lib_name = "solver.dll" if platform.system() == "Windows" else "libsolver.so"
        base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        build_dir = os.path.join(base_dir, "build")
        
        lib_path = None
        for root, dirs, files in os.walk(build_dir):
            if lib_name in files:
                lib_path = os.path.join(root, lib_name)
                break
                
        if not lib_path:
            raise FileNotFoundError(f"Не найден {lib_name}. Выполните сборку CMake!")

        self.lib = ctypes.CDLL(lib_path)
        self.lib.solve_bvp.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_double)]
        self.lib.solve_bvp.restype = None

    def _call_cpp(self, n, task_id):
        """Вспомогательный метод для вызова C++ кода"""
        u_out_type = ctypes.c_double * (n + 1)
        u_out = u_out_type()
        self.lib.solve_bvp(n, task_id, u_out)
        return np.array(u_out)

    def get_analytical_solution(self, x_array, task_id):
        """Аналитическое решение для тестовых задач варианта №2"""
        u_exact = np.zeros_like(x_array)
        
        xi = 0.25
        # Константы из таблицы (Вариант 2)
        k1, q1, f1 = 0.5, 1.0, 1.0
        k2, q2, f2 = 1.25, 0.0625, 2.5 # f2 = 2 + sqrt(0.25) = 2.5

        lam1 = np.sqrt(q1 / k1)
        lam2 = np.sqrt(q2 / k2)
        up1 = f1 / q1 
        up2 = f2 / q2 

        # Условия сопряжения (инвариантны для всех типов задач)
        E11, E12 = np.exp(lam1 * xi), np.exp(-lam1 * xi)
        E21, E22 = np.exp(lam2 * xi), np.exp(-lam2 * xi)
        row3 = [E11, E12, -E21, -E22]
        rhs3 = up2 - up1
        row4 = [k1*lam1*E11, -k1*lam1*E12, -k2*lam2*E21, k2*lam2*E22]
        rhs4 = 0.0

        if task_id == 1:
            # Первая краевая (Дирихле): u(0)=0, u(1)=1
            mu1, mu2 = 0.0, 1.0 
            row1 = [1.0, 1.0, 0.0, 0.0]
            rhs1 = mu1 - up1
            E21_end, E22_end = np.exp(lam2 * 1.0), np.exp(-lam2 * 1.0)
            row2 = [0.0, 0.0, E21_end, E22_end]
            rhs2 = mu2 - up2

        elif task_id == 3:
            # Смешанная краевая (Вариант 2в из тетради)
            alpha1, gamma1 = 3.0, 45.0
            alpha2, gamma2 = 6.0, 48.0
            row1 = [-k1*lam1 + alpha1, k1*lam1 + alpha1, 0.0, 0.0]
            rhs1 = gamma1 - alpha1 * up1
            E21_end, E22_end = np.exp(lam2 * 1.0), np.exp(-lam2 * 1.0)
            row2 = [0.0, 0.0, E21_end*(k2*lam2 + alpha2), E22_end*(-k2*lam2 + alpha2)]
            rhs2 = gamma2 - alpha2 * up2

        # Решаем систему для поиска C1, C2, C3, C4
        A = np.array([row1, row2, row3, row4])
        B = np.array([rhs1, rhs2, rhs3, rhs4])
        C = np.linalg.solve(A, B)

        for i, x in enumerate(x_array):
            if x <= xi:
                u_exact[i] = C[0] * np.exp(lam1 * x) + C[1] * np.exp(-lam1 * x) + up1
            else:
                u_exact[i] = C[2] * np.exp(lam2 * x) + C[3] * np.exp(-lam2 * x) + up2
                
        return u_exact

    def run_test_task(self, n, task_id):
        """Запуск тестовой задачи: сравнение с аналитикой (поиск epsilon_1)"""
        x = np.linspace(0.0, 1.0, n + 1)
        v = self._call_cpp(n, task_id)
        u = self.get_analytical_solution(x, task_id)
        
        diff = np.abs(u - v)
        max_err = np.max(diff)
        max_err_x = x[np.argmax(diff)]
        
        return {
            "is_test": True,
            "x": x,
            "v1": v,         # Численное решение
            "v2": u,         # Точное аналитическое решение
            "diff": diff,    # Модуль разности
            "max_err": max_err,
            "max_err_x": max_err_x
        }

    def run_main_task(self, n, task_id):
        """Запуск основной задачи: расчет на сетках n и 2n (поиск epsilon_2)"""
        # 1. Расчет на обычной сетке (n)
        x = np.linspace(0.0, 1.0, n + 1)
        v_n = self._call_cpp(n, task_id)
        
        # 2. Расчет на удвоенной сетке (2n)
        v_2n_full = self._call_cpp(2 * n, task_id)
        
        # Берем значения из v_2n_full только в общих узлах
        v_2n = v_2n_full[::2]
        
        # Вычисляем разницу в общих узлах
        diff = np.abs(v_n - v_2n)
        max_err = np.max(diff)
        max_err_x = x[np.argmax(diff)]
        
        return {
            "is_test": False,
            "x": x,
            "v1": v_n,       # Решение на сетке n
            "v2": v_2n,      # Решение на сетке 2n
            "diff": diff,    # Модуль разности
            "max_err": max_err,
            "max_err_x": max_err_x
        }

    def solve(self, n, task_id):
        """Главный метод, определяющий тип задачи"""
        if task_id in [1, 3]:
            return self.run_test_task(n, task_id)
        else:
            return self.run_main_task(n, task_id)