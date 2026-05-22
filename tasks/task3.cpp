#include "tasks.h"
#include <cmath>

TaskContext get_task3_test_mixed_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;

    ctx.q = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 0.0625) / 2.0; 
        return (x < 0.25) ? 1.0 : 0.0625; 
    };
    
    ctx.f = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 2.5) / 2.0; 
        return (x < 0.25) ? 1.0 : 2.5; 
    };

    ctx.k = [](double x) { return (x < 0.25) ? 0.5 : 1.25; };

    ctx.alpha1 = 3.0; 
    ctx.gamma1 = 45.0;
    ctx.alpha2 = 6.0;
    ctx.gamma2 = 48.0;

    // Классическая аппроксимация (1 порядок)
    ctx.apply_boundaries = apply_mixed_classic;
    return ctx;
}