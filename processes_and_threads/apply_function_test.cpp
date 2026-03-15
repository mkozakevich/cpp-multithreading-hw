#include <gtest/gtest.h>
#include "apply_function.h"
#include <vector>
#include <thread>
#include <set>
#include <mutex>

TEST(ApplyFunctionTest, EvenNumberOfElements) {
    std::vector<int> v = {1,2,3,4,5,6};

    ApplyFunction<int>(v, [](int& x){
        x *= 2;
    });

    std::vector<int> expected = {2,4,6,8,10,12};

    EXPECT_EQ(v, expected);
}

TEST(ApplyFunctionTest, OddNumberOfElements) {
    std::vector<int> v = {1,2,3,4,5};

    ApplyFunction<int>(v, [](int& x){
        x *= 2;
    });

    std::vector<int> expected = {2,4,6,8,10};

    EXPECT_EQ(v, expected);
}

TEST(ApplyFunctionTest, ThreadsMoreThanElements) {
    std::vector<int> v = {1,2,3};

    ApplyFunction<int>(v, [](int& x){
        x *= 2;
    }, 10);

    std::vector<int> expected = {2,4,6};

    EXPECT_EQ(v, expected);
}

TEST(ApplyFunctionTest, ThreadsLessThanElements) {
    std::vector<int> v = {1,2,3};

    ApplyFunction<int>(v, [](int& x){
        x *= 2;
    }, 2);

    std::vector<int> expected = {2,4,6};

    EXPECT_EQ(v, expected);
}

TEST(ApplyFunctionTest, EmptyVector) {
    std::vector<int> v;

    ApplyFunction<int>(v, [](int& x){
        x *= 2;
    });

    EXPECT_TRUE(v.empty());
}

TEST(ApplyFunctionTest, UsesDefinedNumberOfThreads) {
    std::vector<int> v(100);

    std::set<std::thread::id> thread_ids;
    std::mutex m;

    auto transform = [&](int&) {
        std::lock_guard<std::mutex> lock(m);
        thread_ids.insert(std::this_thread::get_id());
    };

    ApplyFunction<int>(v, transform, 2);

    EXPECT_EQ(thread_ids.size(), 2);
}

TEST(ApplyFunctionTest, UsesOneThreadByDefault) {
    std::vector<int> v(100);

    std::set<std::thread::id> thread_ids;
    std::mutex m;

    auto transform = [&](int&) {
        std::lock_guard<std::mutex> lock(m);
        thread_ids.insert(std::this_thread::get_id());
    };

    ApplyFunction<int>(v, transform);

    EXPECT_EQ(thread_ids.size(), 1);
}

TEST(ApplyFunctionTest, UsesDataSizeNumberOfThreads) {
    std::vector<int> v(2);

    std::set<std::thread::id> thread_ids;
    std::mutex m;

    auto transform = [&](int&) {
        std::lock_guard<std::mutex> lock(m);
        thread_ids.insert(std::this_thread::get_id());
    };

    ApplyFunction<int>(v, transform, 3);

    EXPECT_EQ(thread_ids.size(), 2);
}
