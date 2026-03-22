#pragma once

#include <optional>
#include <mutex>
#include <condition_variable>
#include <utility>

template <class T>
class UnbufferedChannel {
public:
    void Send(const T& value) {
        std::unique_lock<std::mutex> lock(mutex);

        if (closed) {
            throw std::runtime_error("Сhannel closed");
        }

        sender.wait(lock, [this] {
            return !stored_value.has_value() || closed;
        });

        if (closed) {
            throw std::runtime_error("Сhannel closed");
        }

        stored_value = value;

        value_available.notify_one();

        sender.wait(lock, [this] {
            return !stored_value.has_value();
        });
    }

    std::optional<T> Recv() {
        std::unique_lock<std::mutex> lock(mutex);

        value_available.wait(lock, [this] {
            return stored_value.has_value() || closed;
        });

        if (stored_value.has_value()) {
            std::optional<T> result = std::exchange(stored_value, std::nullopt);

            sender.notify_all();

            return result;
        }

        return std::nullopt;
    }

    void Close() {
        std::lock_guard<std::mutex> lock(mutex);

        if (closed) {
            return;
        }

        closed = true;
        sender.notify_all();
        value_available.notify_all();
    }

private:
    std::mutex mutex;
    std::condition_variable value_available;
    std::condition_variable sender;
    bool closed{false};
    std::optional<T> stored_value;
};
