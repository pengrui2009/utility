#include <iostream>
#include "hal_timer.h"

std::unique_ptr<hal::HalTimer> m_timer;

int main(int argc, char *argv[])
{
    m_timer = std::make_unique<hal::HalTimer>();
    m_timer->add(
        1000, [&]() { dlt_log_print(); }, 1000);
    m_timer->add(
        1000 * 10, [&]() { run_hogs_cmd(); }, 1000 * 10);
    return 0;
}