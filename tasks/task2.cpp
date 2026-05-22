#include "tasks.h"
#include <cmath>

TaskContext get_task2_main_first_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;

    // Коэффициент q(x) с честным усреднением в точке разрыва
    ctx.q = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 0.0625) / 2.0; 
        return (x < 0.25) ? 1.0 : (x * x); 
    };
    
    // Коэффициент f(x) с честным усреднением в точке разрыва
    ctx.f = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 2.5) / 2.0; 
        return (x < 0.25) ? 1.0 : (2.0 + std::sqrt(x)); 
    };

    // Коэффициент k(x) (используем sqrt(x + 0.1) слева, чтобы избежать деления на ноль)
    ctx.k = [](double x) { 
        return (x < 0.25) ? std::sqrt(x + 0.1) : (x + 1.0); 
    };

    ctx.mu1 = 0.0; 
    ctx.mu2 = 1.0;
    ctx.apply_boundaries = apply_first_bvp;
    return ctx;
}