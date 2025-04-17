#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "watchdog.h"


int main(int argc, char** argv)
{
    int time_to_sleep = 500;

    MakeMeImmortal(argc, argv, 6, 4);
    while (time_to_sleep > 0)
    {
        time_to_sleep = sleep(time_to_sleep);
    }
    
    return (0);
}