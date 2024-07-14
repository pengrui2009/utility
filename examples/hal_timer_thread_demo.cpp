#include "hal_timer_thread.h"
#include <iostream>
#include <unistd.h>
int main(int argc, char *argv[])
{
    std::shared_ptr<hal::HalTimerThread> timer_ptr = std::make_shared<hal::HalTimerThread>();
    timer_ptr->setTimeSpace(2000);
    timer_ptr->setCallFun([]() {
        std::cout << "timer callback" << std::endl;
    });
    // timer_ptr->setLoop(true);
    timer_ptr->start();

    // while (1)
    // {
    //     sleep(1);
    // }
    sleep(1);
    // timer_ptr->stop();

    timer_ptr->start();

    // while (1)
    // {
    //     sleep(1);
    // }
    sleep(10);
    timer_ptr->stop();
    return 0;
}