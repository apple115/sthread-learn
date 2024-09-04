#ifndef SCHEDULER_H
#define SCHEDULER_H

extern void scheduler_init(void);
extern void scheduler_create_task(void (*func)(void*),void *arg);
extern void scheduler_run(void);
extern void scheduler_exit_current_task(void);
extern void scheduler_relinquish(void);

#endif//SCHEDULER_H
