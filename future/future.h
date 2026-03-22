#pragma once

#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <class T>
struct SharedState {
    std::mutex m;
    std::condition_variable cv;
    bool ready = false;
    std::optional<T> value;
    std::exception_ptr exception;

    void set_value(T v) {
        {
            std::lock_guard<std::mutex> lock(m);
            value = std::move(v);
            ready = true;
        }

        cv.notify_all();
    }

    void set_exception(std::exception_ptr e) {
        {
            std::lock_guard<std::mutex> lock(m);
            exception = e;
            ready = true;
        }

        cv.notify_all();
    }
};

template <>
struct SharedState<void> {
    std::mutex m;
    std::condition_variable cv;
    bool ready = false;
    std::exception_ptr exception;

    void set_value() {
        {
            std::lock_guard<std::mutex> lock(m);
            ready = true;
        }

        cv.notify_all();
    }

    void set_exception(std::exception_ptr e) {
        {
            std::lock_guard<std::mutex> lock(m);
            exception = e;
            ready = true;
        }

        cv.notify_all();
    }
};

template <class T>
class Future {
public:
    Future() = default;
    explicit Future(std::shared_ptr<SharedState<T>> s) : state(std::move(s)) {}

    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    Future(Future&&) noexcept = default;
    Future& operator=(Future&&) noexcept = default;

    bool valid() const { return static_cast<bool>(state); }

    void wait() {
        std::unique_lock<std::mutex> lock(state->m);
        state->cv.wait(lock, [&] { return state->ready; });
    }

    T get() {
        wait();

        if (state->exception) {
            auto e = state->exception;
            state.reset();
            std::rethrow_exception(e);
        }

        T out = std::move(*state->value);
        state.reset();

        return out;
    }

private:
    std::shared_ptr<SharedState<T>> state;
};

template <>
class Future<void> {
public:
    Future() = default;
    explicit Future(std::shared_ptr<SharedState<void>> s) : state(std::move(s)) {}

    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    Future(Future&&) noexcept = default;
    Future& operator=(Future&&) noexcept = default;

    bool valid() const { return static_cast<bool>(state); }

    void wait() {
        std::unique_lock<std::mutex> lock(state->m);
        state->cv.wait(lock, [&] { return state->ready; });
    }

    void get() {
        wait();

        if (state->exception) {
            auto e = state->exception;
            state.reset();
            std::rethrow_exception(e);
        }

        state.reset();
    }

private:
    std::shared_ptr<SharedState<void>> state;
};

