/**
 * @file scheduler.h
 * @brief Public interface for a task scheduler.
 *
 * The scheduler allows registration of recurring tasks to be executed
 * at specified intervals. Each task includes an action function and an
 * optional cleanup function. Tasks can be added, removed, cleared,
 * or executed in a loop.
 *
 * Each task is identified by a unique ID (`uid_ty`), and tasks can be
 * dynamically managed at runtime.
 *
 * The scheduler is used internally by the Watchdog system to manage
 * heartbeat checks and process recovery.
 */

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "uid.h"  /* using uid_ty and function declarations */

/**
 * @typedef scheduler_ty
 * @brief Opaque type for the scheduler instance.
 */
typedef struct scheduler scheduler_ty;

/**
 * @brief Creates a new scheduler instance.
 *
 * @return Pointer to the new scheduler, or NULL on failure.
 */
scheduler_ty* SchedCreate(void);

/**
 * @brief Destroys the scheduler and frees all associated resources.
 *
 * @param sch Pointer to scheduler instance.
 */
void SchedDestroy(scheduler_ty* sch);

/**
 * @brief Adds a task to the scheduler.
 *
 * @param sch Scheduler instance.
 * @param action_func Function to run periodically.
 * @param cleanup_func Function to run on task removal or failure.
 * @param action_params Parameters to pass to the action function.
 * @param cleanup_params Parameters to pass to the cleanup function.
 * @param interval Interval in seconds between task executions.
 *
 * @return Unique ID of the task, or bad UID on failure.
 */
uid_ty SchedAddTask(scheduler_ty* sch, int (*action_func)(void*),
                    void (*cleanup_func)(void*), void* action_params,
                    void* cleanup_params, unsigned long interval);

/**
 * @brief Removes a task from the scheduler by UID.
 *
 * @param sch Scheduler instance.
 * @param uid Task ID to remove.
 */
void SchedRemoveTask(scheduler_ty* sch, uid_ty uid);

/**
 * @brief Returns the number of scheduled tasks.
 *
 * @param sch Scheduler instance.
 * @return Number of tasks currently in the scheduler.
 */
size_t SchedGetSize(const scheduler_ty* sch);

/**
 * @brief Starts the scheduler loop and executes tasks.
 *
 * Blocks until stopped using `SchedStop`.
 *
 * @param sch Scheduler instance.
 * @return 0 on normal exit, non-zero on error.
 */
int SchedRun(scheduler_ty* sch);

/**
 * @brief Requests the scheduler to stop running.
 *
 * Stops the scheduler loop cleanly.
 *
 * @param sch Scheduler instance.
 */
void SchedStop(scheduler_ty* sch);

/**
 * @brief Removes all tasks from the scheduler.
 *
 * Can be used to reset the scheduler state.
 *
 * @param sch Scheduler instance.
 */
void SchedClear(scheduler_ty* sch);

/**
 * @brief Checks if the scheduler has no tasks.
 *
 * @param sch Scheduler instance.
 * @return 1 if empty, 0 if not.
 */
int SchedIsEmpty(const scheduler_ty* sch);

#endif  /* __SCHEDULER_H__ */

