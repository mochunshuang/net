#include <atomic>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <utility>
#include <vector>
#include <chrono>
#include <condition_variable>

// NOLINTBEGIN

#include <atomic>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <utility>
#include <vector>
#include <chrono>
#include <list>
#include <unordered_set>

class OrderWindow
{
  public:
    using id_type = size_t;

    void complete(id_type id)
    {
        std::unique_lock lock(mutex_);

        // 如果ID小于下一个期望值，直接忽略（已处理）
        // if (id < next_) //NOTE: 重复发生回调可能需要这个
        //     return;

        // 存储完成事件
        completed_events_.insert(id);

        // 检查连续完成事件
        while (completed_events_.count(next_))
        {
            completed_events_.erase(next_);

            // 解锁后执行回调，避免死锁
            lock.unlock();
            if (notify_callback)
                notify_callback(next_);
            lock.lock();

            ++next_;
        }
    }

    void set_callback(std::function<void(id_type)> callback)
    {
        notify_callback = std::move(callback);
    }

    id_type get_next() const
    {
        return next_;
    }

  private:
    std::mutex mutex_;
    std::unordered_set<id_type> completed_events_; // 存储完成事件
    id_type next_ = 0;                             // 下一个期望的ID
    std::function<void(id_type)> notify_callback;
};

// 测试代码保持不变

void test_realistic_multi_thread()
{
    constexpr size_t NUM_TASKS = 200;
    constexpr size_t NUM_THREADS = 4;

    OrderWindow window;
    std::vector<OrderWindow::id_type> notified_ids;
    window.set_callback([&](OrderWindow::id_type id) { notified_ids.push_back(id); });

    std::vector<std::thread> workers;
    workers.reserve(NUM_THREADS);

    auto worker_func = [&](size_t start, size_t end) {
        std::vector<size_t> indices(end - start);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = start + i;
        }

        std::random_device rd;
        std::mt19937 g(rd()); // NOTE: 随机范围完成
        std::shuffle(indices.begin(), indices.end(), g);

        for (size_t i : indices)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            window.complete(i);
        }
    };

    // start workers
    for (size_t i = 0; i < NUM_THREADS; ++i)
    {
        size_t start = i * (NUM_TASKS / NUM_THREADS);
        size_t end =
            (i == NUM_THREADS - 1) ? NUM_TASKS : start + (NUM_TASKS / NUM_THREADS);
        workers.emplace_back(worker_func, start, end);
    }
    for (auto &t : workers)
        t.join();

    bool success = true;
    if (window.get_next() != NUM_TASKS)
    {
        std::cerr << "Error: window.next_ = " << window.get_next() << ", expected "
                  << NUM_TASKS << "\n";
        success = false;
    }

    {
        if (notified_ids.size() != NUM_TASKS)
        {
            std::cerr << "Error: notified_ids.size() = " << notified_ids.size()
                      << ", expected " << NUM_TASKS << "\n";
            success = false;
        }
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
    }
    assert(success);
}

int main()
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    for (auto &item : v)
        std::cout << item << " ";
    std::cout << '\n';

    int tests = 10000;
    while (tests-- > 0)
    {
        std::cout << "Remaining tests: " << tests << "\n";
        test_realistic_multi_thread();
    }
    std::cout << "All tests passed!\n";
    return 0;
}
// NOLINTEND