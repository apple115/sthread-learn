#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "scheduler.h"

struct task {
    enum {
        ST_CREATED,
        ST_RUNNING,
        ST_WAITING,
    } status;

    int id;
    jmp_buf buf;

    void (*func)(void*);
    void *arg;

    struct list_head task_list;

    void *stack_bottom;
    void *stack_top;
    int stack_size;
};

enum {
    INIT=0,
    SCHEDULE,
    EXIT_TASK,
};

struct scheduler_private {
    jmp_buf buf;
    struct task *current;
    struct list_head task_list;
} priv;

void scheduler_init() {
    priv.current =NULL;
    list_head_init(&priv.task_list);
}

void scheduler_create_task(void (*func)(void *), void *arg)
{
    static int id = 1;
    struct task *task = malloc(sizeof(*task));
    task->status = ST_CREATED;
    task->func = func;
    task->arg = arg;
    task->id = id++;
    task->stack_size = 16 * 1024;
    task->stack_bottom = malloc(task->stack_size);
    task->stack_top = task->stack_bottom + task->stack_size;
    list_add_tail(&task->task_list,&priv.task_list);
}

static struct task *scheduler_choose_task(void)
{
    struct task *task;
    list_for_each_entry(task, &priv.task_list,task_list)
    {
        if (task->status == ST_RUNNING || task->status == ST_CREATED) {
            list_move_tail(&task->task_list,&priv.task_list);
            return task;
        }
    }
    return NULL;
}

static void schedule(void) {
    struct task *next = scheduler_choose_task();
    if(!next)
        return ;
    priv.current =next;
    if(next->status == ST_CREATED) {
        register void *top = next->stack_top;
        asm volatile(
            "mov %[rs], %%rsp \n"
            : [ rs ] "+r" (top) ::
        );
        next->status = ST_RUNNING;
        next->func(next->arg);
        scheduler_exit_current_task();
    } else {
        longjmp(next->buf, 1);
    }
}

void scheduler_relinquish(void)
{
    if (setjmp(priv.current->buf)) {
        return;
    } else {
        longjmp(priv.buf, SCHEDULE);
    }
}

void scheduler_exit_current_task(void)
{
    struct task *task = priv.current;
    list_del(&task->task_list);
    longjmp(priv.buf, EXIT_TASK);
}

static void scheduler_free_current_task(void)
{
    struct task *task = priv.current;
    priv.current = NULL;
    free(task->stack_bottom);
    free(task);
}


void scheduler_run(void)
{
    /* This is the exit path for the scheduler! */
    switch (setjmp(priv.buf)) {
    case EXIT_TASK:
        scheduler_free_current_task();
    case INIT:
    case SCHEDULE:
        schedule();
        /* if return, there's nothing else to do and we exit */
        return;
    default:
        fprintf(stderr, "Uh oh, scheduler error\n");
        return;
    }
}

