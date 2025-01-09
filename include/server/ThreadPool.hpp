#pragma once

#include <functional>
#include <queue>
#include<thread>
#include<mutex>
#include<condition_variable>



class ThreadPool {
public:

    ThreadPool();
    ~ThreadPool();


    void thread_loop();
    void add_job(std::function<void(int)> job, int arg);
private:
    using Job = std::function<void()>;

    std::mutex mtx;
    std::condition_variable cond;
    std::vector<std::thread> threads;
    std::queue<Job> jobs;
    bool should_terminate{false};
};