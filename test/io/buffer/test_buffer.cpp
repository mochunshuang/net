#include <cassert>
#include <iostream>

#include "../../test_common/test_macro.hpp"
#include "../../../include/__detail/io/__buffer.hpp"

// NOLINTBEGIN

int main()
{
    namespace io = mcs::net::io::buffer;
    // using namespace mcs::net::io::buffer;

    // 1. 正确转换 mutable_buffer -> const_buffer
    TEST("Buffer Copy - Edge Cases") = [] {
        constexpr std::array src_data = {1, 2, 3, 4};
        auto src = io::buffer(src_data);

        std::array dest_data = {0, 0, 0, 0};
        auto dest = io::buffer(dest_data);

        // 关键：显式转换确保类型安全
        io::const_buffer const_src{src}; // NOTE: 允许
        // io::const_buffer const_src = src; // 不允许。应该允许
        size_t copied = buffer_copy(dest, const_src);
        std::cout << "copied: " << copied << '\n';
        EXPECT(copied == 4 * sizeof(decltype(src_data)::value_type));
    };

    TEST("Buffer Copy - Edge Cases") = [] {
        std::vector<char> empty_vec;
        auto src = io::buffer(empty_vec);
        char dest[10] = {};
        auto copied = io::buffer_copy(io::buffer(dest), src);

        EXPECT(copied == 0);
    };

    TEST("Truncated copy") = [] {
        const std::array src = {1, 2, 3, 4, 5}; // 使用const确保生成const_buffer
        auto const_src = io::buffer(src);

        std::array dest = {0, 0, 0};
        auto mutable_dest = io::buffer(dest);

        size_t copied = io::buffer_copy(mutable_dest, const_src);
        assert(copied == 12); // 3个int * 4字节

        // 验证前3个元素
        assert(dest[0] == 1);
        assert(dest[1] == 2);
        assert(dest[2] == 3);
    };

    {
        char raw_data[1024];
        const char const_raw_data[1024] = "";
        void *void_ptr_data = raw_data;
        const void *const_void_ptr_data = const_raw_data;
        std::span<char> span_1{raw_data, 1024};
        std::span<const char> span_2{raw_data, 1024};
        std::array<char, 1024> std_array_data;
        const std::array<char, 1024> &const_std_array_data_1 = std_array_data;
        std::array<const char, 1024> const_std_array_data_2 = {{0}};
        std::vector<char> vector_data(1024);
        const std::vector<char> &const_vector_data = vector_data;
        std::string string_data(1024, ' ');
        const std::string const_string_data(1024, ' ');
        std::vector<io::mutable_buffer> mutable_buffer_sequence;
        std::vector<io::const_buffer> const_buffer_sequence;

        // mutable_buffer constructors.

        io::mutable_buffer mb1;
        assert(mb1.size() == 0);
        io::mutable_buffer mb2(void_ptr_data, 1024);
        assert(mb2.size() == 1024);
        io::mutable_buffer mb3(span_1);
        assert(mb3.size() == 1024);
        io::mutable_buffer mb4(mb1);
        assert(mb4.size() == 0);

        // mutable_buffer functions.

        void *ptr1 = mb1.data();
        (void)ptr1;

        std::size_t n1 = mb1.size();
        (void)n1;

        // mutable_buffer operators.

        mb1 += 128;
        mb1 = mb2 + 128;
        mb1 = 128 + mb2;

        // const_buffer constructors.

        io::const_buffer cb1;
        io::const_buffer cb2(const_void_ptr_data, 1024);
        io::const_buffer cb3(span_1);
        (void)cb3;
        io::const_buffer cb4(span_2);
        (void)cb4;
        io::const_buffer cb5(cb1);
        (void)cb5;
        io::const_buffer cb6(mb1);
        (void)cb6;

        // const_buffer functions.

        const void *ptr2 = cb1.data();
        (void)ptr2;

        std::size_t n2 = cb1.size();
        (void)n2;

        // const_buffer operators.

        cb1 += 128;
        cb1 = cb2 + 128;
        cb1 = 128 + cb2;

        // buffer_size function overloads.
        using namespace mcs::net::io::buffer;
        std::size_t size1 = io::buffer_size(mb1);
        (void)size1;
        std::size_t size2 = io::buffer_size(cb1);
        (void)size2;
        std::size_t size3 = io::buffer_size(mutable_buffer_sequence);
        (void)size3;
        std::size_t size4 = io::buffer_size(const_buffer_sequence);
        (void)size4;

        // buffer function overloads.

        mb1 = buffer(mb2);
        mb1 = buffer(mb2, 128);
        cb1 = buffer(cb2);
        cb1 = buffer(cb2, 128);
        mb1 = buffer(void_ptr_data, 1024);
        cb1 = buffer(const_void_ptr_data, 1024);
        mb1 = buffer(raw_data);
        mb1 = buffer(raw_data, 1024);
        cb1 = buffer(const_raw_data);
        cb1 = buffer(const_raw_data, 1024);
        mb1 = buffer(span_1);

        assert(mb1.size() == 1024);
        mb1 = buffer(span_1, 128);
        assert(mb1.size() == 128);

        cb1 = buffer(span_2);
        cb1 = buffer(span_2, 128);
    }

    TEST("buffer_copy_runtime") = [] {
        char dest_data[256];
        char source_data[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        memset(dest_data, 0, sizeof(dest_data));
        io::mutable_buffer mb1 = io::buffer(dest_data);
        io::mutable_buffer mb2 = io::buffer(source_data);
        std::size_t n = io::buffer_copy(mb1, mb2); // NOTE: m -> m
        assert(n == sizeof(source_data));
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data)); // NOTE: clean dest_data
        mb1 = io::buffer(dest_data);
        io::const_buffer cb1 = io::buffer(source_data);
        n = io::buffer_copy(mb1, cb1); // NOTE: c -> m
        assert(n == sizeof(source_data));
        assert(memcmp(dest_data, source_data, n) == 0);

        // vector
        memset(dest_data, 0, sizeof(dest_data)); // NOTE: clean dest_data
        mb1 = io::buffer(dest_data);
        std::vector<io::mutable_buffer> mv1;
        mv1.push_back(io::buffer(source_data, 5));
        mv1.push_back(io::buffer(source_data) + 5);
        n = io::buffer_copy(mb1, mv1); // NOTE: vec -> m
        assert(n == sizeof(source_data));
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data));
        mb1 = io::buffer(dest_data);
        std::vector<io::const_buffer> cv1;
        cv1.push_back(io::buffer(source_data, 6));
        cv1.push_back(io::buffer(source_data) + 6);
        n = io::buffer_copy(mb1, cv1);
        assert(n == sizeof(source_data));
        assert(memcmp(dest_data, source_data, n) == 0);

        // NOTE: source vec change
        memset(dest_data, 0, sizeof(dest_data));
        mv1.clear();
        mv1.push_back(io::buffer(dest_data, 7));
        mv1.push_back(io::buffer(dest_data) + 7);
        cb1 = io::buffer(source_data);
        n = io::buffer_copy(mv1, cb1);
        assert(n == sizeof(source_data));
        assert(memcmp(dest_data, source_data, n) == 0);

        // NOTE: dest_data change by two vec
        memset(dest_data, 0, sizeof(dest_data));
        mv1.clear();
        mv1.push_back(io::buffer(dest_data, 7));
        mv1.push_back(io::buffer(dest_data) + 7);
        cv1.clear();
        cv1.push_back(io::buffer(source_data, 8));
        cv1.push_back(io::buffer(source_data) + 8);
        n = io::buffer_copy(mv1, cv1);
        assert(n == sizeof(source_data));
        assert(memcmp(dest_data, source_data, n) == 0);

        // NOTE: partial copy
        memset(dest_data, 0, sizeof(dest_data));
        mb1 = io::buffer(dest_data);
        mb2 = io::buffer(source_data);
        n = io::buffer_copy(mb1, mb2, 10);
        assert(n == 10);
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data));
        mb1 = io::buffer(dest_data);
        cb1 = io::buffer(source_data);
        n = buffer_copy(mb1, cb1, 10);
        assert(n == 10);
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data));
        mb1 = io::buffer(dest_data);
        mv1.clear();
        mv1.push_back(io::buffer(source_data, 5));
        mv1.push_back(io::buffer(source_data) + 5);
        n = buffer_copy(mb1, mv1, 10);
        assert(n == 10);
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data));
        mb1 = io::buffer(dest_data);
        cv1.clear();
        cv1.push_back(io::buffer(source_data, 6));
        cv1.push_back(io::buffer(source_data) + 6);
        n = buffer_copy(mb1, cv1, 10);
        assert(n == 10);
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data));
        mv1.clear();
        mv1.push_back(io::buffer(dest_data, 7));
        mv1.push_back(io::buffer(dest_data) + 7);
        cb1 = io::buffer(source_data);
        n = buffer_copy(mv1, cb1, 10);
        assert(n == 10);
        assert(memcmp(dest_data, source_data, n) == 0);

        memset(dest_data, 0, sizeof(dest_data));
        mv1.clear();
        mv1.push_back(io::buffer(dest_data, 7));
        mv1.push_back(io::buffer(dest_data) + 7);
        cv1.clear();
        cv1.push_back(io::buffer(source_data, 8));
        cv1.push_back(io::buffer(source_data) + 8);
        n = buffer_copy(mv1, cv1, 10);
        assert(n == 10);
        assert(memcmp(dest_data, source_data, n) == 0);
    };

    TEST("dynamic_vector_buffer") = [] {
        std::vector<char> vec = {'a', 'b', 'c'};

        using Buffer = io::dynamic_vector_buffer<char, std::allocator<char>>;
        // "Default construction and basic properties"
        {
            Buffer buf(vec);
            assert(buf.max_size() == std::numeric_limits<std::size_t>::max());
            assert(buf.size() == 3);
            assert(buf.capacity() >= 3);
        }
        // Fixed maximum size
        {
            Buffer buf(vec, 5);
            assert(buf.max_size() == 5);
            assert(buf.size() == 3);

            buf.grow(2);
            assert(buf.size() == 5);

            bool has_length_error = false;
            try
            {
                buf.grow(1);
            }
            catch (std::length_error &e)
            {
                has_length_error = true;
            }
            assert(has_length_error);
        }
        // Buffer data access
        {
            Buffer buf(vec);
            auto mb = buf.data(0, 3);
            assert(mb.size() == 3);
            assert(static_cast<char *>(mb.data())[0] == 'a');

            auto cb = static_cast<const Buffer &>(buf).data(1, 2);
            assert(cb.size() == 2);
            assert(static_cast<const char *>(cb.data())[0] == 'b');
        }
        // Growth operations
        {
            // NOTE: dynamic_vector_buffer 引用 vec
            Buffer buf(vec, 10);
            std::cout << "vec.size(): " << vec.size() << '\n';
            assert(buf.size() == vec.size());
            assert(buf.size() == 5);
            buf.grow(3);
            std::cout << "vec.size(): " << vec.size() << '\n';
            assert(buf.size() == vec.size());
            assert(buf.size() == 5 + 3);

            bool has_length_error = false;
            try
            {
                buf.grow(3);
            }
            catch (std::length_error &e)
            {
                has_length_error = true;
            }
            assert(has_length_error);
        }
        // Shrink operations
        {
            Buffer buf(vec);
            assert(buf.size() == 5 + 3);
            assert(vec[0] == 'a');

            buf.shrink(2); // 收缩 2
            assert(buf.size() == 6);
            assert(vec.size() == 6);
            assert(vec[0] == 'a'); // NOTE: 不会  擦除？

            buf.shrink(7); // Shrink more than size
            assert(buf.size() == 0);
            assert(vec.empty());
        }
        // Consume operations
        {
            vec = {'a', 'b', 'c'};
            Buffer buf(vec);
            assert(vec[0] == 'a');

            buf.consume(2);
            assert(buf.size() == 1);
            assert(vec.size() == 1);
            assert(vec[0] == 'c');

            buf.consume(5); // Consume more than size
            assert(buf.size() == 0);
            assert(vec.empty());
        }
        //"Capacity reporting"

        {
            vec.reserve(100);
            Buffer buf(vec, 50);
            assert(buf.capacity() == 50); // Capped by max_size

            Buffer unbuf(vec);
            assert(unbuf.capacity() >= 100);
        }

        TEST("Edge case handling") = [] {
            std::vector<int> empty_vec;

            // Empty vector handling
            {
                auto buf = io::dynamic_buffer(empty_vec, 10);
                assert(buf.size() == 0);
                assert(buf.capacity() == 0);

                auto mb = buf.data(0, 5);
                assert(mb.size() == 0);

                buf.grow(3);
                assert(buf.size() == 3);
                assert(empty_vec.size() == 3);
            }

            // Zero maximum size
            {
                std::vector<int> data{1, 2, 3};
                auto buf = io::dynamic_buffer(data, 0);

                assert(buf.size() == 0);
                assert(buf.capacity() == 0);

                bool has_length_error = false;
                try
                {
                    buf.grow(1);
                }
                catch (std::length_error &e)
                {
                    has_length_error = true;
                }
                assert(has_length_error);

                auto mb = buf.data(0, 10);
                assert(mb.size() == 0);
            }
            // Partial buffer access
            {
                std::vector<char> data{'a', 'b', 'c', 'd'};
                auto buf = io::dynamic_buffer(data, 3); // Max size = 3

                auto chunk = buf.data(1, 5);
                assert(chunk.size() == 2); // Should be capped by (3-1)=2
                assert(static_cast<char *>(chunk.data())[0] == 'b');
            }
        };
    };

    TEST("test_dynamic_vector_buffer") = [] {
        std::cout << "===== dynamic_vector_buffer 核心能力测试 =====" << std::endl;

        // 1. 创建底层存储和缓冲区
        std::vector<char> storage;
        io::dynamic_vector_buffer buf(storage, 1024); // 最大1KB

        std::cout << "初始状态: size=" << buf.size()
                  << ", capacity=" << storage.capacity() << std::endl;

        // 2. 模拟异步写入操作
        {
            // 获取可写区域 (模拟 async_write_some)
            auto writable = buf.data(0, 256); // 请求256字节空间
            std::cout << "可写区域: size=" << writable.size() << std::endl;

            // 模拟数据写入
            char *ptr = static_cast<char *>(writable.data());
            for (size_t i = 0; i < writable.size(); i++)
            {
                ptr[i] = 'A' + (i % 26); // 填充测试数据
            }

            // 提交写入的数据
            buf.consume(writable.size());
            std::cout << "写入后: size=" << buf.size() << ", data[0]=" << storage[0]
                      << std::endl;
        }

        // 3. 同时读写操作
        {
            // 在已有数据的情况下获取新可写区域
            auto writable = buf.data(0, 128);

            // 模拟写入新数据
            char *ptr = static_cast<char *>(writable.data());
            for (size_t i = 0; i < writable.size(); i++)
            {
                ptr[i] = 'a' + (i % 26); // 填充不同的测试数据
            }
            buf.consume(writable.size());

            // 读取缓冲区中的数据 (模拟 async_read_some)
            // auto readable = buf.data();
            // std::cout << "读写后: size=" << buf.size()
            //           << ", data[0]=" << static_cast<char *>(readable.data)[0]
            //           << ", data[256]=" << static_cast<char *>(readable.data)[256]
            //           << std::endl;
        }

        // 4. 消费数据后继续写入
        {
            // 消费部分已处理数据
            buf.consume(200);
            std::cout << "消费后: size=" << buf.size() << ", storage[0]=" << storage[0]
                      << ", storage.front()=" << storage.front() << std::endl;

            // 获取新可写区域
            auto writable = buf.data(0, 200);

            // 模拟写入新数据
            char *ptr = static_cast<char *>(writable.data());
            for (size_t i = 0; i < writable.size(); i++)
            {
                ptr[i] = '0' + (i % 10); // 填充数字
            }
            buf.consume(writable.size());

            // 验证缓冲区状态
            // auto readable = buf.data();
            // std::cout << "最终状态: size=" << buf.size()
            //           << ", data[0]=" << static_cast<char *>(readable.data)[0]
            //           << ", data[100]=" << static_cast<char *>(readable.data)[100]
            //           << std::endl;
        }

        // 5. 测试边界条件
        try
        {
            std::cout << "\n测试边界条件:" << std::endl;

            // 尝试超过最大限制
            auto writable = buf.data(0, 2048); // 请求超过1KB
            std::cout << "超限请求: 实际获取=" << writable.size() << std::endl;

            // 尝试提交过多数据
            buf.consume(writable.size() + 1); // 应该抛出异常
        }
        catch (const std::length_error &e)
        {
            std::cout << "捕获异常: " << e.what() << std::endl;
        }

        // 6. 测试缓冲区自动增长
        {
            std::cout << "\n测试缓冲区增长:" << std::endl;
            std::vector<char> raw;
            io::dynamic_vector_buffer grow_buf(raw);

            // 模拟多次写入
            for (int i = 0; i < 5; i++)
            {
                auto space = grow_buf.data(0, 256);
                grow_buf.consume(space.size());
                std::cout << "写入后: size=" << grow_buf.size()
                          << ", capacity=" << raw.capacity() << std::endl;
            }
        }
    };
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND