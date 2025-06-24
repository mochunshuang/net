#include <atomic>
#include <cstddef>
#include <iostream>
#include <list>
#include <mutex>
#include <random>
#include <thread>
#include <vector>
#include <condition_variable>
#include <barrier>
#include <functional>

struct OrderWindow
{
    using id_type = size_t;

    struct item
    {
        id_type id;
        std::atomic<bool> complete;
    };

    struct spinlock
    {
      private:
        std::atomic_flag m_{};

      public:
        void lock() noexcept
        {
            while (m_.test_and_set(std::memory_order_acquire))
            {
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
                m_.wait(true, std::memory_order_relaxed);
#endif
            }
        }
        bool try_lock() noexcept
        {
            return !m_.test_and_set(std::memory_order_acquire);
        }
        void unlock() noexcept
        {
            m_.clear(std::memory_order_release);
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
            m_.notify_one();
#endif
        }
    };

    void try_notifiy() noexcept
    {
        if (front_lock.try_lock())
        {
            while (!pending_.empty())
            {
                auto *cur = pending_.front();
                if (cur->complete.load(std::memory_order_relaxed) && next_ == cur->id)
                {
                    pending_.pop_front();
                    ++next_;
                    if (notify_callback)
                    {
                        notify_callback(cur->id);
                    }
                    else
                    {
                        std::cout << "notify: " << cur->id << '\n';
                    }
                    continue;
                }
                break;
            }
            front_lock.unlock();
        }
    }

    void complete(item *seq) noexcept
    {
        seq->complete.store(true, std::memory_order_release);
        try_notifiy();
    }

    void pending(item *seq) noexcept
    {
        std::lock_guard lock{back_lock};
        pending_.push_back(seq);
    }

    spinlock front_lock;
    spinlock back_lock;
    std::list<item *> pending_;
    id_type next_{0};
    std::function<void(id_type)> notify_callback;
};

void test_multi()
{
    constexpr size_t N = 1000;    // 任务数量
    constexpr size_t WORKERS = 4; // 工作线程数
    OrderWindow window;
    std::vector<OrderWindow::item> items(N);

    // 记录通知顺序
    std::vector<OrderWindow::id_type> notified_ids;
    std::mutex notification_mutex;
    window.notify_callback = [&](OrderWindow::id_type id) {
        std::lock_guard<std::mutex> lock(notification_mutex);
        notified_ids.push_back(id);
    };

    // 初始化所有任务
    for (size_t i = 0; i < N; ++i)
    {
        items[i].id = i;
        items[i].complete = false;
    }

    // 提交所有任务（主线程按顺序提交）
    for (size_t i = 0; i < N; ++i)
    {
        window.pending(&items[i]);
    }

    // 工作线程函数（乱序完成任务）
    auto worker_func = [&](size_t start, size_t end) {
        // 随机打乱任务完成顺序
        std::vector<size_t> indices(end - start);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = start + i;
        }

        std::random_device rd;
        std::shuffle(indices.begin(), indices.end(), std::mt19937(rd()));

        // 完成任务（确保在pending之后）
        for (size_t i : indices)
        {
            // 模拟实际工作延迟
            std::this_thread::sleep_for(std::chrono::microseconds(10));

            items[i].complete = true;
            window.complete(&items[i]);
        }
    };

    // 启动工作线程
    std::vector<std::thread> workers;
    size_t chunk = N / WORKERS;
    for (size_t i = 0; i < WORKERS; ++i)
    {
        size_t start = i * chunk;
        size_t end = (i == WORKERS - 1) ? N : start + chunk;
        workers.emplace_back(worker_func, start, end);
    }

    // 等待所有工作线程完成
    for (auto &t : workers)
        t.join();

    // 最终触发可能遗漏的通知
    window.try_notifiy();

    // 验证结果
    bool success = true;

    // 检查通知数量
    if (notified_ids.size() != N)
    {
        std::cerr << "Error: " << notified_ids.size() << " notified, expected " << N
                  << "\n";
        success = false;
    }

    // 检查通知顺序
    for (size_t i = 0; i < notified_ids.size(); ++i)
    {
        if (notified_ids[i] != i)
        {
            std::cerr << "Error: Position " << i << ": expected " << i << ", got "
                      << notified_ids[i] << "\n";
            success = false;
            break;
        }
    }

    // 检查窗口状态
    if (window.next_ != N)
    {
        std::cerr << "Error: window.next_ = " << window.next_ << ", expected " << N
                  << "\n";
        success = false;
    }

    if (success)
    {
        std::cout << "Multi-threaded test PASSED\n";
    }
    else
    {
        std::cout << "Multi-threaded test FAILED\n";
    }
}

int main()
{
    test_multi();
    std::cout << "Main done\n";
    return 0;
}