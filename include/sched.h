#ifndef SCHED_H
#define SCHED_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define PERIODIC 0
#define APERIODIC 1
#define SPORADIC 2
typedef uint stime;           //scheduler time


typedef struct task{
    //simple task that has a type, arrival time, wcet, period, relative deadline
    //wcet = worst case execution time
    short type;
    stime arrival;
    stime wcet;
    stime p;
    stime D;
} task;

typedef struct job job;
typedef struct queue queue;
typedef struct list_node node;
typedef struct graph_node gnode;

/* *** DATA STRUCTURES *** */



struct job{
    short type;
    stime arrival;
    stime wcet;
    stime et;     //maybe smaller than wcet by a random factor
    stime d;      //absolute deadline
};

struct list_node{
    void* val;
    struct list_node* nx;  //next node
};

struct graph_node{
    int val;
    int nr_nodes;
    struct graph_node** nodes;
};

struct queue{
    int size;
    node *front;
    node *back;
};

/* *** SCHEDULE FUNCTIONS *** */

//is_feasible: also calculates hyperperiod and maximum exec. time(useful for frame calculation)
bool is_feasible(task* p_tasks, int n_p, stime* hyperPeriod, stime* max_wcet); 

int ford_fulkerson(int n_j, int f, int F, queue job_queues[]);

int findFramesize(stime H, stime max_wcet, task* p_tasks, int n_p);

void makeTable(task p_tasks[], FILE* file);

int queue_aperiodic(FILE* file, queue* ap_q);

void schedule();    //scheduler goes online

/* *** UTILS *** */

//task related utils
void printTask(task t);
int storeTasks(FILE* src, task tasks[], int type); //returns number of tasks


//math utils
int lcm(int a, int b);
void findFactors(int n, int min, queue* factors);   //arranges factors in decreasing order in queue

//queue utils
void enqueue(queue* q, void* val);
void dequeue(queue* q);
void free_queue(queue* q);


//misc utils
void helloWorld();          //debug

#endif /* SCHED_H */





