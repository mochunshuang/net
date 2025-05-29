#include <functional>
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_set>
#include <cassert>

// NOLINTBEGIN

class OrderWindow
{
  public:
    using SeqType = uint64_t;
    using Callback = std::function<void(SeqType begin, SeqType end)>;

    explicit OrderWindow(Callback cb, SeqType min_batch_size = 1)
        : callback_(std::move(cb)), confirmed_end_(0), min_batch_size_(min_batch_size)
    {
    }

    void PostIO(SeqType seq)
    {
        if (!pending_.empty())
        {
            assert(seq > pending_.back());
        }
        pending_.push_back(seq);
    }

    void CompleteIO(SeqType seq)
    {
        if (seq <= confirmed_end_)
            return;

        completed_.insert(seq);

        SeqType new_end = confirmed_end_;
        while (completed_.count(new_end + 1))
        {
            new_end++;
        }

        if (new_end > confirmed_end_)
        {
            const SeqType current_max_pending =
                pending_.empty() ? confirmed_end_ : pending_.back();
            const SeqType batch_size = new_end - confirmed_end_;

            if (batch_size >= min_batch_size_ || new_end == current_max_pending)
            {
                const SeqType old_end = confirmed_end_;
                confirmed_end_ = new_end;
                CleanPending(new_end);
                callback_(old_end + 1, new_end);
            }
        }
    }

    void CleanPending(SeqType up_to)
    {
        auto it = pending_.begin();
        while (it != pending_.end() && *it <= up_to)
        {
            completed_.erase(*it);
            it = pending_.erase(it);
        }
    }

    void Reset()
    {
        confirmed_end_ = 0;
        pending_.clear();
        completed_.clear();
    }

    // 以下成员变量可设为private，测试代码中暂时公开
    SeqType confirmed_end_ = 0;
    std::vector<SeqType> pending_;
    std::unordered_set<SeqType> completed_;
    Callback callback_;
    SeqType min_batch_size_;
};

void TestSequentialAck()
{
    // 测试用例1: 乱序完成，批大小=1
    {
        std::vector<std::pair<uint64_t, uint64_t>> callback_records;
        OrderWindow window([&](auto b, auto e) { callback_records.emplace_back(b, e); },
                           1);

        for (uint64_t i = 1; i <= 9; ++i)
            window.PostIO(i);
        const std::vector<OrderWindow::SeqType> completion_order = {3, 2, 1, 6, 5,
                                                                    7, 4, 8, 9};
        for (auto seq : completion_order)
            window.CompleteIO(seq);

        const std::vector<std::pair<uint64_t, uint64_t>> expected = {
            {1, 3}, {4, 7}, {8, 8}, {9, 9}};
        assert(callback_records == expected);
        assert(window.pending_.empty());
        assert(window.completed_.empty());
    }

    // 测试用例2: 顺序完成，批大小=3
    {
        std::vector<std::pair<uint64_t, uint64_t>> callback_records;
        OrderWindow window([&](auto b, auto e) { callback_records.emplace_back(b, e); },
                           3);

        for (uint64_t i = 1; i <= 9; ++i)
            window.PostIO(i);
        for (uint64_t seq = 1; seq <= 9; ++seq)
            window.CompleteIO(seq);

        assert(callback_records.size() == 3);
        assert(callback_records[0] == std::make_pair(1ULL, 3ULL));
        assert(callback_records[1] == std::make_pair(4ULL, 6ULL));
        assert(callback_records[2] == std::make_pair(7ULL, 9ULL));
        assert(window.pending_.empty());
        assert(window.completed_.empty());
    }

    // 测试用例3: 边界重置验证
    {
        OrderWindow window([](auto, auto) {}, 1);
        window.Reset();
        window.PostIO(1);
        window.PostIO(2);
        assert(window.pending_ == std::vector<uint64_t>({1, 2}));
    }
    // std::numeric_limits<int>::max();
}

int main()
{
    TestSequentialAck();
    std::cout << "All tests passed!\n";
    return 0;
}
// NOLINTEND