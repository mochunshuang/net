#include <cassert>

// NOLINTBEGIN

// 模拟基类
struct operation_base
{
    int type;
    struct operation_callback
    {
        void (*complete)(operation_base *);
        void (*complete_error)(operation_base *, int);
    } callback;

    int data = {1};
};

// 模拟派生类
template <typename Operation, int OpType>
struct iocp_operation : operation_base
{
    constexpr explicit iocp_operation()
        : operation_base{OpType, {&iocp_operation::invoke, &iocp_operation::invoke_error}}
    {
    }

    // NOTE: 私有化 不影响。指针变量。绑定了就不会动了
  private:
    constexpr static void invoke(operation_base *self)
    {
        Operation::complete(static_cast<Operation *>(self));
    }

    constexpr static void invoke_error(operation_base *self, int err)
    {
        Operation::complete_error(static_cast<Operation *>(self), err);
    }
};

// 模拟业务上下文
struct MyOperation : iocp_operation<MyOperation, 0>
{

    static constexpr void complete(MyOperation *self) noexcept
    {
        auto *derived = static_cast<MyOperation *>(self);
        derived->handle_complete();
    }
    static constexpr void complete_error(MyOperation *self, int code) noexcept
    {
        auto *derived = static_cast<MyOperation *>(self);
        derived->handle_error(code);
    }

    bool completed = false;
    bool errored = false;
    int erro_code{0};

  private:
    constexpr void handle_complete()
    {
        assert(this->data == 1);
        completed = true;
    }
    constexpr void handle_error(int code)
    {
        assert(this->data == 1);

        errored = true;
        erro_code = code;
    }
};

constexpr auto test()
{
    MyOperation impl;

    operation_base *base = &impl;

    auto callbackimpl = impl.callback;

    callbackimpl.complete(base); // NOTE: 回调调用
    assert(impl.completed && !impl.errored);

    assert(impl.erro_code == 0);
    callbackimpl.complete_error(&impl, 42);
    assert(impl.errored); // NOTE: 回调调用
    assert(impl.erro_code == 42);
    return true;
}

int main()
{
    static_assert(test()); // NOTE: 编译器测试

    {
        // NOTE: 运行时测试
        struct Operation : iocp_operation<Operation, 1>
        {

            static constexpr void complete(Operation *self) noexcept
            {
                auto *derived = static_cast<Operation *>(self);
                derived->handle_complete();
            }
            static constexpr void complete_error(Operation *self, int code) noexcept
            {
                auto *derived = static_cast<Operation *>(self);
                derived->handle_error(code);
            }

            bool completed = false;
            bool errored = false;
            int erro_code{0};

          private:
            constexpr void handle_complete()
            {
                assert(this->data == 1);
                completed = true;
            }
            constexpr void handle_error(int code)
            {
                assert(this->data == 1);

                errored = true;
                erro_code = code;
            }
        };

        {
            Operation impl;

            operation_base *base = &impl;

            auto callbackimpl = impl.callback;

            callbackimpl.complete(base); // NOTE: 回调调用
            assert(impl.completed && !impl.errored);

            assert(impl.erro_code == 0);
            callbackimpl.complete_error(&impl, 100);
            assert(impl.errored); // NOTE: 回调调用
            assert(impl.erro_code == 100);
        }
    }
}
// NOLINTEND