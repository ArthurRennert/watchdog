# 🛡️ Watchdog – Fault-Tolerant Process Monitoring in Pure C

> Make your process immortal: If it crashes, it revives.
> Lightweight. Pure C. No dependencies.

------------------------------------------------------------

## 📖 Introduction

This project implements a complete watchdog system in C89 that allows **two independent processes to monitor and revive each other** using only POSIX tools (`fork`, `execv`, `kill`, `signal`, `sleep`).  

When a process dies (by crash or `SIGKILL`), the other one notices and **revives it instantly**.  
This setup ensures high availability, resilience, and complete autonomy — without external services like `systemd`, Docker, or supervisor.

------------------------------------------------------------

## 🎯 Project Goals

- ✅ Keep a process alive **no matter what**
- ✅ No dependencies — pure C
- ✅ Fully modular — pluggable scheduler, UID, task system
- ✅ Educational — great for learning low-level process control

------------------------------------------------------------

## 🔧 How It Works (in short)

- `client_test` runs and calls `MakeMeImmortal()`
- This spawns a `watchdog_exec` process using `fork()` + `execv()`
- Both processes:
  - Send heartbeats via `SIGUSR1`
  - Expect a reply within `interval` seconds
  - If no reply received `max_fails` times → restart peer

------------------------------------------------------------

## 🧪 Getting Started

### ✅ Compilation
```c
gcc src/client_test.c libwatchdog.a -I include/ -o client_test
```
------------------------------------------------------------

### ▶️ Run
```c
./client_test
```
Now try killing it from another terminal:
```c
kill -9 <PID_OF_CLIENT>
```
Within a few seconds, it comes back to life — revived by the watchdog.

------------------------------------------------------------

🔬 API Overview
```c
int MakeMeImmortal(int argc, char* argv[], unsigned long interval, int max_fails);
```
Initializes a watchdog thread and launches a separate watchdog process that monitors this one.

interval: Time in seconds between checks (e.g., 6)

max_fails: How many missed checks to tolerate before restarting (e.g., 4)

------------------------------------------------------------

🧪 Example

```c
int main(int argc, char** argv)
{
    MakeMeImmortal(argc, argv, 6, 4); // Every 6 seconds, allow 4 missed heartbeats

    /* Simulate normal program loop */
    while (time_to_sleep > 0)
    {
        time_to_sleep = sleep(time_to_sleep);
    }

    return 0;
}
```
------------------------------------------------------------

✋ DoNotResuscitate()

Call this when your program exits intentionally.
It prevents the watchdog from reviving it again.

🧪 Example
```c
DoNotResuscitate();
return 0;
```
------------------------------------------------------------

🧱 Project Structure:

.
├── src/
│   ├── client_test.c         # Sample monitored process
│   ├── watchdog.c            # Core watchdog interface
│   ├── watchdog_exec.c       # Standalone watchdog process
│   ├── watchdog_utils.c      # Heartbeat, spawn, revive logic
│   ├── scheduler.c           # Periodic task manager
│   ├── uid.c                 # UID system for task identity
│   ├── sorted_list.c         # Sorted list implementation
│   ├── doubly_linked_list.c  # Doubly linked list module
│   ├── priority_queue.c      # Priority-based queue
│   ├── task.c                # Task wrapper
│
├── include/                  # All header files
│   └── watchdog.h, scheduler.h, uid.h, etc.
│
├── lib/
│   └── libwatchdog.a         # Static library built from above
│
├── Makefile
└── README.md

------------------------------------------------------------

🧠 Modules Explained

```markdown
| **Module**            | **Description**                                         |
|-----------------------|---------------------------------------------------------|
| `watchdog.c`          | Interface for `MakeMeImmortal()` and thread setup       |
| `watchdog_exec.c`     | Executed process that watches the parent process        |
| `watchdog_utils.c`    | Heartbeat logic, task scheduling, process control       |
| `scheduler.c`         | Generic recurring task manager (with intervals)         |
| `uid.c`               | Generates unique task IDs                               |
| `sorted_list.c`       | Sorted data structure used by other modules             |
| `doubly_linked_list.c`| Base data structure for queues and task lists           |
| `priority_queue.c`    | Priority-based wrapper for task execution               |
| `task.c`              | Encapsulates a task: function, args, timing             |

```

------------------------------------------------------------

You can link it to your own projects using:
```c
gcc your_file.c libwatchdog.a -I include/
```

------------------------------------------------------------

⚙️ How the Scheduler Works
```c
SchedAddTask(scheduler, SendSolTSK, DoNothingTSK, wd_ptr, NULL, 6);
```
Each task:

Has an action (what to do)

Has an interval (how often to run)

Can be added/removed dynamically

------------------------------------------------------------

🔁 Communication Flow
```text
client_test             watchdog_exec
     |                        |
     | --> fork + execv() --> |
     |                        |
     | <---- SIGUSR1 -------  |   (heartbeat ping)
     | ---- SIGUSR1 ------->  |   (pong)
     |                        |
     | missed 4 heartbeats →  |
     | <----- SIGKILL ------- |
     | <----- execv() ------- |  (restart)

```
------------------------------------------------------------

📄 Example Log Output

ps in watchdog.c MMI
PID   TTY   ...
1234  pts/1 ./client_test
1235  pts/1 ./watchdog_exec
Received SIGUSR1!
ps in watchdog_utils.c ReviveIfErrorTSK
1234  pts/1 ./client_test
1235  pts/1 ./watchdog_exec

------------------------------------------------------------

🧪 Test it Yourself

# Run the monitored process
./client_test

# Get its PID
ps | grep client_test

# Kill it
kill -9 <pid>

# Wait... it should come back in a few seconds!

------------------------------------------------------------

⭐ Final Words
This project is:

Educational ✅

Production-ready ✅

Fully in C ✅

Yours to explore ✅

Want to make sure your process stays alive forever?
This is your tool. 🔄🧬

------------------------------------------------------------

## 📄 License

This project was created by **Arthur Rennert**.
You are free to **use, copy, modify, distribute, and share** this code for any purpose — personal, academic, or commercial.
No permission is needed. Just give credit if you can 😊

