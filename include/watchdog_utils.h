#include "scheduler.h"

typedef struct wd_ty
{
    scheduler_ty* scheduler;
    int           argc;
    char**        target_args;
    char*         target_app_name;
    unsigned long interval;
    unsigned long max_fails;
    unsigned long fails;
    int           (*revive_task)(void*);
    pid_t         target_pid;
} wd_ty;


wd_ty* WdCreate(char** args);

void WdDestroy(wd_ty* wd);

int WdAddTask(wd_ty* wd, int (*task)(void *), unsigned long interval);

void WdClearTasks(wd_ty* wd);

void WdStart(wd_ty* wd);

void WdStop(wd_ty* wd);

void WdSendSignal(wd_ty* wd, int sig_num);

void WdExecTarget(wd_ty* wd);

void WdSpawnTarget(wd_ty* wd);

int WdWaitPid(wd_ty* wd);

int SendSolTSK(void* args);

int CheckSolTSK(void* args);

int ReviveIfErrorTSK(void* args);

void DoNothingTSK(void* args);

int SetSignalMask(int sig_num, int val);

int SetSignalHandler(int sig_num, void (*handler)(int));

void SIGUSR1Handler(int sig_num);

int GetSol(void);