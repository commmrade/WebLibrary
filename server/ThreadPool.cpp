#include <server/ThreadPool.hpp>

void ThreadPool::create() {
    int num_thrs = std::thread::hardware_concurrency();

    for (auto i{0}; i < num_thrs; i++) {
        threads.emplace_back(std::thread(&ThreadPool::thread_loop, this)); // Creating "empty" threads
    }
}
void ThreadPool::stop() {
    {
        std::unique_lock lock{mtx};
        should_terminate = true;
    }
    cond.notify_all();

    for (auto &thread : threads) {
        thread.join();
    }
    threads.clear();
    threads.shrink_to_fit();
}

void ThreadPool::thread_loop() {
    while (true) {                      
        std::unique_lock lock{mtx};
        cond.wait(lock, [this] { return !jobs.empty() || should_terminate; }); // Waiting until there is a task or it should stop
        if (should_terminate) {
            return;
        }

        Job job = jobs.front();
        jobs.pop();
        job();
    }
}

void ThreadPool::add_job(std::function<void(int)> job, int arg) {
    std::unique_lock lock{mtx};
    jobs.push([job, arg] { job(arg); });
    cond.notify_one();
}