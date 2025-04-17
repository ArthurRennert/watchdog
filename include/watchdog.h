/**
 * @file watchdog.h
 * @brief Public API for the Watchdog system.
 *
 * This header provides the interface for initializing and controlling
 * the Watchdog mechanism from a client application.
 *
 * Usage:
 *  Include this header in your main application and call
 *  `MakeMeImmortal` to enable watchdog supervision.
 *
 *  Use `DoNotResuscitate` before terminating the process to avoid being
 *  automatically restarted by the watchdog.
 */

#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

/**
 * @brief Initializes the watchdog mechanism for the current process.
 *
 * This function should be called at the beginning of your program to start
 * the watchdog thread. The watchdog will monitor the process and restart it
 * if it crashes or is killed unexpectedly.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument array.
 * @param interval Time interval in seconds between heartbeat signals.
 * @param max_fails Maximum allowed missed heartbeats before recovery.
 *
 * @return 0 on success, non-zero on failure.
 */
int MakeMeImmortal(int argc, char* argv[], const unsigned long interval,
                   const int max_fails);

/**
 * @brief Requests to stop the watchdog from reviving the process.
 *
 * Call this function before exiting if you want to shut down gracefully
 * and avoid being restarted by the watchdog.
 *
 * @return Always returns 0.
 */
int DoNotResuscitate(void);

#endif  /* __WATCHDOG_H__ */

