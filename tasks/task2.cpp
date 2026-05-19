#include "tasks.h"
#include <cmath>

TaskContext get_task2_main_first_bvp() {
    TaskContext ctx;
    ctx.xi = 0.25;
    ctx.mu1 = 0.0; ctx.mu2 = 1.0;
    
    ctx.k = [](double x) { return (x < 0.25) ? std::sqrt(x + 0.1) : (x + 1.0); };
    ctx.q = [](double x) { return (x < 0.25) ? 1.0 : (x * x); };
    ctx.f = [](double x) { return (x < 0.25) ? 1.0 : std::exp(-x); };
    
    ctx.apply_boundaries = apply_first_bvp;
    return ctx;
}