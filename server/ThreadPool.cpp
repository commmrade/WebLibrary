#include <functional>
#include <server/ThreadPool.hpp>
#include <print>

ThreadPool::ThreadPool() {
    int num_thrs = std::thread::hardware_concurrency();

    for (auto i{0}; i < num_thrs; i++) {
        m_threads.emplace_back(std::thread(&ThreadPool::thread_loop, this)); // Creating "empty" m_threads
    }
}
ThreadPool::~ThreadPool() {
    m_should_terminate = true;
    m_semaphore.release(std::thread::hardware_concurrency());
    for (auto &thread : m_threads) {
        thread.join();
    }
}


void ThreadPool::thread_loop() {
    while (true) {                
        std::function<void()> job;
        m_semaphore.acquire(); 
        {
            std::unique_lock lock{m_mtx};
            if (m_should_terminate) {
                return;
            }

            if (!m_jobs.empty()) {
                job = m_jobs.front();
                m_jobs.pop();

            }
        }
        if (job) job();
    }
}

void ThreadPool::add_job(std::function<void()> job) {
    {
        std::unique_lock lock{m_mtx};
        m_jobs.push(job);
    }
    m_semaphore.release();
}