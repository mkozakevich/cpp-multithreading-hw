#include <benchmark/benchmark.h>
#include "apply_function.h"
#include <vector>
#include <cmath>

void CheapTransform(int& x) {
    x += 1;
}

void ExpensiveTransform(int& x) {
    double v = x;

    for (int i = 0; i < 100; ++i) {
        v = std::sin(v) + std::cos(v);
    }

    x = static_cast<int>(v);
}

void SingleThreadWithCheapTransform(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(1000, 1);
        ApplyFunction<int>(v, CheapTransform, 1);
    }
}

void MultiThreadWithCheapTransform(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(1000, 1);
        ApplyFunction<int>(v, CheapTransform, 8);
    }
}

void SingleThreadWithExpensiveTransform(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(1000, 1);
        ApplyFunction<int>(v, ExpensiveTransform, 1);
    }
}

void MultiThreadWithExpensiveTransform(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(1000, 1);
        ApplyFunction<int>(v, ExpensiveTransform, 8);
    }
}

void SingleThreadWithLowNumberOfElements(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(10, 1);
        ApplyFunction<int>(v, CheapTransform, 1);
    }
}

void MultiThreadWithLowNumberOfElements(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(10, 1);
        ApplyFunction<int>(v, CheapTransform, 8);
    }
}

void SingleThreadWithHighNumberOfElements(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(10000000, 1);
        ApplyFunction<int>(v, CheapTransform, 1);
    }
}

void MultiThreadWithHighNumberOfElements(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(1000000, 1);
        ApplyFunction<int>(v, CheapTransform, 8);
    }
}

// Подбор transform

// Если в transform происходят легкие вычисления, то создать потоки дороже чем просто выполнить в один поток
BENCHMARK(SingleThreadWithCheapTransform);
BENCHMARK(MultiThreadWithCheapTransform);

// Если в transform происходят сложные вычисления, то выигрывает многопоточка
BENCHMARK(SingleThreadWithExpensiveTransform);
BENCHMARK(MultiThreadWithExpensiveTransform);

// Подбор размера вектора data

// Если в data мало элементов, то создать потоки дороже чем просто выполнить в один поток
BENCHMARK(SingleThreadWithLowNumberOfElements);
BENCHMARK(MultiThreadWithLowNumberOfElements);

// Если в data очень много элементов, то выигрывает многопоточка
BENCHMARK(SingleThreadWithHighNumberOfElements);
BENCHMARK(MultiThreadWithHighNumberOfElements);

BENCHMARK_MAIN();
