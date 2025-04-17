/**
 * @file watchdog_utils.c
 * @brief Implementation of the internal watchdog logic and process management.
 *
 * This file defines the behavior of the watchdog system, including:
 *  - Creating and monitoring a target process
 *  - Scheduling heartbeat signals
 *  - Reacting to process failure
 *  - Handling POSIX signals for inter-process communication
 */
 
#define _POSIX_C_SOURCE 199506L

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "scheduler.h"
#include "watchdog_utils.h"
#include "uid.h"

static volatile sig_atomic_t g_is_sol_received = 0;

wd_ty* WdCreate(char** args)
{
	wd_ty* wd = NULL;
	
	wd = (wd_ty*) malloc(sizeof(wd_ty));
	if (NULL == wd)
	{
		printf("malloc failed\n");
        /* exit(0); */
	}

	wd->scheduler = SchedCreate();
	if (NULL == wd->scheduler)
	{
		free(wd);
		printf("SchedCreate failed\n");
        /* exit(0); */
	}

	wd->interval = strtoul(args[1], NULL, 10);
	wd->max_fails = strtoul(args[2], NULL, 10);
	wd->fails = 0;
	wd->target_pid = -1;
	wd->target_args = args;
	wd->revive_task = NULL;
		
	return (wd);
}

void WdDestroy(wd_ty* wd)
{
	SchedDestroy(wd->scheduler);
	free(wd);
	g_is_sol_received = 0;
}

int WdAddTask(wd_ty* wd, int (*task)(void *), unsigned long interval)
{
	uid_ty uid = SchedAddTask(wd->scheduler, task, DoNothingTSK, wd, NULL,
                              interval);
	if (UIDIsSame(uid, GetBadUID()) != 0)
	{
		printf("SchedAddTask failed\n");
        /* exit(0); */
	}
	
	return 0;
}

void WdClearTasks(wd_ty* wd)
{
	SchedClear(wd->scheduler);
}

void WdStart(wd_ty* wd)
{
	SchedRun(wd->scheduler);
}

void WdStop(wd_ty* wd)
{
	SchedStop(wd->scheduler);
}

void WdSendSignal(wd_ty* wd, int sig_num)
{
	int status = kill(wd->target_pid, sig_num);

	if (EPERM == status || ESRCH == status)
	{
		printf("kill() failed\n");
        /* exit(0); */
	}
}

void WdExecTarget(wd_ty* wd)
{
	execv(wd->target_args[0], wd->target_args);
	
	printf("execv() failed\n");
}

void WdSpawnTarget(wd_ty* wd)
{
	pid_t pid = 0;
	
	pid = fork();
	
	if (pid < 0)
	{
		printf("fork() failed\n");
		/* exit(0); */
	}
	else if (0 == pid)
	{
		WdExecTarget(wd);
	}
	
	wd->target_pid = pid;
}

int WdWaitPid(wd_ty* wd)
{
	int status = 0;
	
	do
	{
		status = waitpid(wd->target_pid, NULL, 0);
	}
	while (status && EINTR == errno);
	
	return (status ? 0 : 1);
}

int SendSolTSK(void* args)
{
	wd_ty* wd = (wd_ty*) args;

	WdSendSignal(wd, SIGUSR1);

	return 1;
}

int CheckSolTSK(void* args)
{
	wd_ty* wd = (wd_ty*) args;
	
	if (g_is_sol_received == 1)
	{
		wd->fails = 0;
		g_is_sol_received = 0;
	}
	else
	{
		++wd->fails;
	}
	
	return 1;
}

int ReviveIfErrorTSK(void* args)
{
	wd_ty* wd = (wd_ty*) args;

	printf("ps in watchdog_utils.c ReviveIfErrorTSK\n");
    system("ps");

	if (wd->fails == wd->max_fails)
	{
		WdSendSignal(wd, SIGKILL);
		WdClearTasks(wd);
		WdAddTask(wd, wd->revive_task, 1);
	}
	
	return 1;
}

void DoNothingTSK(void* args)
{
	(void)args;
}

void SIGUSR1Handler(int sig_num)
{
	(void) sig_num;
	
	g_is_sol_received = 1;

	if (sig_num == SIGUSR1)
    {
        printf("Received SIGUSR1!\n");
    }
}

int SetSignalMask(int sig_num, int val)
{
	sigset_t sig_set;
	
	if (sigemptyset(&sig_set))
	{
		printf("sigemptyset() failed\n");
		/* exit(0); */
	}
	
	if (sigaddset(&sig_set, sig_num))
	{
		printf("sigaddset() failed\n");
		/* exit(0); */
	}
	
	if (pthread_sigmask(val, &sig_set, NULL))
	{
		printf("pthread_sigmask() failed\n");
		/* exit(0); */
	}
	
	return 0;
}

int SetSignalHandler(int sig_num, void (*handler)(int))
{
	struct sigaction sa;
	
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	
	if (sigemptyset(&sa.sa_mask))
	{
		printf("sigemptyset() failed\n");
		/* exit(0); */
	}
	
	if (sigaction(sig_num, &sa, NULL))
	{
		printf("sigaction() failed\n");
		/* exit(0); */
	}
	
	return 0;
}

int GetSol(void)
{
	return (g_is_sol_received);
}
