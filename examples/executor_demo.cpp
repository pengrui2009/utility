#include <iostream>
#include "executor.h"

#include <unistd.h>

int main(int argc, char *argcv[])
{
    // using namespace utility::executor;
    using TaskExecutor = utility::Executor<std::function<void(void)>>;

    std::unique_ptr<TaskExecutor> task_ptr{nullptr};
    task_ptr = std::make_unique<TaskExecutor>();

    auto sleep_handle = []() {
        static int count = 0;

        while (count++ < 20)
        {
            std::cout << "count:" << count << std::endl;
            sleep(1);
        }

        std::cout << "task exit." << std::endl;
    };

    task_ptr->AddExecute(sleep_handle);

    while (1)
    {
        sleep(1);
    }

    return 0;
}