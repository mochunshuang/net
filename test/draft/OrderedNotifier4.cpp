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
#include <shared_mutex>

// NOLINTBEGIN

class spinlock
{
  private:
    std::atomic<bool> flag{false};

  public:
    void lock()
    {
        while (flag.exchange(true, std::memory_order_acquire))
        {
            std::this_thread::yield();
        }
    }

    bool try_lock()
    {
        return !flag.exchange(true, std::memory_order_acquire);
    }

    void unlock()
    {
        flag.store(false, std::memory_order_release);
    }

    bool is_locked() const
    {
        return flag.load(std::memory_order_relaxed);
    }
};

// NOTE: 仅仅从头部移除
// NOTE: 仅仅从尾部添加

template <typename item>
struct slist
{
    void push_back(item *new_item) noexcept
    {
        if (head_.load(std::memory_order_acquire) == nullptr)
        {
            std::lock_guard lock(front_lock);
            item *expect = head_.load(std::memory_order_acquire);
            if (expect == nullptr)
            {
                while (!head_.compare_exchange_weak(expect, new_item,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed))
                    ; // the body of the loop is empty
            }
        }
        if (tail_.load(std::memory_order_acquire) == nullptr)
        {
            if (back_lock.try_lock())
            {
                item *expect = nullptr;
                while (!tail_.compare_exchange_weak(expect, new_item,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed))
                    ; // the body of the loop is empty
                back_lock.unlock();
            }
        }
        else
        {
            item *expect = tail_.load(std::memory_order_acquire);
            while (!tail_.compare_exchange_weak(
                expect, new_item, std::memory_order_release, std::memory_order_relaxed))
                ; // the body of the loop is empty
        }
    }
    item *pop_front(size_t id)
    {
        item *expect = head_.load(std::memory_order_acquire);
        if (expect == nullptr || expect->id != id || !expect->complete)
            return nullptr;
        while (head_.compare_exchange_weak(
            expect, expect->next, std::memory_order_release, std::memory_order_relaxed))
            return expect;
    }
    item *try_pop_front(size_t id)
    {
        while (true)
        {
            item *expect = head_.load(std::memory_order_acquire);
            if (expect == nullptr || expect->id != id || !expect->complete)
                return nullptr;
            // head_ = head_.next
            if (head_.compare_exchange_weak(expect, expect->next,
                                            std::memory_order_release,
                                            std::memory_order_relaxed))
                return expect;
        }
    }
    bool empty() noexcept
    {
        std::lock_guard lock(front_lock);
        return head_.load(std::memory_order_relaxed) == nullptr;
    }

    std::atomic<item *> head_{nullptr};
    std::atomic<item *> tail_{nullptr};
    spinlock front_lock;
    spinlock back_lock;
};

// 原始 OrderWindow 实现保持不变
struct OrderWindow
{
    using id_type = size_t;

    struct item
    {
        id_type id;
        bool complete;
        item *next;
    };

    void try_notifiy() noexcept
    {
        while (not pending_.empty() &&
               pending_.try_pop_front(next_.load(std::memory_order_acquire)) != nullptr)
        {
            auto old_id = next_.fetch_add(1, std::memory_order_release);
            notify_callback(old_id);
        }
    }

    void complete(item *seq) noexcept
    {
        try_notifiy();
    }

    void pending(item *seq) noexcept
    {
        pending_.push_back(seq);
    }

    slist<item> pending_;
    std::atomic<id_type> next_{0}; // NOTE: 从0 开始
    std::function<void(id_type)> notify_callback;
};

void test_realistic_multi_thread()
{
    constexpr size_t NUM_TASKS = 20;
    constexpr size_t NUM_THREADS = 1;

    OrderWindow window;
    std::vector<OrderWindow::item> items(NUM_TASKS);

    // 记录通知顺序
    std::vector<OrderWindow::id_type> notified_ids;
    window.notify_callback = [&](OrderWindow::id_type id) {
        // NOTE: 不需要锁
        notified_ids.push_back(id);
    };

    // 初始化所有任务
    auto init_test = [&] {
        for (size_t i = 0; i < NUM_TASKS; ++i)
        {
            items[i].id = i;
            items[i].complete = false;
        }
    };
    init_test();

    // 创建线程池
    std::vector<std::thread> workers{NUM_THREADS};
    // 工作线程函数（乱序完成任务）
    auto worker_func = [&](size_t start, size_t end) {
        // 随机打乱任务完成顺序
        std::vector<size_t> indices(end - start);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = start + i;

            window.pending(&items[start + i]); // NOTE: 并发 启动
        }

        std::random_device rd;
        std::shuffle(indices.begin(), indices.end(), std::mt19937(rd()));

        // 完成任务（确保在pending之后）
        for (size_t i : indices) // NOTE: 并发乱序 完成
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            items[i].complete = true;
            window.complete(&items[i]);
        }
    };

    // 启动
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        constexpr auto count = NUM_TASKS / NUM_THREADS;
        size_t start = i * count;
        size_t end = start + count;
        workers[i] = std::move(std::thread{worker_func, start, end});
    }

    // 等待所有工作线程完成
    for (auto &t : workers)
        t.join();

    bool success = true;

    // 检查窗口状态
    if (window.next_ != NUM_TASKS)
    {
        std::cerr << "Error: window.next_ = " << window.next_ << ", expected "
                  << NUM_TASKS << "\n";
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

    assert(success);
}

// NOTE: 失败的原因 是 无法保证 顺序
int main()
{
    {
        {
            std::vector<int> v(3);
            assert(v.size() == 3);
        }
        std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(v.begin(), v.end(), g);

        for (auto &item : v)
        {
            std::cout << item << " ";
        }
        std::cout << '\n';
    }
    int i = 100;
    while (i-- > 0)
    {
        std::cout << "times: " << i << "\n";
        test_realistic_multi_thread();
    }

    std::cout << "Main done\n";
    return 0;
}
// NOLINTEND