#include <cstddef> // size_t
#include <memory>  // unique_ptr
#include <cstring> // memcpy
#include <stdexcept>

// NOLINTBEGIN

class Buffer
{
  public:
    // 动态分配内存（构造函数）
    explicit Buffer(size_t size) : data_(new char[size]), size_(size), is_owner_(true) {}

    // 引用外部内存（构造函数）
    Buffer(char *external_data, size_t size, bool take_ownership = false)
        : data_(take_ownership ? external_data : nullptr), size_(size),
          is_owner_(take_ownership)
    {
        if (!take_ownership)
        {
            // 浅拷贝（仅引用外部内存）
            data_ = external_data;
        }
    }

    // 禁止拷贝（避免双重释放）
    Buffer(const Buffer &) = delete;
    Buffer &operator=(const Buffer &) = delete;

    // 移动语义
    Buffer(Buffer &&other) noexcept
        : data_(other.data_), size_(other.size_), is_owner_(other.is_owner_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.is_owner_ = false;
    }

    Buffer &operator=(Buffer &&other) noexcept
    {
        if (this != &other)
        {
            release(); // 释放当前资源
            data_ = other.data_;
            size_ = other.size_;
            is_owner_ = other.is_owner_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.is_owner_ = false;
        }
        return *this;
    }

    // 析构函数
    ~Buffer()
    {
        release();
    }

    // 访问数据
    char *data()
    {
        return data_;
    }
    const char *data() const
    {
        return data_;
    }
    size_t size() const
    {
        return size_;
    }

    // 深拷贝（可选）
    Buffer clone() const
    {
        Buffer new_buf(size_);
        std::memcpy(new_buf.data_, data_, size_);
        return new_buf;
    }

  private:
    void release()
    {
        if (is_owner_ && data_)
        {
            delete[] data_;
        }
        data_ = nullptr;
        size_ = 0;
    }

    char *data_ = nullptr;
    size_t size_ = 0;
    bool is_owner_ = false; // 是否拥有内存所有权
};

#include <iostream>

int main()
{
    {
        Buffer dynamic_buf(1024); // 分配 1024 字节的堆内存
        std::strcpy(dynamic_buf.data(), "Hello");
    }

    {
        char static_data[] = "Static Data";
        Buffer static_buf(static_data, sizeof(static_data)); // 仅引用
    }
    {
        char *external_data = new char[100];
        Buffer owned_buf(external_data, 100, true); // 接管所有权
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND