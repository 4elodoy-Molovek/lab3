#include "solver.h"
#include "../tasks/tasks.h"
#include <cmath>
#include <iostream>

// Интегрирование по формуле Симпсона (с защитой границ)
double integrate_smooth(const std::function<double(double)>& func, double a, double b, int n = 20) {
    if (std::abs(b - a) < 1e-14) return 0.0;
    
    // Эпсилон-сдвиг: отступаем от краев на одну десятимиллиардную, 
    // чтобы не наступить в точку разрыва или сингулярность x=0
    const double eps = 1e-10;
    double a_safe = a + eps;
    double b_safe = b - eps;

    double h = (b_safe - a_safe) / n;
    
    // Берем функцию на безопасных краях
    double sum = func(a_safe) + func(b_safe);
    
    // Внутренние узлы Симпсона
    for (int i = 1; i < n; i++) {
        sum += func(a_safe + i * h) * ((i % 2 == 0) ? 2.0 : 4.0);
    }
    
    return sum * h / 3.0;
}

// Интегратор с детектором разрыва (Разбивает интеграл, если xi внутри)
double integrate(const std::function<double(double)>& func, double a, double b, double xi) {
    if (a > b) return -integrate(func, b, a, xi);
    double eps = 1e-12;
    if (a + eps < xi && xi < b - eps) {
        return integrate_smooth(func, a, xi) + integrate_smooth(func, xi, b);
    }
    return integrate_smooth(func, a, b);
}

// ================= Реализации краевых условий =================

void apply_first_bvp(int n, double h, const TaskContext& ctx, std::vector<double>& A, std::vector<double>& C, std::vector<double>& B, std::vector<double>& F) {
    C[0] = 1.0; B[0] = 0.0; F[0] = ctx.mu1;
    C[n] = 1.0; A[n] = 0.0; F[n] = ctx.mu2;
}

void apply_mixed_classic(int n, double h, const TaskContext& ctx, std::vector<double>& A, std::vector<double>& C, std::vector<double>& B, std::vector<double>& F) {
    // Левая граница: -k u' + alpha u = gamma. Аппроксимация O(h): u' = (u1 - u0)/h
    double k0 = ctx.k(ctx.a);
    C[0] = k0 / h + ctx.alpha1;
    B[0] = k0 / h;
    F[0] = ctx.gamma1;

    // Правая граница: k u' + alpha u = gamma
    double kn = ctx.k(ctx.b);
    C[n] = kn / h + ctx.alpha2;
    A[n] = kn / h;
    F[n] = ctx.gamma2;
}

void apply_mixed_improved(int n, double h, const TaskContext& ctx, std::vector<double>& A, std::vector<double>& C, std::vector<double>& B, std::vector<double>& F) {
    // Улучшенная O(h^2) через уравнение на полуцелом узле
    double k_half = ctx.k(ctx.a + h/2.0);
    double q0 = ctx.q(ctx.a);
    double f0 = ctx.f(ctx.a);
    C[0] = k_half / h + ctx.alpha1 + q0 * h / 2.0;
    B[0] = k_half / h;
    F[0] = ctx.gamma1 + f0 * h / 2.0;

    double k_n_half = ctx.k(ctx.b - h/2.0);
    double qn = ctx.q(ctx.b);
    double fn = ctx.f(ctx.b);
    C[n] = k_n_half / h + ctx.alpha2 + qn * h / 2.0;
    A[n] = k_n_half / h;
    F[n] = ctx.gamma2 + fn * h / 2.0;
}

// ================= Основной солвер =================

extern "C" SOLVER_EXPORT void solve_bvp(int n, int task_id, double* u_out) {
    TaskContext ctx;
    if (task_id == 1) ctx = get_task1_test_first_bvp();
    else if (task_id == 2) ctx = get_task2_main_first_bvp();
    else if (task_id == 3) ctx = get_task3_test_mixed_bvp();
    else if (task_id == 4) ctx = get_task4_main_mixed_bvp();

    double h = (ctx.b - ctx.a) / n;
    std::vector<double> A(n + 1, 0.0), C(n + 1, 0.0), B(n + 1, 0.0), F(n + 1, 0.0);

    // Метод баланса для внутренних узлов
    for (int i = 1; i < n; ++i) {
        double x_i = ctx.a + i * h;
        auto inv_k = [&ctx](double x) { return 1.0 / ctx.k(x); };
        
        double a_i = h / integrate(inv_k, x_i - h, x_i, ctx.xi);
        double b_i = h / integrate(inv_k, x_i, x_i + h, ctx.xi);
        double d_i = integrate(ctx.q, x_i - h/2.0, x_i + h/2.0, ctx.xi) / h;
        double phi_i = integrate(ctx.f, x_i - h/2.0, x_i + h/2.0, ctx.xi) / h;

        A[i] = a_i / (h * h);
        B[i] = b_i / (h * h);
        C[i] = A[i] + B[i] + d_i;
        F[i] = phi_i;
    }

    ctx.apply_boundaries(n, h, ctx, A, C, B, F);

    // Метод прогонки
    std::vector<double> alpha(n + 1, 0.0), beta(n + 1, 0.0);
    alpha[1] = B[0] / C[0];
    beta[1] = F[0] / C[0];
    for (int i = 1; i < n; ++i) {
        double denom = C[i] - A[i] * alpha[i];
        alpha[i + 1] = B[i] / denom;
        beta[i + 1] = (F[i] + A[i] * beta[i]) / denom;
    }

    u_out[n] = (F[n] + A[n] * beta[n]) / (C[n] - A[n] * alpha[n]);
    for (int i = n - 1; i >= 0; --i) {
        u_out[i] = alpha[i + 1] * u_out[i + 1] + beta[i + 1];
    }
}