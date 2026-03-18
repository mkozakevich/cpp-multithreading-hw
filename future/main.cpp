#include "thread_pool.h"

#include <iostream>
#include <string>

int main() {
    ThreadPool pool(4);

    auto f1 = pool.Submit([] {
        return 10;
    });
    auto f2 = pool.Submit([](int x) {
        return std::to_string(x);
    }, 20);

    auto f3 = pool.Submit([] {});

    auto f4 = pool.Submit([]() -> int {
        throw std::runtime_error("some exception");
    });

    std::cout << "f1 " << f1.get() << "\n";
    std::cout << "f2 " << f2.get() << "\n";
    f3.get();

    try {
        (void)f4.get();
    } catch (const std::exception& e) {
        std::cout << "f4 " << e.what() << "\n";
    }

    return 0;
}

