#include <atomic>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <random>
#include <thread>
#include <utility>
#include <vector>
#include <chrono>
#include <memory>

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

struct OrderWindow
{
    using id_type = size_t;

    struct item
    {
        id_type id;
        bool complete;
    };

    std::mutex mutex_;
    std::vector<item *> items_; // Store items by ID
    id_type next_{0};
    std::function<void(id_type)> notify_callback;

  public:
    void pending(item *seq) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (seq->id >= items_.size())
        {
            items_.resize(seq->id + 1, nullptr);
        }
        items_[seq->id] = seq;
    }

    void complete(item *seq) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // Notify all consecutive completed tasks starting from `next_`
        while (next_ < items_.size() && items_[next_] != nullptr &&
               items_[next_]->complete)
        {
            if (notify_callback)
            {
                notify_callback(next_);
            }
            ++next_;
        }
    }
};

void test_realistic_multi_thread()
{
    constexpr size_t NUM_TASKS = 20;
    constexpr size_t NUM_THREADS = 4;

    OrderWindow window;
    std::vector<OrderWindow::item> items(NUM_TASKS);

    std::vector<OrderWindow::id_type> notified_ids;
    window.notify_callback = [&](OrderWindow::id_type id) {
        notified_ids.push_back(id);
    };

    auto init_test = [&] {
        for (size_t i = 0; i < NUM_TASKS; ++i)
        {
            items[i].id = i;
            items[i].complete = false;
        }
    };
    init_test();

    std::vector<std::thread> workers{NUM_THREADS};
    auto worker_func = [&](size_t start, size_t end) {
        std::vector<size_t> indices(end - start);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = start + i;
            window.pending(&items[start + i]);
        }

        std::random_device rd;
        std::shuffle(indices.begin(), indices.end(), std::mt19937(rd()));

        for (size_t i : indices)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            items[i].complete = true;
            window.complete(&items[i]);
        }
    };

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        constexpr auto count = NUM_TASKS / NUM_THREADS;
        size_t start = i * count;
        size_t end = start + count;
        workers[i] = std::thread(worker_func, start, end);
    }

    for (auto &t : workers)
        t.join();

    bool success = true;

    if (window.next_ != NUM_TASKS)
    {
        std::cerr << "Error: window.next_ = " << window.next_ << ", expected "
                  << NUM_TASKS << "\n";
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

    assert(success);
}

int main()
{
    {
        std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(v.begin(), v.end(), g);
        for (auto &item : v)
            std::cout << item << " ";
        std::cout << '\n';
    }

    int i = 1000;
    while (i-- > 0)
    {
        std::cout << "times: " << i << "\n";
        test_realistic_multi_thread();
    }

    std::cout << "Main done\n";
    return 0;
}
// NOLINTEND