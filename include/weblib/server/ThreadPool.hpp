#pragma once

#include <atomic>
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
    void add_job(std::function<void()>&& job); // Make it accept not only void<int> store a lambda with all params captured then task is executed like task()
private:
    using Job = std::function<void()>;

    std::mutex m_mtx;
    std::counting_semaphore<std::numeric_limits<int>::max()> m_semaphore{0};
    std::vector<std::thread> m_threads;
    std::queue<Job> m_jobs;
    std::atomic<bool> m_should_terminate{false};
};