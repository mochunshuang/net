#include <functional>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>

class OrderWindow
{
  public:
    using SeqType = uint64_t;
    using Callback = std::function<void(SeqType begin, SeqType end)>;

    explicit OrderWindow(Callback cb) : callback_(std::move(cb)), confirmed_end_(0) {}

    SeqType PostIO()
    {
        const SeqType seq = next_seq_++;
        pending_.push_back(seq);
        return seq;
    }

    void CompleteIO(SeqType seq)
    {
        if (seq <= confirmed_end_)
            return; // 过滤已确认的包

        completed_.insert(seq);

        SeqType new_end = confirmed_end_;
        while (completed_.count(new_end + 1))
        {
            new_end++;
        }

        if (new_end > confirmed_end_)
        {
            const SeqType old_end = confirmed_end_;
            confirmed_end_ = new_end;
            CleanPending(new_end);
            callback_(old_end + 1, new_end);
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
    { // 新增重置方法
        next_seq_ = 1;
        confirmed_end_ = 0;
        pending_.clear();
        completed_.clear();
    }

    //   private:
    SeqType next_seq_ = 1;
    SeqType confirmed_end_ = 0;
    std::vector<SeqType> pending_;
    std::unordered_set<SeqType> completed_;
    Callback callback_;
};

void TestSequentialAck()
{
    // 测试用例1
    {
        std::vector<std::pair<uint64_t, uint64_t>> callback_records;
        OrderWindow window([&](auto b, auto e) { callback_records.emplace_back(b, e); });

        for (int i = 0; i < 9; ++i)
            window.PostIO();
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

    // 测试用例2（修复部分）
    {
        std::vector<std::pair<uint64_t, uint64_t>> callback_records;
        OrderWindow window([&](auto b, auto e) { callback_records.emplace_back(b, e); });

        // 提交1-9
        for (int i = 0; i < 9; ++i)
            window.PostIO();

        // 按顺序完成1-9（注意序列号从1开始）
        for (uint64_t seq = 1; seq <= 9; ++seq)
        {
            window.CompleteIO(seq);
        }

        // 应触发单次回调[1-9]
        assert(callback_records.size() == 9);
        assert(callback_records[0] == std::make_pair(1, 1));
        assert(window.pending_.empty());
        assert(window.completed_.empty());
    }

    // 测试用例3（验证边界重置）
    {
        OrderWindow window([](auto, auto) {});
        window.Reset(); // 显式重置
        assert(window.PostIO() == 1);
        assert(window.PostIO() == 2);
    }
}

int main()
{
    TestSequentialAck();
    std::cout << "All tests passed!\n";
    return 0;
}