#include <iostream>

int main()
{
    // // 无捕获lambda（转换为函数指针）
    // auto lambda_no_capture = []() {
    //     std::cout << "无捕获lambda\n";
    // };
    // void *p_func = reinterpret_cast<void *>(lambda_no_capture); // 实现定义
    // reinterpret_cast<void (*)()>(p_func)();                     //

    // 有捕获lambda（作为对象处理）
    int y = 20;
    auto lambda_capture = [y]() {
        std::cout << "捕获y=" << y << "\n";
    };
    void *p_obj = &lambda_capture;
    (*static_cast<decltype(lambda_capture) *>(p_obj))(); // 安全调用

#if 0
    // 错误示例：类型不匹配
    auto another_lambda = []() {
        std::cout << "另一个lambda\n";
    };
    void *p_wrong = &another_lambda;
    static_cast<decltype(lambda_capture) *>(p_wrong)->operator()(); // 未定义行为
#endif

    {
        int y = 20;
        auto lambda_capture = [y](int a, double b) {
            std::cout << "捕获y=" << y << "a :" << a << ", b: " << b << "\n";
        };
        void *p_obj = &lambda_capture;
        // 又如何知道类型呢？
        (*static_cast<decltype(lambda_capture) *>(p_obj))(0, 1); // 安全调用
    }

    return 0;
}