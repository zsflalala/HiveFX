#include "ThreadPool.h"

CThreadPool::CThreadPool(size_t vThreadCount): m_Stop(false)
{
    for (size_t i = 0; i < vThreadCount; ++i)
    {
        m_Workers.emplace_back([this]()
       {
           while (true)
           {
               std::function<void()> Task;
               {
                   std::unique_lock<std::mutex> Lock(m_QueueMutex);
                   m_Condition.wait(Lock, [this]() { return m_Stop || !m_Tasks.empty(); });
                   if (m_Stop && m_Tasks.empty()) return;
                   Task = std::move(m_Tasks.front());
                   m_Tasks.pop();
               }
               Task();
           }
       });
    }
}

CThreadPool::~CThreadPool()
{
    std::unique_lock<std::mutex> Lock(m_QueueMutex);
    m_Stop = true;
    m_Condition.notify_all();
    for (std::thread &Worker : m_Workers)
    {
        Worker.join();
    }
}
