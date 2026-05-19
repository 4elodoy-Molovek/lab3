#include "tasks.h"

TaskContext get_task3_test_mixed_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;

    // Тестовая задача: коэффициенты - это константы, 
    // вычисленные как пределы функций из 2-го варианта в точке разрыва xi=0.25
    ctx.k = [](double x) { return (x < 0.25) ? 0.591608 : 1.25; };     
    ctx.q = [](double x) { return (x < 0.25) ? 1.0 : 0.0625; };         
    ctx.f = [](double x) { return (x < 0.25) ? 1.0 : 0.778801; };       

    // Краевые условия (Смешанные) - Вариант 2в
    // Левая граница: gamma1 = 3, theta1 = 15
    ctx.alpha1 = 3.0; 
    ctx.gamma1 = 45.0; // 3.0 * 15.0

    // Правая граница: gamma2 = 6, theta2 = 8
    ctx.alpha2 = 6.0;
    ctx.gamma2 = 48.0; // 6.0 * 8.0

    // Используем классическую аппроксимацию ГУ с 1-м порядком точности
    ctx.apply_boundaries = apply_mixed_classic;
    return ctx;
}