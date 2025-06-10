#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <thread>
#include <mutex>
#include <semaphore>


class ThreadPool {
public:

    ThreadPool();
    ~ThreadPool();


    void thread_loop();
    void add_job(std::function<void()> job); // Make it accept not only void<int> store a lambda with all params captured then task is executed like task()
private:
    using Job = std::function<void()>;

    std::mutex mtx;
    std::counting_semaphore<std::numeric_limits<int>::max()> semaphore{0};
    std::vector<std::thread> threads;
    std::queue<Job> jobs;
    std::atomic<bool> should_terminate{false};
};