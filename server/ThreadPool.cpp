#include <functional>
#include <server/ThreadPool.hpp>

ThreadPool::ThreadPool() {
    int num_thrs = std::thread::hardware_concurrency();

    for (auto i{0}; i < num_thrs; i++) {
        threads.emplace_back(std::thread(&ThreadPool::thread_loop, this)); // Creating "empty" threads
    }
}
ThreadPool::~ThreadPool() {
    {
        std::unique_lock lock{mtx};
        should_terminate = true;
    }
    cond.notify_all();
    for (auto &thread : threads) {
        thread.join();
    }
}


void ThreadPool::thread_loop() {
    while (true) {                      
        std::function<void()> job;
        {
            std::unique_lock lock{mtx};
            cond.wait(lock, [this] { return !jobs.empty() || should_terminate; }); // Waiting until there is a task or it should stop
            if (should_terminate) {
                return;
            }

            job = jobs.front();
            jobs.pop();
        }
        job();
    }
}

void ThreadPool::add_job(std::function<void()> job) {
    {
        std::unique_lock lock{mtx};
        jobs.push(job);
    }
    cond.notify_one();
}