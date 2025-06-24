#include <iostream>
#include <numeric>
#include <unordered_set>
#include <vector>
#include <random>
#include <format>
#include <chrono>

// NOLINTBEGIN

class MonitoredSet : public std::unordered_set<int>
{
  public:
    size_t rehash_count = 0;
    size_t max_bucket_count = 0;
    size_t min_bucket_count = 1000000;

    void insert_and_monitor(int value)
    {
        // 记录插入前的哈希表状态
        size_t old_bucket_count = bucket_count();
        float old_load_factor = load_factor();

        // 执行插入
        this->insert(value);

        // 检测是否发生重新哈希
        if (bucket_count() != old_bucket_count)
        {
            rehash_count++;
            max_bucket_count = std::max(max_bucket_count, bucket_count());
            min_bucket_count = std::min(min_bucket_count, bucket_count());
            // 记录重新哈希详细信息
            std::cout << std::format("Rehash #{:<3} | Size: {:<3} | Buckets: {:<4} → "
                                     "{:<4} | Load: {:.2f} → {:.2f}\n",
                                     rehash_count, size(), old_bucket_count,
                                     bucket_count(), old_load_factor, load_factor());
        }
    }
};

int main()
{
    constexpr int TOTAL_EVENTS = 1000;
    constexpr int TOTAL_RUNS = 1'000;

    // 重新哈希统计
    size_t total_rehashes = 0;
    size_t max_rehashes_per_run = 0;
    size_t max_bucket_count_global = 0;
    size_t min_bucket_count_global = TOTAL_RUNS * TOTAL_EVENTS;

    // 操作统计
    size_t total_insertions = 0;
    size_t total_deletions = 0;

    std::random_device rd;
    std::mt19937 rng(rd());

    auto start = std::chrono::high_resolution_clock::now();

    for (int run = 0; run < TOTAL_RUNS; ++run)
    {
        MonitoredSet completed_events;
        int next = 1;
        size_t run_rehashes = 0;

        // 创建事件序列
        std::vector<int> event_order(TOTAL_EVENTS);
        std::iota(event_order.begin(), event_order.end(), 1);
        std::shuffle(event_order.begin(), event_order.end(), rng);

        for (int id : event_order)
        {
            // 监控插入操作
            completed_events.insert_and_monitor(id);
            total_insertions++;

            // 处理连续完成事件
            size_t deletions_in_step = 0;
            while (completed_events.contains(next))
            {
                completed_events.erase(next);
                next++;
                deletions_in_step++;
            }
            total_deletions += deletions_in_step;
        }

        // 更新重新哈希统计
        run_rehashes = completed_events.rehash_count;
        total_rehashes += run_rehashes;
        max_rehashes_per_run = std::max(max_rehashes_per_run, run_rehashes);
        max_bucket_count_global =
            std::max(max_bucket_count_global, completed_events.max_bucket_count);
        min_bucket_count_global =
            std::min(min_bucket_count_global, completed_events.min_bucket_count);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 输出统计结果
    std::cout << "\n===== 哈希表性能分析 =====\n";
    std::cout << std::format("总实验次数: {}\n", TOTAL_RUNS);
    std::cout << std::format("总插入操作: {}\n", total_insertions);
    std::cout << std::format("总删除操作: {}\n", total_deletions);
    std::cout << std::format("总重新哈希次数: {}\n", total_rehashes);
    std::cout << std::format("每次实验平均重新哈希次数: {:.2f}\n",
                             static_cast<double>(total_rehashes) / TOTAL_RUNS);
    std::cout << std::format("单次实验最大重新哈希次数: {}\n", max_rehashes_per_run);
    std::cout << std::format("最大/最小桶数量: {}/{}\n", max_bucket_count_global,
                             min_bucket_count_global);
    std::cout << std::format("总执行时间: {} ms\n", duration.count());

    // 分析重新哈希分布
    std::cout << "\n===== 重新哈希行为分析 =====\n";
    std::cout << "| 发生频率           | 典型场景                     |\n";
    std::cout << "|--------------------|------------------------------|\n";
    std::cout << "| 插入时 (高概率)    | 当元素数量超过负载因子阈值时 |\n";
    std::cout << "| 删除时 (从不)      | unordered_set 不因删除而重组 |\n";
    std::cout << "| reserve() 后 (低)  | 预分配后插入可能跳过重组     |\n";

    // NOTE: hash 次数是稳定的
    return 0;
}
// NOLINTEND