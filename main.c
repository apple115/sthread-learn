#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

struct tester_args {
    char *name;
   int iters;
};

void tester(void *arg) {
    struct tester_args *ta = (struct tester_args *)arg;
    for (int i=0; i<ta->iters; i++) {
        printf("task %s:%d \n",ta->name,i);
        scheduler_relinquish();
    }
    free(ta);
}

void create_test_task(char *name,int iters) {
    struct tester_args *ta = malloc(sizeof(struct tester_args));
    ta->name = name;
    ta->iters =iters;
    scheduler_create_task(tester,ta);
}

int main(int argc,char **argv) {
    scheduler_init();
    create_test_task("A", 5);
    create_test_task("B", 4);
    create_test_task("C", 2);
    scheduler_run();
    printf("finished running all task!\n");
    return EXIT_SUCCESS;
}

