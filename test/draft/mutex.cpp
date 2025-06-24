#include <iostream>
#include <mutex>

int main()
{
    std::mutex lock;
    {
        if (lock.try_lock())
        {
            std::cout << " get lock\n";

            if (lock.try_lock())
            {
                std::cout << " try_lock again ok\n";
            }
            else
            {
                std::cout << " try_lock again fail\n";
            }
            lock.unlock();
        }
    }
    std::cout << "main done\n";
    return 0;
}