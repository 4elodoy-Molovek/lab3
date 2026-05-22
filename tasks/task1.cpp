#include "tasks.h"
#include <cmath>

TaskContext get_task1_test_first_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;
    ctx.mu1 = 0.0; 
    ctx.mu2 = 1.0;
    
    // Коэффициент q(x)
    ctx.q = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 0.0625) / 2.0; // Строго в разрыве - среднее
        return (x < 0.25) ? 1.0 : 0.0625; 
    };
    
    // Коэффициент f(x)
    ctx.f = [](double x) { 
        if (std::abs(x - 0.25) < 1e-9) return (1.0 + 2.5) / 2.0; // Строго в разрыве - среднее
        return (x < 0.25) ? 1.0 : 2.5; 
    };

    // k(x) вычисляется в полуцелых узлах (между ячейками), поэтому в сам разрыв он никогда не попадет
    ctx.k = [](double x) { return (x < 0.25) ? 0.5 : 1.25; };

    ctx.apply_boundaries = apply_first_bvp;
    return ctx;
}