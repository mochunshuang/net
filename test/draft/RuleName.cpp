#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>
#include <cctype>
#include <vector>

using std::string;

// 示例规则实现
class RuleNameInsensitive
{
    string name_;

  public:
    constexpr explicit RuleNameInsensitive(std::string_view s) noexcept : name_(s) {}
    [[nodiscard]] constexpr std::string_view getName() const noexcept
    {
        return name_;
    }
};
class RuleNameSensitive
{
    string name_;

  public:
    constexpr explicit RuleNameSensitive(std::string_view s) noexcept : name_(s) {}
    [[nodiscard]] constexpr std::string_view getName() const noexcept
    {
        return name_;
    }
};

class Rule
{
    RuleNameInsensitive name_;

  public:
    constexpr explicit Rule(std::string_view s) noexcept : name_(s) {}
    constexpr const RuleNameInsensitive &name() noexcept
    {
        return name_;
    }
};

class RuleRegistry
{
  private:
    // 核心存储结构：全大写名称 -> 规则对象
    std::unordered_map<std::string, Rule *> registry_;

    // 名称规范化（核心逻辑）
    constexpr static std::string normalize(const std::string_view &name_) noexcept
    {
        std::string name(name_);
        // 1. 去除首尾尖括号
        if (!name.empty() && name.front() == '<')
            name.erase(0, 1);
        if (!name.empty() && name.back() == '>')
            name.pop_back();
        // 2. 转换全大写
        for (auto &c : name)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        return name;
    }

  public:
    // 注册接口
    void add_rule(Rule *rule)
    {
        registry_[normalize(rule->name().getName())] = rule;
    }

    // 查询接口
    Rule *get_rule(const std::string &name) const
    {
        if (auto it = registry_.find(normalize(name)); it != registry_.end())
            return it->second;
        return nullptr;
    }
};

int main()
{
    // 使用示例
    RuleRegistry registry;
    {

        // constexpr std::string s = "Hello";        // 编译期字符串 不可以
        // constexpr std::vector<int> v = {1, 2, 3}; // 编译期向量（但操作受限）

        // constexpr std::string s(std::string("aaa")); // 不可以可以的
    }

    // 注册默认实现
    // constexpr Rule default_rule("RuleNameInsensitive");
    // registry.add_rule(&default_rule);

    // // 所有查询将指向同一个对象
    // Rule *r1 = registry.get_rule("rulenameinsensitive");   // OK
    // Rule *r2 = registry.get_rule("<RULENAMEINSENSITIVE>"); // OK
    // Rule *r3 = registry.get_rule("rUlEnAmEiNsEnSiTiVe");   // OK

    // // 三个指针将指向同一个规则对象
    // assert(r1 == r2 && r2 == r3);
    return 0;
}