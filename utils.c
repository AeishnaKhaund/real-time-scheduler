#include <sched.h>

//general utils

void printTask(task t){
    
    printf("type: %d \narrival: %d \nwcet: %d \nperiod: %d \nrelative deadline: %d \n", t.type, t.arrival, t.wcet, t.p, t.D);
}

//math utils

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int lcm(int a, int b){
    return (a * b)/gcd(a, b);
}

void findFactors(int n, int min, queue* factors){

    for(int i = n; i >= min; i--){
        if(n % i == 0){
            int* tmp; *tmp = i;
            enqueue(factors, tmp);
        }
    }
}

int storeTasks(FILE* src, task* tasks, int type){
    int n;
    fscanf(src, "%d", &n);
    tasks = (task*) malloc(n * sizeof(task));
    for(int i = 0; i < n; i++){
        fscanf(src, "%d %d %d %d", &tasks[i].arrival, &tasks[i].wcet, &tasks[i].p, &tasks[i].D);
        tasks[i].type = type;
    }
    return n;
}


//queue utils
void enqueue(queue* q, void* val){
    if(q->front){
        //non-empty queue
        node* tmp = (node*) malloc(sizeof(node));
        tmp->val = val;
        q->back->nx = tmp;
        q->back = tmp;
    }
    else{
        //empty queue
        q->front = (node*) malloc(sizeof(node));
        q->front->val = val;
        q->back = q->front;
        q->size = 1;
    }
    q->size++;
}

void dequeue(queue* q){
    if(!q->front){
        //empty queue
        q->size = 0;
        return;
    }
    node* tmp = q->front;
    q->front = q->front->nx;
    q->front = NULL;
    free(tmp);
}

void free_queue(queue* q){
    node* itr = q->front, *itr2;
    while(itr){
        itr2 = itr->nx;
        free(itr);
        itr = itr2;
    }
}

//debug
void helloWorld(){
    printf("Hello world!\n");
}

/*
    char* task = NULL;
    size_t n = 0;

    //check if read correctly
    getline(&task, &n, periodic);
    printf("%s\n", task);
    free(task);

    getline(&task, &n, aperiodic);
    printf("%s\n", task);
    free(task);

    getline(&task, &n, sporadic);
    printf("%s\n", task);
    free(task);
*/

    // task t1;
    // t1.type = PERIODIC;
    // t1.arrival = 0;
    // t1.wcet = 2;
    // t1.p = 5;
    // t1.D = 10;