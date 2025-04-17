#define _POSIX_C_SOURCE 200809L

#include <stddef.h>     /* using size_t                 */
#include <stdlib.h>     /* using malloc                 */
#include <stdio.h>      /* using sprintf                */
#include <string.h>     /* using memcpy, strlen         */
#include <unistd.h>     /* using fork                   */
#include <sys/types.h>  /* using pid_t                  */
#include <sys/wait.h>   /* using wait                   */
#include <pthread.h>    /* pthread_create, pthread_t    */

#include "watchdog.h"
#include "watchdog_utils.h"
#include "utils.h"

#define WD_PATH "./watchdog_exec"

static void            AssignIntToString    (char* dest_str, int num);
static char*           CreateString         (size_t n);
static char**          CreateStrings        (size_t n);
static void            HandleWatchdogSignal (int signal);
static char**          CreateWdArgs         (unsigned int interval,
                                            unsigned int max_fails,
                                            int argc,
                                            char* args[]);
void*                  WdThread             (void* args);
static void            DestroyWdArgs        (char** wd_args);
void                   SIGUSR2Handler       (int sig_num);
                                             
static volatile int          g_is_dnr_req  = 0;
static          pthread_t    g_wd_thread;


int MakeMeImmortal(int argc, char* argv[], const unsigned long interval,
                   const int max_fails)
{
    char** wd_args = CreateWdArgs(interval, max_fails, argc, argv);
    printf("ps in watchdog.c MMI\n");
    system("ps");
	
	if (pthread_create(&g_wd_thread, NULL, WdThread, wd_args))
	{
		DestroyWdArgs(wd_args);
		perror("Failed to create scheduler thread");
        /* exit(0); */
	}

    return (0);
}


static int TerminateIfDNRTSK(void* args)
{
	wd_ty* wd = (wd_ty*) args;
	
	if (g_is_dnr_req)
	{	
		WdSendSignal(wd, SIGKILL);
		WdWaitPid(wd);
		WdStop(wd);
		g_is_dnr_req = 0;
		
		return (0);
	}
	return (1);
}

static int SpawnTargetTSK(void* args)
{
	wd_ty* wd = (wd_ty*) args;
	
	printf("ps in watchdog.c SpawnTargetTSK\n");
    system("ps");

	WdSpawnTarget(wd);
	WdAddTask(wd, TerminateIfDNRTSK, 1);
	WdAddTask(wd, SendSolTSK, 10);
	WdAddTask(wd, CheckSolTSK, 5);
	WdAddTask(wd, ReviveIfErrorTSK, 5);
		
	return (0);
}

void* WdThread(void* args)
{
	wd_ty* wd = NULL;

	SetSignalMask(SIGUSR1, SIG_BLOCK);
	SetSignalHandler(SIGUSR1, SIGUSR1Handler);
	SetSignalMask(SIGUSR1, SIG_UNBLOCK);
	wd = WdCreate(args);
	wd->revive_task = SpawnTargetTSK;
	WdAddTask(wd, SpawnTargetTSK, 1);
	WdStart(wd);
	DestroyWdArgs(args);
	WdDestroy(wd);

	pthread_exit(NULL);
}

static char** CreateWdArgs(unsigned int interval, unsigned int max_fails, int argc, char* args[])
{
	char** wd_args = NULL;
	int wd_argc = 3 + argc + 1;
	int i;
	
	wd_args = CreateStrings(wd_argc * sizeof(char*));
	
    wd_args[0] = CreateString(strlen(WD_PATH) + 1);
    memcpy(wd_args[0], WD_PATH, strlen(WD_PATH));

    wd_args[1] = CreateString(sizeof(int));
    AssignIntToString(wd_args[1], interval);

    wd_args[2] = CreateString(sizeof(int));
    AssignIntToString(wd_args[2], max_fails);

    for (i = 0; i < argc; ++i)
    {
        wd_args[i+3] = CreateString(strlen(args[i]) + 1);
        memcpy(wd_args[i+3], args[i], strlen(args[i]));
    }
    wd_args[i+3] = NULL;
	
	for(i = 0; i < argc + 4; ++i)
		printf("wd_args[%d] = %s\n,", i, wd_args[i]);
	return (wd_args);
}

int DoNotResuscitate()
{
	g_is_dnr_req = 1;
    return (0);
}

static void DestroyWdArgs(char** wd_args)
{
    unsigned int i = 0;

    while (wd_args[i] != NULL)
    {
        free(wd_args[i]);
        ++i;
    }
    free(wd_args);
}

static void AssignIntToString(char* dest_str, int num)
{
    ExitIfBad(sprintf(dest_str, "%d", num) > 0, "sprintf error",
                      EXIT_FAILURE);
}

static char* CreateString(size_t n)
{
    char* string = (char*) malloc(n * sizeof(char));
    ExitIfBad(string != NULL, "malloc error", EXIT_FAILURE); 
    return (string);
}

static char** CreateStrings(size_t n)
{
    char** strings = (char**) malloc(n * sizeof(char*));
    ExitIfBad(strings != NULL, "malloc error", EXIT_FAILURE); 
    return (strings);
}