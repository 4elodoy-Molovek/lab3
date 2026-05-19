#pragma once
#include "../core/solver.h"

// Функции-генераторы контекстов для ваших 4-х человек
TaskContext get_task1_test_first_bvp();
TaskContext get_task2_main_first_bvp();
TaskContext get_task3_test_mixed_bvp();
TaskContext get_task4_main_mixed_bvp();

// Универсальные аппроксиматоры краевых условий (реализованы в solver.cpp)
void apply_first_bvp(int n, double h, const TaskContext& ctx, std::vector<double>& A, std::vector<double>& C, std::vector<double>& B, std::vector<double>& F);
void apply_mixed_classic(int n, double h, const TaskContext& ctx, std::vector<double>& A, std::vector<double>& C, std::vector<double>& B, std::vector<double>& F);
void apply_mixed_improved(int n, double h, const TaskContext& ctx, std::vector<double>& A, std::vector<double>& C, std::vector<double>& B, std::vector<double>& F);