#include "tasks.h"
#include <cmath>

TaskContext get_task4_main_mixed_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;

    // Используем sqrt(x + 0.1) для k(x) слева, чтобы избежать деления на ноль 
    // при численном интегрировании 1/k(x) в точке x=0!
    ctx.q = [](double x) { return (x < 0.25) ? 1.0 : (x * x); };
    ctx.k = [](double x) { return (x < 0.25) ? std::sqrt(x + 0.1) : (x + 1.0); };
    ctx.f = [](double x) { return (x < 0.25) ? 1.0 : (2.0 + std::sqrt(x)); };

    // Краевые условия (Смешанные) - Вариант 2в
    // Левая граница: gamma1 = 3, theta1 = 15
    ctx.alpha1 = 3.0; 
    ctx.gamma1 = 45.0; // 3.0 * 15.0

    // Правая граница: gamma2 = 6, theta2 = 8
    ctx.alpha2 = 6.0;
    ctx.gamma2 = 48.0; // 6.0 * 8.0

    // Улучшенная аппроксимация ГУ O(h^2)
    ctx.apply_boundaries = apply_mixed_improved;
    return ctx;
}