/**
 * @file client_test.c
 * @brief Example client program that uses the Watchdog system to become "immortal".
 *
 * This program demonstrates how to integrate with the Watchdog mechanism.
 * It calls `MakeMeImmortal`, which initializes the mutual monitoring process,
 * and then enters a sleep loop. If this process crashes or is terminated,
 * the Watchdog will automatically restart it.
 *
 * @dependencies
 *  - watchdog.h
 * 
 * @usage
 *  Compile with the watchdog library:
 *      gcc client_test.c -Llib -lwatchdog -o client_test
 *
 *  Run:
 *      ./client_test
 * 
 * @note
 *  The process sleeps for 500 seconds. During this time, the watchdog will
 *  ensure that the process remains running.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "watchdog.h"

/**
 * @brief Main function to demonstrate watchdog integration.
 *
 * Calls MakeMeImmortal to start the watchdog mechanism, and then
 * enters a sleep loop for 500 seconds.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument values.
 *
 * @return int Exit status code.
 */
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

