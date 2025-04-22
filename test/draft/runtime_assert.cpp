#include <exception>
#include <source_location>
#include <string>
#include <stdexcept>

// NOLINTNEXTLINE
constexpr void runtime_assert(
    bool condition, const char *condition_str = "",
    std::source_location location = std::source_location::current())
{
    if (!condition)
    {
        throw std::runtime_error("Assertion failed: " + std::string(condition_str) +
                                 "\n" + "File: " + location.file_name() + "\n" +
                                 "Line: " + std::to_string(location.line()) + "\n" +
                                 "Function: " + location.function_name());
    }
}

class Obj
{
  public:
    explicit Obj(int v) noexcept
    {
        runtime_assert(v > 0);
    }
};

#include <iostream>

int main()
{
    Obj a(-1);
    std::cout << "main done\n";
    return 0;
}