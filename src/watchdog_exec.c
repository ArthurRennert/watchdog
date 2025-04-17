#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>  /* for malloc */
#include <string.h>  /* for strdup */
#include <assert.h>  /* for assert */
#include <signal.h>  /* using SIGKILL */

#include "scheduler.h"
#include "watchdog_utils.h"

int ExecTargetTSK(void* args);

int main(int argc, char* argv[])
{
	wd_ty* wd = NULL;
	(void) argc;

	SetSignalHandler(SIGUSR1, SIGUSR1Handler);

	wd = WdCreate(argv);
	wd->target_pid = getppid();
	wd->target_args = &wd->target_args[3];
	wd->revive_task = ExecTargetTSK;
	WdAddTask(wd, SendSolTSK, 6);
	WdAddTask(wd, CheckSolTSK, 4);
	WdAddTask(wd, ReviveIfErrorTSK, 10);
	WdStart(wd);

	return (0);
}

int ExecTargetTSK(void* args)
{
	wd_ty* wd = (wd_ty*) args;
	
	WdExecTarget(wd);
	
	return (0);
}
