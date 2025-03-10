#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class CThreadPool
{
public:
    explicit CThreadPool(size_t vThreadCount);
    ~CThreadPool();

    template <typename F, typename... Args>
    void enqueueTask(F &&f, Args &&... args)
    {
        auto Task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> Lock(m_QueueMutex);
            m_Tasks.emplace(std::move(Task));
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