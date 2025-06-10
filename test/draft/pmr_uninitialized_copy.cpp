#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// NOLINTBEGIN

struct Person
{
    std::string name;
    int age;

    Person(const Person &) = default;
    Person(Person &&) = default;
    Person &operator=(const Person &) = default;
    Person &operator=(Person &&) = default;
    Person(std::string n, int a) : name(std::move(n)), age(a)
    {
        std::cout << "Constructing " << name << "\n";
    }
    ~Person()
    {
        std::cout << "Destructing " << name << "\n";
    }
};

int main()
{
    std::vector<Person> people = {{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}};

    // 分配未初始化的内存
    std::cout << "========allocate start" << "\n";
    auto p = std::allocator<Person>().allocate(people.size());
    std::cout << "========allocate end" << "\n";

    std::cout << "========uninitialized_copy " << "\n";
    // 将已构造对象复制到未初始化内存
    std::uninitialized_copy(people.begin(), people.end(), p);

    // 验证复制结果
    for (size_t i = 0; i < people.size(); ++i)
    {
        std::cout << p[i].name << " " << p[i].age << "\n";
    }

    std::cout << "========destroy_at " << "\n";
    // 手动销毁对象
    for (size_t i = 0; i < people.size(); ++i)
    {
        std::destroy_at(p + i);
    }

    std::cout << "========deallocate start" << "\n";
    // 释放内存
    std::allocator<Person>().deallocate(p, people.size());
    std::cout << "========deallocate end" << "\n";
}
// NOLINTEND