#include <iostream>
#include <iomanip>
#include <memory_resource>
#include <vector>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <string>
#include <map>

// NOLINTBEGIN

// 测量函数执行时间的工具
template <typename Func>
double measure_time(Func func)
{
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// 测试配置
struct TestConfig
{
    size_t object_size;      // 对象大小
    size_t allocation_count; // 分配次数
    size_t max_blocks;       // max_blocks_per_chunk
    size_t largest_block;    // largest_required_pool_block
    std::string description; // 配置描述
};

// 测试函数
double run_test(const TestConfig &config, int warmup_runs = 3, int measurement_runs = 5)
{
    // 热身运行，稳定环境
    for (int i = 0; i < warmup_runs; ++i)
    {
        std::pmr::pool_options pool_opts;
        pool_opts.max_blocks_per_chunk = config.max_blocks;
        pool_opts.largest_required_pool_block = config.largest_block;

        std::pmr::unsynchronized_pool_resource pool{pool_opts,
                                                    std::pmr::new_delete_resource()};
        std::pmr::vector<char> vec{&pool};

        measure_time([&] {
            for (size_t i = 0; i < config.allocation_count; ++i)
            {
                vec.resize(config.object_size);
                vec.clear();
            }
        });
    }

    // 实际测量，取多次运行的平均值
    double total_time = 0.0;
    for (int i = 0; i < measurement_runs; ++i)
    {
        std::pmr::pool_options pool_opts;
        pool_opts.max_blocks_per_chunk = config.max_blocks;
        pool_opts.largest_required_pool_block = config.largest_block;

        std::pmr::unsynchronized_pool_resource pool{pool_opts,
                                                    std::pmr::new_delete_resource()};
        std::pmr::vector<char> vec{&pool};

        total_time += measure_time([&] {
            for (size_t i = 0; i < config.allocation_count; ++i)
            {
                vec.resize(config.object_size);
                vec.clear();
            }
        });
    }

    return total_time / measurement_runs;
}

// 运行一组测试并分析结果
void run_test_suite(const std::vector<TestConfig> &configs, const std::string &test_name)
{
    std::cout << "\n=== " << test_name << " ===" << std::endl;

    // 存储所有测试结果
    std::map<std::string, double> results;

    // 运行所有测试配置
    for (const auto &config : configs)
    {
        double time = run_test(config);
        results[config.description] = time;
        std::cout << config.description << " - 平均耗时: " << std::fixed
                  << std::setprecision(3) << time << " ms" << std::endl;
    }

    // 找出最佳配置
    std::string best_config;
    double best_time = std::numeric_limits<double>::max();

    for (const auto &[desc, time] : results)
    {
        if (time < best_time)
        {
            best_time = time;
            best_config = desc;
        }
    }

    std::cout << "\n最佳配置: " << best_config << " (耗时: " << best_time << " ms)"
              << std::endl;

    // 打印所有配置的性能对比
    std::cout << "\n性能对比 (相对于最佳配置):" << std::endl;
    for (const auto &[desc, time] : results)
    {
        if (desc == best_config)
            continue;

        double diff = time - best_time;
        double percent_diff = diff / best_time * 100;

        std::cout << desc << ": 差异 " << (diff > 0 ? "+" : "") << diff << " ms ("
                  << (diff > 0 ? "+" : "") << percent_diff << "%)" << std::endl;
    }
}

int main()
{
    // 统一的分配次数
    const size_t ALLOCATION_COUNT = 1000;
    const size_t k_object_size = 128;

    // 测试1: 不同 max_blocks_per_chunk 值
    {
        std::vector<TestConfig> test_configs;
        for (auto mb : {16u, 32u, 64u, 128u, 256u, 512u, 1024u})
        {
            test_configs.push_back({k_object_size,    // 对象大小
                                    ALLOCATION_COUNT, // 分配次数
                                    mb,               // max_blocks_per_chunk
                                    512,              // largest_required_pool_block
                                    "max_blocks=" + std::to_string(mb)});
        }
        run_test_suite(test_configs, "测试1: max_blocks_per_chunk 的影响");
    }

    // 测试2: 不同 largest_required_pool_block 值
    {
        std::vector<TestConfig> test_configs;
        for (auto lb : {64u, 128u, 256u, 512u, 1024u})
        {
            test_configs.push_back({k_object_size,    // 对象大小
                                    ALLOCATION_COUNT, // 分配次数
                                    256,              // max_blocks_per_chunk
                                    lb,               // largest_required_pool_block
                                    "largest_block=" + std::to_string(lb)});
        }
        run_test_suite(test_configs, "测试2: largest_required_pool_block 的影响");
    }

    // 测试3: 对象大小与 largest_required_pool_block 的关系
    {
        std::vector<TestConfig> test_configs;
        for (auto obj_size : {32u, 64u, 128u, 256u, 512u})
        {
            test_configs.push_back(
                {obj_size,         // 对象大小
                 ALLOCATION_COUNT, // 分配次数
                 256,              // max_blocks_per_chunk
                 obj_size,         // largest_required_pool_block (匹配对象大小)
                 "对象大小=" + std::to_string(obj_size)});
        }
        run_test_suite(test_configs,
                       "测试3: 对象大小与 largest_required_pool_block 匹配的影响");
    }

    // NOTE: 无法得出一个结论，因为最佳配置的结果随调用次数发生变化

    return 0;
}
// NOLINTEND