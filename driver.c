#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sched.h>

//input files
FILE* periodic;
FILE* aperiodic;
FILE* sporadic;
FILE* ftable;

int main(int argc, char *argv[]){

    helloWorld();
    //taking input files from command line arg
    //executed as "./sched path1 path2 path3"
    
    periodic = fopen(argv[1], "r");
    //aperiodic = fopen(argv[2], "r");
    //sporadic = fopen(argv[3], "r");

    //arrays to store tasks
    struct task* p_tasks;
    task* a_tasks;
    task* s_tasks;

    //store periodic tasks first and check feasibility
    stime hyperPeriod, frame_size, max_wcet;
    int n_p = storeTasks(periodic, p_tasks, PERIODIC);
    if(is_feasible(p_tasks, n_p, &hyperPeriod, &max_wcet)){
        
        //make sched table
        int f = findFramesize(hyperPeriod, max_wcet, p_tasks, n_p);
        //store table in periodicSchedule.txt
        ftable = fopen("periodicSchedule.txt", "w");
        goto scheduler;
    }
    else{

        //report error and exit scheduler
        printf("The given tasks cannot be scheduled. Please change periodic task set for feasible schedule table.\n");
        goto out;
    }

    scheduler:
    //execution comes to this label if the frame table has been made

    out:
    printf("Exitting scheduler...\n");

    //close all files
    fclose(periodic);
    fclose(aperiodic);
    fclose(sporadic);
    
}

