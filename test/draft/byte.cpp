#include <cstddef>
#include <cstdint>
#include <iostream>

int main()
{
    std::cout << "int * case: " << '\n';
    {
        // 指针的算术运算是以指向类型的大小为步长的。例如：
        // int* p; p += n; 会移动 n * sizeof(int) 字节。
        int n = 0;
        int *p = &n;
        p += n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';

        int *old_p = p;
        n = 1;
        p += n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';

        std::cout << "sizeof(int) == 4: " << (sizeof(int) == 4)
                  << ", (p - old_p) == 4 : " << ((p - old_p) == sizeof(int))
                  << ", (p - old_p) == n : " << ((p - old_p) == n) << '\n';

        ptrdiff_t bytes_diff = // NOLINTNEXTLINE
            reinterpret_cast<std::byte *>(p) - reinterpret_cast<std::byte *>(old_p);
        std::cout << "int * case: 字节差: " << bytes_diff << "\n";
        std::cout << "(bytes_diff==sizeof(int)): " << (bytes_diff == sizeof(int)) << "\n";
    }
    std::cout << "\nchar * case: " << '\n';
    {
        int n = 0;
        void *p = &n;
        p = static_cast<char *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';
        void *old_p = p;
        n = 1;
        p = static_cast<char *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';

        ptrdiff_t bytes_diff = // NOLINTNEXTLINE
            reinterpret_cast<std::byte *>(p) - reinterpret_cast<std::byte *>(old_p);
        std::cout << "double * case: 字节差: " << bytes_diff << "\n";
        std::cout << "(bytes_diff==sizeof(char)): " << (bytes_diff == sizeof(char))
                  << "\n";
    }
    std::cout << "\nuint8_t * case: " << '\n';
    {
        int n = 0;
        void *p = &n;
        p = static_cast<uint8_t *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';
        void *old_p = p;
        n = 1;
        p = static_cast<uint8_t *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';
        ptrdiff_t bytes_diff = // NOLINTNEXTLINE
            reinterpret_cast<std::byte *>(p) - reinterpret_cast<std::byte *>(old_p);
        std::cout << "double * case: 字节差: " << bytes_diff << "\n";
        std::cout << "(bytes_diff==sizeof(uint8_t)): " << (bytes_diff == sizeof(uint8_t))
                  << "\n";
    }
    std::cout << "\nbyte * case: " << '\n';
    {
        int n = 0;
        void *p = &n;
        p = static_cast<std::byte *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';
        void *old_p = p;
        n = 1;
        p = static_cast<std::byte *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';

        ptrdiff_t bytes_diff = // NOLINTNEXTLINE
            reinterpret_cast<std::byte *>(p) - reinterpret_cast<std::byte *>(old_p);
        std::cout << "double * case: 字节差: " << bytes_diff << "\n";
        std::cout << "(bytes_diff==sizeof(std::byte)): "
                  << (bytes_diff == sizeof(std::byte)) << "\n";
    }
    std::cout << "\ndouble * case: " << '\n';
    {
        int n = 0;
        void *p = &n;
        p = static_cast<double *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';

        void *old_p = p;
        n = 1;
        p = static_cast<double *>(p) + n;
        std::cout << "n: " << n << ", &n: " << &n << ", p: " << p
                  << ", &n==p: " << (&n == p) << '\n';

        ptrdiff_t bytes_diff = // NOLINTNEXTLINE
            reinterpret_cast<std::byte *>(p) - reinterpret_cast<std::byte *>(old_p);
        std::cout << "double * case: 字节差: " << bytes_diff << "\n";
        std::cout << "(bytes_diff==sizeof(double)): " << (bytes_diff == sizeof(double))
                  << "\n";
    }
    {
        // NOTE: 如果类型相同，两个指针加减法的值和 数学运算相同。 但字节偏移量 可能不同
        static_assert(sizeof(char) == sizeof(uint8_t));
        static_assert(sizeof(char) == sizeof(std::byte));
    }
    std::cout << "main done\n";
    return 0;
}