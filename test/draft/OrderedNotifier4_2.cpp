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
#include <unordered_set>
#include <shared_mutex>
#include <condition_variable>

// NOLINTBEGIN

// 删除原始的 spinlock 和 slist 实现
class OrderWindow
{
  public:
    using id_type = size_t;

    void complete(id_type id)
    {
        std::unique_lock lock(mutex_);
        completed_ids_.insert(id); // NOTE: 添加
        maybe_notify(lock);
    }

    void set_callback(std::function<void(id_type)> callback)
    {
        notify_callback = std::move(callback);
    }

    id_type get_next() const
    {
        return next_.load();
    }

  private:
    void maybe_notify(std::unique_lock<std::mutex> &lock)
    {
        while (true)
        {
            auto it = completed_ids_.find(next_.load()); // NOTE: 要求这里最严格才行
            if (it == completed_ids_.end())
                break;

            auto current = next_.fetch_add(1, std::memory_order_relaxed);
            completed_ids_.erase(it);
            lock.unlock(); // 解锁后再回调避免死锁
            if (notify_callback)
                notify_callback(current);
            lock.lock();
        }
    }

    mutable std::mutex mutex_;
    std::unordered_set<id_type> completed_ids_;
    std::atomic<id_type> next_{0};
    std::function<void(id_type)> notify_callback;
};

void test_realistic_multi_thread()
{
    constexpr size_t NUM_TASKS = 20;
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
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);

        for (size_t i : indices)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            window.complete(i);
        }
    };

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

    int tests = 1000;
    while (tests-- > 0)
    {
        std::cout << "Remaining tests: " << tests << "\n";
        test_realistic_multi_thread();
    }
    std::cout << "All tests passed!\n";
    return 0;
}
// NOLINTEND