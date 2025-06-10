#include <array>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory_resource>

// NOLINTBEGIN

template <typename Func>
auto benchmark(Func test_func, int iterations)
{
    const auto start = std::chrono::system_clock::now();
    while (iterations-- > 0)
        test_func();
    const auto stop = std::chrono::system_clock::now();
    const auto secs = std::chrono::duration<double>(stop - start);
    return secs.count();
}

int main()
{
    constexpr int iterations{10};
    constexpr int total_nodes{1'00'00};

    auto default_std_alloc = [total_nodes] {
        std::list<int> list;
        for (int i{}; i != total_nodes; ++i)
            list.push_back(i);
    };

    auto default_pmr_alloc = [total_nodes] {
        std::pmr::list<int> list;
        for (int i{}; i != total_nodes; ++i)
            list.push_back(i);
    };

    auto pmr_alloc_no_buf = [total_nodes] {
        std::pmr::monotonic_buffer_resource mbr;
        std::pmr::polymorphic_allocator<int> pa{&mbr};
        std::pmr::list<int> list{pa};
        for (int i{}; i != total_nodes; ++i)
            list.push_back(i);
    };

    auto pmr_alloc_and_buf = [total_nodes] {
        std::array<std::byte, total_nodes * 32> buffer; // enough to fit in all nodes
        std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size()};
        std::pmr::polymorphic_allocator<int> pa{&mbr};
        std::pmr::list<int> list{pa};
        for (int i{}; i != total_nodes; ++i)
            list.push_back(i);
    };

    const double t1 = benchmark(default_std_alloc, iterations);
    const double t2 = benchmark(default_pmr_alloc, iterations);
    const double t3 = benchmark(pmr_alloc_no_buf, iterations);
    const double t4 = benchmark(pmr_alloc_and_buf, iterations);

    // NOTE: t1 是基准。比值越大越好。大数据量看没多少差距。 层数多是有开销的
    std::cout << std::fixed << std::setprecision(3) << "t1 (default std alloc): " << t1
              << " sec; t1/t1: " << t1 / t1 << '\n'
              << "t2 (default pmr alloc): " << t2 << " sec; t1/t2: " << t1 / t2 << '\n'
              << "t3 (pmr alloc  no buf): " << t3 << " sec; t1/t3: " << t1 / t3 << '\n'
              << "t4 (pmr alloc and buf): " << t4 << " sec; t1/t4: " << t1 / t4 << '\n';

    // NOTE: 申请和释放的速度new/delete比不过pmr::xxx_resource，但std::pmr::集合导致这结果
}

// NOTE: 测试的数据量 从 大 到小
/*

t1 (default std alloc): 0.020 sec; t1/t1: 1.000
t2 (default pmr alloc): 0.029 sec; t1/t2: 0.693
t3 (pmr alloc  no buf): 0.018 sec; t1/t3: 1.075
t4 (pmr alloc and buf): 0.019 sec; t1/t4: 1.024

t1 (default std alloc): 0.001 sec; t1/t1: 1.000
t2 (default pmr alloc): 0.001 sec; t1/t2: 0.688
t3 (pmr alloc  no buf): 0.001 sec; t1/t3: 1.068
t4 (pmr alloc and buf): 0.001 sec; t1/t4: 0.958

t1 (default std alloc): 0.001 sec; t1/t1: 1.000
t2 (default pmr alloc): 0.002 sec; t1/t2: 0.470
t3 (pmr alloc  no buf): 0.001 sec; t1/t3: 0.899
t4 (pmr alloc and buf): 0.001 sec; t1/t4: 0.806

*/
// NOLINTEND