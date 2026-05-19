#pragma once
#include <functional>
#include <vector>

struct TaskContext {
    std::function<double(double)> k;
    std::function<double(double)> q;
    std::function<double(double)> f;

    double a = 0.0;
    double b = 1.0;
    double xi = 0.25; // Точка разрыва

    // Параметры для первой краевой задачи (u(a) = mu1, u(b) = mu2)
    double mu1 = 0.0;
    double mu2 = 0.0;

    // Параметры для смешанной краевой: -k*u' + alpha*u = gamma
    double alpha1 = 0.0, gamma1 = 0.0; // Левая граница
    double alpha2 = 0.0, gamma2 = 0.0; // Правая граница

    // Функция-колбэк для применения краевых условий
    std::function<void(int n, double h, const TaskContext& ctx, 
                       std::vector<double>& A, std::vector<double>& C, 
                       std::vector<double>& B, std::vector<double>& F)> apply_boundaries;
};

// Кроссплатформенный макрос для экспорта функции
#ifdef _WIN32
    #define SOLVER_EXPORT __declspec(dllexport)
#else
    #define SOLVER_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Единая точка входа для Python
SOLVER_EXPORT void solve_bvp(int n, int task_id, double* u_out);

#ifdef __cplusplus
}
#endif