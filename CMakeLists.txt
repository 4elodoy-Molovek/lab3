cmake_minimum_required(VERSION 3.10)
project(SolverBVP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Собираем ядро и все 4 задачи в одну динамическую библиотеку
add_library(solver SHARED 
    core/solver.cpp 
    tasks/task1.cpp 
    tasks/task2.cpp 
    tasks/task3.cpp 
    tasks/task4.cpp
)