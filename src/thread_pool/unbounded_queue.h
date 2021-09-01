#pragma once

#include <mutex>
#include <queue>
//#include <utility>
//#include <stdexcept>
#include <condition_variable>


template<typename T>
class unbounded_queue
{
public:
    explicit unbounded_queue(bool block = true)
    : m_block{ block } {}

    void push(const T& item)
    {
        {
            std::lock_guard<std::mutex> guard(m_queue_lock);
            m_queue.push(item);
        }
        m_condition.notify_one();
    }

    void push(T&& item)
    {
        {
            std::lock_guard<std::mutex> guard(m_queue_lock);
            m_queue.push(std::move(item));
        }
        m_condition.notify_one();
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        {
            std::lock_guard<std::mutex> guard(m_queue_lock);
            m_queue.emplace(std::forward<Args>(args)...);
        }
        m_condition.notify_one();
    }

    bool try_push(const T& item)
    {
        {
            std::unique_lock<std::mutex> lock(m_queue_lock, std::try_to_lock);
            if(!lock)
                return false;
            m_queue.push(item);
        }
        m_condition.notify_one();
        return true;
    }

    bool try_push(T&& item)
    {
        {
            std::unique_lock<std::mutex> lock(m_queue_lock, std::try_to_lock);
            if(!lock)
                return false;
            m_queue.push(std::move(item));
        }
        m_condition.notify_one();
        return true;
    }

    bool pop(T& item)
    {
        std::unique_lock<std::mutex> guard(m_queue_lock);
        m_condition.wait(guard, [&]() { return !m_queue.empty() || !m_block; });
        if(m_queue.empty())
            return false;
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool try_pop(T& item)
    {
        std::unique_lock<std::mutex> lock(m_queue_lock, std::try_to_lock);
        if(!lock || m_queue.empty())
            return false;
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> guard(m_queue_lock);
        return m_queue.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> guard(m_queue_lock);
        return m_queue.empty();
    }

    void block()
    {
        std::lock_guard<std::mutex> guard(m_queue_lock);
        m_block = true;
    }

    void unblock()
    {
        {
            std::lock_guard<std::mutex> guard(m_queue_lock);
            m_block = false;
        }
        m_condition.notify_all();
    }

    bool blocking() const
    {
        std::lock_guard<std::mutex> guard(m_queue_lock);
        return m_block;
    }

private:
    using queue_t = std::queue<T>;
    queue_t m_queue;

    bool m_block;

    mutable std::mutex m_queue_lock;
    std::condition_variable m_condition;
};
