/**
 * @brief Creates and initializes a new watchdog context.
 *
 * Parses the arguments passed to the watchdog, extracts the monitoring
 * interval and failure limit, and allocates memory for the internal structure.
 *
 * @param args Argument array: [path, interval, max_fails, program args...]
 * @return Pointer to a new `wd_ty` structure, or NULL on failure.
 */
wd_ty* WdCreate(char** args);

/**
 * @brief Frees all resources associated with the watchdog.
 *
 * Destroys the internal scheduler and releases memory allocated for the watchdog.
 *
 * @param wd Pointer to the watchdog instance to destroy.
 */
void WdDestroy(wd_ty* wd);

/**
 * @brief Adds a task to the watchdog's internal scheduler.
 *
 * The task will be executed periodically at the given interval.
 *
 * @param wd Pointer to the watchdog instance.
 * @param task Function pointer to the task to run.
 * @param interval Time interval in seconds between executions.
 * @return 0 on success, non-zero on failure.
 */
int WdAddTask(wd_ty* wd, int (*task)(void *), unsigned long interval);

/**
 * @brief Clears all scheduled tasks from the watchdog's scheduler.
 *
 * This is usually called before shutting down the scheduler or restarting tasks.
 *
 * @param wd Pointer to the watchdog instance.
 */
void WdClearTasks(wd_ty* wd);

/**
 * @brief Starts the watchdog's internal scheduler loop.
 *
 * Begins execution of all added tasks based on their configured intervals.
 *
 * @param wd Pointer to the watchdog instance.
 */
void WdStart(wd_ty* wd);

/**
 * @brief Stops the watchdog's scheduler.
 *
 * Can be called to terminate the loop from outside (e.g., on shutdown).
 *
 * @param wd Pointer to the watchdog instance.
 */
void WdStop(wd_ty* wd);

/**
 * @brief Sends a signal to the monitored target process.
 *
 * Used to either test the process' responsiveness or terminate it.
 *
 * @param wd Pointer to the watchdog instance.
 * @param sig_num Signal number to send (e.g., SIGKILL, SIGUSR1).
 */
void WdSendSignal(wd_ty* wd, int sig_num);

/**
 * @brief Executes the target program (replaces current process).
 *
 * Uses `execv()` to replace the child process with the original client program.
 *
 * @param wd Pointer to the watchdog instance.
 */
void WdExecTarget(wd_ty* wd);

/**
 * @brief Spawns a new child process to run the monitored target.
 *
 * Uses `fork()` to create a new process, which immediately calls `WdExecTarget()`.
 * The PID of the child is stored in the watchdog context.
 *
 * @param wd Pointer to the watchdog instance.
 */
void WdSpawnTarget(wd_ty* wd);

/**
 * @brief Waits for the target process to terminate.
 *
 * Blocks until the monitored process exits, unless interrupted by a signal.
 *
 * @param wd Pointer to the watchdog instance.
 * @return 0 on success, 1 on failure or interruption.
 */
int WdWaitPid(wd_ty* wd);

/**
 * @brief Watchdog task: sends a heartbeat signal to the target.
 *
 * Sends `SIGUSR1` to the monitored process as a "ping" to verify it's alive.
 *
 * @param args Pointer to `wd_ty` structure.
 * @return Always returns 1 (continue).
 */
int SendSolTSK(void* args);

/**
 * @brief Watchdog task: checks for heartbeat response.
 *
 * Verifies if the target responded with `SIGUSR1`. If not, increments
 * the internal failure counter.
 *
 * @param args Pointer to `wd_ty` structure.
 * @return Always returns 1 (continue).
 */
int CheckSolTSK(void* args);

/**
 * @brief Watchdog task: revives the process if failure limit was reached.
 *
 * If the failure count equals `max_fails`, terminates the target,
 * clears all tasks, and schedules the revive task again.
 *
 * @param args Pointer to `wd_ty` structure.
 * @return Always returns 1 (continue).
 */
int ReviveIfErrorTSK(void* args);

/**
 * @brief No-op task function (used for scheduler fallback).
 *
 * @param args Ignored.
 */
void DoNothingTSK(void* args);

/**
 * @brief Sets the signal mask for the current thread.
 *
 * Allows blocking or unblocking a specific signal using `pthread_sigmask`.
 *
 * @param sig_num Signal number to modify.
 * @param val Either SIG_BLOCK or SIG_UNBLOCK.
 * @return 0 on success, non-zero on failure.
 */
int SetSignalMask(int sig_num, int val);

/**
 * @brief Registers a signal handler for a given signal.
 *
 * Sets a custom handler using `sigaction`.
 *
 * @param sig_num Signal number to handle.
 * @param handler Function to be called on signal reception.
 * @return 0 on success, non-zero on failure.
 */
int SetSignalHandler(int sig_num, void (*handler)(int));

/**
 * @brief Signal handler for SIGUSR1.
 *
 * Sets the internal heartbeat received flag to indicate the target is alive.
 *
 * @param sig_num Signal number (expected to be SIGUSR1).
 */
void SIGUSR1Handler(int sig_num);

/**
 * @brief Returns whether a heartbeat signal was received.
 *
 * @return 1 if SIGUSR1 was received since last check, 0 otherwise.
 */
int GetSol(void);

#endif /* __WATCHDOG_UTILS_H__ */

