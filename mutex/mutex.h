#pragma once

#include <atomic>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <climits>

class Mutex {
private:
    std::atomic<int> state{0};

    static int compare_exchange(std::atomic<int>* atomic, int expected, int desired) {
        int old_value = expected;

        atomic->compare_exchange_strong(old_value, desired, std::memory_order_acquire);

        return old_value;
    }

    static int futex_wait(std::atomic<int>* addr, int expected) {
        return syscall(SYS_futex, reinterpret_cast<int*>(addr), FUTEX_WAIT, expected, nullptr, nullptr, 0);
    }

    static int futex_wake(std::atomic<int>* addr, int count) {
        return syscall(SYS_futex, reinterpret_cast<int*>(addr), FUTEX_WAKE, count, nullptr, nullptr, 0);
    }

public:
    void lock() {
        int current_state = compare_exchange(&state, 0, 1);

        if (current_state != 0) {
            do {
                if (current_state == 2 || compare_exchange(&state, 1, 2) != 0) {
                    futex_wait(&state, 2);
                }
            } while ((current_state = compare_exchange(&state, 0, 2)) != 0);
        }
    }

    bool try_lock() {
        int expected = 0;

        return state.compare_exchange_strong(expected, 1, std::memory_order_acquire);
    }

    void unlock() {
        if (state.exchange(0, std::memory_order_release) == 2) {
            futex_wake(&state, 1);
        }
    }
};
