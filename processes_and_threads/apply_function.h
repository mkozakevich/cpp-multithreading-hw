#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <algorithm>
#include <thread>

template <typename T>
void ApplyFunction(std::vector<T>& data, const std::function<void(T&)>& transform, const int threadCount = 1) {
    if (data.empty()) {
        return;
    }

    int number_of_threads = std::min<int>(threadCount, data.size());

    if (number_of_threads <= 1) {
        for (auto& element : data) {
            transform(element);
        }

        return;
    }

    std::vector<std::thread> threads;

    size_t chunk_size = data.size() / number_of_threads;
    size_t remainder = data.size() % number_of_threads;

    size_t chunk_start = 0;

    for (int i = 0; i < number_of_threads; ++i) {
        size_t total_size = chunk_size + (remainder && i == 0 ? 1 : 0);
        size_t chunk_end = chunk_start + total_size;

        threads.emplace_back(std::thread([chunk_start, chunk_end, &data, &transform]() {
            for (size_t j = chunk_start; j < chunk_end; ++j) {
                transform(data[j]);
            }
        }));

        chunk_start = chunk_end;
    }

    for (auto& thread : threads) {
        thread.join();
    }
}
