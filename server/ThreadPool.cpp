#include <functional>
#include <ostream>
#include <server/ThreadPool.hpp>
#include <print>

ThreadPool::ThreadPool() {
    int num_thrs = std::thread::hardware_concurrency();

    for (auto i{0}; i < num_thrs; i++) {
        threads.emplace_back(std::thread(&ThreadPool::thread_loop, this)); // Creating "empty" threads
    }
}
ThreadPool::~ThreadPool() {
    should_terminate = true;
    semaphore.release(std::thread::hardware_concurrency());
    for (auto &thread : threads) {
        thread.join();
    }
}


void ThreadPool::thread_loop() {
    while (true) {                
        std::function<void()> job;
        semaphore.acquire(); 
        {
            std::unique_lock lock{mtx};
            if (should_terminate) {
                return;
            }

            if (!jobs.empty()) {
                job = jobs.front();
                jobs.pop();

            }
        }
        if (job) job();
    }
}

void ThreadPool::add_job(std::function<void()> job) {
    {
        std::unique_lock lock{mtx};
        jobs.push(job);
    }
    semaphore.release();
}