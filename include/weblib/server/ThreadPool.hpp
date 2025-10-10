// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include "weblib/debug.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <limits>
#include <print>
#include <queue>
#include <thread>
#include <mutex>
#include <semaphore>

using default_func_type = std::move_only_function<void()>;
template <typename FunctionType = default_func_type>
    requires std::invocable<FunctionType>
class ThreadPool
{
  public:
    ThreadPool(unsigned int threads_count = std::thread::hardware_concurrency())
    {
        m_workers.reserve(threads_count);
        for (unsigned int i = 0; i < threads_count; ++i)
        {
            m_workers.emplace_back(
                [this]
                {
                    while (true)
                    {
                        std::unique_lock lock{m_mutex};
                        m_condvar.wait(lock,
                                       [this]
                                       {
                                           return !m_tasks.empty() ||
                                                  !m_should_work.load(std::memory_order_acquire);
                                       });
                        if (!m_should_work.load(std::memory_order_acquire))
                        {
                            return;
                        }

                        FunctionType task = std::move(m_tasks.front()); 
                        m_tasks.pop_front();
                        lock.unlock();

                        task();
                    }
                });
        }
    }
    ThreadPool(const ThreadPool &rhs)            = delete;
    ThreadPool &operator=(const ThreadPool &rhs) = delete;

    ~ThreadPool()
    {
        m_should_work.store(false, std::memory_order_release);
        m_condvar.notify_all();
    }

    template <typename Function, typename... Args,
              typename ReturnT = std::invoke_result_t<Function, Args...>>
        requires std::invocable<Function, Args...>
    [[nodiscard]]
    auto enqueue(Function &&function, Args &&...args) -> std::future<ReturnT>
    {
        std::promise<ReturnT> promise;
        auto                  future =
            promise.get_future();
        enqueue_task(
            [function = std::forward<Function>(function), promise = std::move(promise),
             ... args = std::forward<Args>(args)]() mutable
            {
                try
                {
                    if constexpr (std::is_same_v<void, ReturnT>)
                    {
                        std::invoke(function, args...);
                    }
                    else
                    {
                        promise.set_value(std::invoke(function, args...));
                    }
                }
                catch (...)
                {
                    promise.set_exception(std::current_exception());
                }
            });
        return future;
    }

    template <typename Function, typename... Args>
        requires std::invocable<Function, Args...>
    void enqueue_detach(Function &&function, Args &&...args)
    {
        enqueue_task(
            [function = std::forward<Function>(function),
             ... args = std::forward<Args>(args)]() mutable
            {
                try
                {
                    std::invoke(function,
                                args...); // Ignore return value if it is marked with [[nodiscard]]
                }
                catch (const std::exception &ex)
                {
                    debug::log_error(std::format("Exception in worker: {}", ex.what()));
                }
            });
    }

    void clear()
    {
        std::unique_lock lock{m_mutex};
        m_tasks.clear();
    }

    std::deque<FunctionType>::size_type queue_size() const
    {
        std::unique_lock lock{m_mutex};
        return m_tasks.size();
    }

  private:
    void enqueue_task(FunctionType &&task)
    {
        std::unique_lock lock{m_mutex};
        m_tasks.push_back(std::move(task));
        m_condvar.notify_one();
    }

    std::vector<std::jthread> m_workers;
    std::deque<FunctionType>  m_tasks; // Deque because we can easily pop_front, back and etc
    std::atomic<bool>         m_should_work{true};
    std::condition_variable   m_condvar;
    mutable std::mutex        m_mutex;
};
