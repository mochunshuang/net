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
};

// 模拟派生类
template <typename Context, int OpType>
struct iocp_operation : operation_base
{
    Context &ctx;

    constexpr explicit iocp_operation(Context &ctx)
        : operation_base{OpType,
                         {&iocp_operation::invoke, &iocp_operation::invoke_error}},
          ctx(ctx)
    {
    }

    // NOTE: 私有化 不影响。指针变量。绑定了就不会动了
  private:
    constexpr static void invoke(operation_base *self)
    {
        auto *derived = static_cast<iocp_operation *>(self);
        derived->ctx.handle_complete();
    }

    constexpr static void invoke_error(operation_base *self, int err)
    {
        auto *derived = static_cast<iocp_operation *>(self);
        derived->ctx.handle_error(err);
    }
};

// 模拟业务上下文
struct MyOperation
{
    constexpr void handle_complete()
    {
        completed = true;
    }
    constexpr void handle_error(int)
    {
        errored = true;
    }
    bool completed = false;
    bool errored = false;
};

constexpr bool test_completed(operation_base *op)
{
    op->callback.complete(op);
    return true;
}

constexpr bool test_complete_error(operation_base *op, int code)
{
    op->callback.complete_error(op, code);
    return true;
}

constexpr auto test()
{
    MyOperation ctx;
    iocp_operation<MyOperation, 1> op(ctx);

    // 触发回调
    test_completed(&op);
    assert(ctx.completed && !ctx.errored);

    test_complete_error(&op, 42);
    assert(ctx.errored);
    return true;
}

int main()
{
    static_assert(test());
}
// NOLINTEND