#include "task.h"

int main(int argc, char *argv[])
{
    utility::Task t;
    int count = 0;

    while (1)
    {
        // std::cout << "count:" << count << std::endl;
        if (count < 10)
        {
            // do nothing
            t.Start();
        }
        else if ((count >= 10) && (count <= 20))
        {
            t.Suspend();
        }
        else if ((count > 20) && (count < 30))
        {
            t.Resume();
        }
        else if (count > 30)
        {
            t.Cancel();
        }
        count++;
        usleep(500000);
    }
    return 0;
}
