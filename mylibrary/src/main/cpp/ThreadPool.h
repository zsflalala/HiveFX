#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {

public:
    explicit ThreadPool(size_t threadCount) : m_Stop(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            m_Workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_QueueMutex);
                        m_Condition.wait(lock, [this]() { return m_Stop || !m_Tasks.empty(); });
                        if (m_Stop && m_Tasks.empty()) return;
                        task = std::move(m_Tasks.front());
                        m_Tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }
        m_Condition.notify_all();
        for (std::thread &worker : m_Workers) {
            worker.join();
        }
    }

    template <typename F, typename... Args>
    void enqueueTask(F &&f, Args &&... args) {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Tasks.emplace(std::move(task));
        }
        m_Condition.notify_one();
    }

private:
    std::vector<std::thread> m_Workers;
    std::queue<std::function<void()>> m_Tasks;
    std::mutex m_QueueMutex;
    std::condition_variable m_Condition;
    bool m_Stop;
};