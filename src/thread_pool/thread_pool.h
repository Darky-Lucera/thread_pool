#pragma once

//#include <tuple>
#include <atomic>
#include <vector>
#include <thread>
#include <memory>
#include <future>
//#include <utility>
//#include <stdexcept>
#include <functional>
//#include <type_traits>
#include "unbounded_queue.h"


class thread_pool
{
public:
    explicit thread_pool(std::size_t thread_count = std::thread::hardware_concurrency())
    : m_queues(thread_count), m_count(thread_count)
    {
        if(!thread_count)
            throw std::invalid_argument("bad thread count! must be non-zero!");

        auto worker = [this](size_t i)
        {
            while(true)
            {
                proc_t f;
                for(size_t n = 0; n < m_count * K; ++n)
                    if(m_queues[(i + n) % m_count].try_pop(f))
                        break;
                if(!f && !m_queues[i].pop(f))
                    break;
                f();
            }
        };

        m_threads.reserve(thread_count);
        for(size_t i = 0; i < thread_count; ++i)
            m_threads.emplace_back(worker, i);
    }

    ~thread_pool()
    {
        for(auto& queue : m_queues)
            queue.unblock();
        for(auto& thread : m_threads)
            thread.join();
    }

    template<typename F, typename... Args>
    void enqueue_work(F&& f, Args&&... args)
    {
        //auto work = [p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() { std::apply(p, t); };
        auto work = [=]() { f(std::forward<Args>(args)...); };
        auto i = m_index++;

        for(size_t n = 0; n < m_count * K; ++n)
            if(m_queues[(i + n) % m_count].try_push(work))
                return;

        m_queues[i % m_count].push(std::move(work));
    }

    template<typename F, typename... Args>
    [[nodiscard]] auto enqueue_task(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using task_return_type = typename std::result_of<F(Args...)>::type;
        using task_type = std::packaged_task<task_return_type()>;

        auto task = std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto work = [=]() { (*task)(); };
        auto result = task->get_future();
        auto i = m_index++;

        for(size_t n = 0; n < m_count * K; ++n)
            if(m_queues[(i + n) % m_count].try_push(work))
                return result;

        m_queues[i % m_count].push(std::move(work));

        return result;
    }

    size_t size() const {
        size_t size = 0;

        for(size_t i = 0; i < m_count; ++i)
            size += m_queues[i].size();

        return size;
    }

    bool empty() const {
        for(size_t i = 0; i < m_count; ++i) {
            if(m_queues[i].size() > 0) {
                return false;
            }
        }

        return true;
    }

private:
    using proc_t = std::function<void(void)>;
    using queue_t = unbounded_queue<proc_t>;
    using queues_t = std::vector<queue_t>;
    queues_t m_queues;

    using threads_t = std::vector<std::thread>;
    threads_t m_threads;

    const std::size_t m_count;
    std::atomic_uint m_index { 0 };

    static const unsigned int K = 2;
};
