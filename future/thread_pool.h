#pragma once

#include "future.h"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <stdexcept>
#include <type_traits>
#include <tuple>
#include <utility>
#include <vector>

class ThreadPool {
public:
    ThreadPool(std::size_t threads = 0) : stop(false) {
        if (threads == 0) {
            threads = std::thread::hardware_concurrency();
            if (threads == 0) {
                threads = 1;   
            }
        }

        workers.reserve(threads);

        for (std::size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] { this->worker_loop(); });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool() {
        stop.store(true);
        tasks_cv.notify_all();

        for (auto& t : workers) {
            if (t.joinable()) {
                t.join();   
            }
        }
    }

    template <class F, class... Args>
    auto Submit(F&& f, Args&&... args)
        -> Future<std::decay_t<std::invoke_result_t<F, Args...>>> {
        using R0 = std::invoke_result_t<F, Args...>;
        using R = std::decay_t<R0>;

        auto state = std::make_shared<SharedState<R>>();
        auto fut = Future<R>(state);

        {
            std::lock_guard<std::mutex> lock(tasks_m);

            auto task_func = std::forward<F>(f);
            auto task_args = std::make_tuple(std::forward<Args>(args)...);

            tasks.emplace([state, task_func = std::move(task_func), task_args = std::move(task_args)]() mutable {
                try {
                    if constexpr (std::is_void_v<R>) {
                        std::apply(task_func, task_args);
                        state->set_value();
                    } else {
                        R res = std::apply(task_func, task_args);
                        state->set_value(std::move(res));
                    }
                } catch (...) {
                    state->set_exception(std::current_exception());
                }
            });
        }

        tasks_cv.notify_one();

        return fut;
    }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(tasks_m);
                tasks_cv.wait(lock, [&] { return stop.load() || !tasks.empty(); });

                if (stop.load() && tasks.empty()) {
                    return;
                }

                job = std::move(tasks.front());
                tasks.pop();
            }
            job();
        }
    }

    std::vector<std::thread> workers;

    std::mutex tasks_m;
    std::condition_variable tasks_cv;
    std::queue<std::function<void()>> tasks;

    std::atomic<bool> stop;
};

