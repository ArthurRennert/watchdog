/**
 * @file uid.h
 * @brief Unique Identifier (UID) module.
 *
 * Provides functionality for generating, comparing, and identifying
 * unique task identifiers (`uid_ty`) based on timestamp, process ID,
 * counter, and IP address. Used primarily by the scheduler to uniquely
 * identify scheduled tasks.
 */

#ifndef __UID_H__
#define __UID_H__

#include <time.h>            /* using time_t */
#include <sys/types.h>       /* using pid_t */
#include <netinet/in.h>      /* using INET6_ADDRSTRLEN */

/**
 * @struct uid
 * @brief Represents a unique identifier for a task or resource.
 *
 * A UID is composed of:
 *  - A monotonic counter (per process)
 *  - Timestamp of creation
 *  - Process ID (PID)
 *  - Local IP address (as string)
 */
typedef struct uid
{
	size_t  counter;                    /**< Monotonic counter */
	time_t  timestamp;                  /**< Time of creation */
	pid_t   pid;                        /**< Process ID that created the UID */
	char    ip[INET6_ADDRSTRLEN];       /**< String representation of local IP */
} uid_ty;

/**
 * @brief Creates a new unique identifier.
 *
 * Combines current time, process ID, counter, and IP address.
 *
 * @return A new `uid_ty` instance.
 */
uid_ty UIDCreate(void);

/**
 * @brief Compares two UIDs for equality.
 *
 * Returns 1 if the two UIDs are equal, 0 otherwise.
 *
 * @param uid_1 First UID.
 * @param uid_2 Second UID.
 * @return 1 if identical, 0 otherwise.
 */
int UIDIsSame(uid_ty uid_1, uid_ty uid_2);

/**
 * @brief Returns a special "invalid" UID used to signal errors.
 *
 * Can be compared with other UIDs using `UIDIsSame`.
 *
 * @return A `uid_ty` representing an invalid UID.
 */
uid_ty GetBadUID(void);

#endif  /* __UID_H__ */

