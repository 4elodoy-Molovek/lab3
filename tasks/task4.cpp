#include "tasks.h"
#include <cmath>

TaskContext get_task4_main_mixed_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;

    // Усреднение для q(x)
    ctx.q = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 0.0625) / 2.0; 
        return (x < 0.25) ? 1.0 : (x * x); 
    };
    
    // Усреднение для f(x)
    ctx.f = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 2.5) / 2.0; 
        return (x < 0.25) ? 1.0 : (2.0 + std::sqrt(x)); 
    };

    ctx.k = [](double x) { 
        return (x < 0.25) ? std::sqrt(x + 0.1) : (x + 1.0); 
    };

    ctx.alpha1 = 3.0; 
    ctx.gamma1 = 45.0; 
    ctx.alpha2 = 6.0;
    ctx.gamma2 = 48.0; 

    ctx.apply_boundaries = apply_mixed_improved;
    return ctx;
}