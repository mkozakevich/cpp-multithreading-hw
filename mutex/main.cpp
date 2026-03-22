#include "mutex.h"
#include <thread>
#include <vector>
#include <iostream>

Mutex mutex;
int counter = 0;

void worker() {
    for (int i = 0; i < 100000; i++) {
        mutex.lock();
        counter++;
        mutex.unlock();
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; i++) {
        threads.emplace_back(worker);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << counter << std::endl;
}
