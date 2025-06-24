#include <cstddef>
#include <iostream>
#include <list>

struct OrderWindow
{
    using id_type = size_t;

    struct item
    {
        id_type id;
        bool complete;
    };

    void try_notifiy() noexcept // NOLINT
    {
        while (!pending_.empty())
        {
            auto *cur = pending_.front();
            if (cur->complete && next_ == cur->id)
            {
                // NOTE: lock ?
                pending_.pop_front();
                ++next_;
                std::cout << "notifiy: " << cur->id << '\n';
                continue;
            }
            break;
        }
    }
    void complete(item *seq) noexcept
    {
        seq->complete = true;
        try_notifiy();
    }
    void pending(item *seq) noexcept
    {
        pending_.push_back(seq);
    }

    std::list<item *> pending_; // NOLINT
    id_type next_{0};           // NOLINT
};

// NOLINTBEGIN
int main()
{

    OrderWindow window;
    OrderWindow::item items[5] = {{0}, {1}, {2}, {3}, {4}};

    auto reset = [&] noexcept {
        for (int i = 0; i < 5; ++i)
        {
            items[i].complete = false;
            window.next_ = 0;
        }
    };

    // 乱序提交和完成
    // NOTE: post 必须有序。这是前提
    std::cout << "complete: 3 1 2 0 4\n";
    {
        window.pending(&items[0]);
        window.pending(&items[1]);
        window.pending(&items[2]);
        window.pending(&items[3]);

        window.complete(&items[3]); // 不应触发通知（还未post）
        window.complete(&items[1]); // 完成但未到next_
        window.complete(&items[2]); // 完成但未到next_
        window.complete(&items[0]); // 应触发0,1,2的连续通知

        window.pending(&items[4]);
        window.complete(&items[4]); // 触发4
    }
    std::cout << "\ncomplete: 4 2 3 1 0\n";
    reset();
    {
        window.pending(&items[0]);
        window.pending(&items[1]);
        window.pending(&items[2]);
        window.pending(&items[3]);
        window.pending(&items[4]);

        window.complete(&items[4]);
        window.complete(&items[2]);
        window.complete(&items[3]);
        window.complete(&items[1]);
        window.complete(&items[0]);
    }

    std::cout << "\ncomplete: 0 1 2 3 4\n";
    reset();
    {
        window.pending(&items[0]);
        window.pending(&items[1]);
        window.pending(&items[2]);
        window.pending(&items[3]);
        window.pending(&items[4]);

        window.complete(&items[0]);
        window.complete(&items[1]);
        window.complete(&items[2]);
        window.complete(&items[3]);
        window.complete(&items[4]);
    }

    std::cout << "\ncomplete: 0 1 4 3 2\n";
    reset();
    {
        window.pending(&items[0]);
        window.pending(&items[1]);
        window.pending(&items[2]);
        window.pending(&items[3]);
        window.pending(&items[4]);

        window.complete(&items[0]);
        window.complete(&items[1]);
        window.complete(&items[4]);
        window.complete(&items[3]);
        window.complete(&items[2]);
    }

    std::cout << "\ncomplete: 0 2 1 4 3\n";
    reset();
    {
        window.pending(&items[0]);
        window.pending(&items[1]);
        window.pending(&items[2]);

        window.complete(&items[0]);
        window.complete(&items[2]);
        window.complete(&items[1]);

        window.pending(&items[3]);
        window.pending(&items[4]);

        window.complete(&items[4]);
        window.complete(&items[3]);
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND