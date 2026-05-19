#include "tasks.h"

TaskContext get_task1_test_first_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;
    ctx.mu1 = 0.0; ctx.mu2 = 1.0;
    
    ctx.q = [](double x) { return (x < 0.25) ? 1.0 : 0.0625; };
    ctx.k = [](double x) { return (x < 0.25) ? std::sqrt(x) : (x + 1.0); };
    ctx.f = [](double x) { return (x < 0.25) ? 1.0 : (2.0 + std::sqrt(x)); };

    ctx.apply_boundaries = apply_first_bvp;
    return ctx;
}