#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__


int MakeMeImmortal(int argc, char* argv[], const unsigned long interval,
                   const int max_fails);
			   
int DoNotResuscitate();


#endif  /* __WATCHDOG_H__ */
