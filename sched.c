
// #include <Utils.h>

#include <sched.h>

/*
To-do:
- task-splitting when feasible frame not found; currently just gives error and returns
- the random numbers generated are deterministic over every run of the scheduler
*/

bool is_feasible(task* p_tasks, int n_p, stime* hyperPeriod, stime* max_wcet){

    //check total utilization and return false if > 1
    //also calculates hyperPeriod for tasks by finding successive lcm's of task periods.
    float total_ut = (float)p_tasks[0].wcet/p_tasks[0].p;
    stime hp = p_tasks[0].p;

    for(int i = 1; i < n_p; i++){
        total_ut += ((float)p_tasks[i].wcet / (float)p_tasks[i].p);
        hp = lcm(hp, p_tasks[i].p);
        printf("%f\n", total_ut);
    }

    if(total_ut > 1.0)
        return false;

    hyperPeriod = hp;
    return true;
}

bool bfs(int** rGraph, int s, int t, int* parent, int n)
{
    // Create a visited array and mark all vertices as not
    // visited
    bool visited[n];
    memset(visited, 0, sizeof(visited));
 
    // Create a queue, enqueue source vertex and mark source
    // vertex as visited
    queue q; int s1 = s;
    enqueue(&q, &s1);
    visited[s] = true;
    parent[s] = -1;
 
    // Standard BFS Loop
    while (q.front) {
        int u = *(int*)q.front->val;
        dequeue(&q);
 
        for (int v = 0; v < n; v++) {
            if (visited[v] == false && rGraph[u][v] > 0) {
                // If we find a connection to the sink node,
                // then there is no point in BFS anymore We
                // just have to set its parent and can return
                // true
                if (v == t) {
                    parent[v] = u;
                    return true;
                }
                int *v2 = (int*) malloc(sizeof(int)); *v2 = v;
                enqueue(&q, v2);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
 
    // We didn't reach sink in BFS starting from source, so
    // return false
    return false;
}

bool ford_fulkerson(int** rGraph, int n_j, int f, int F, queue job_queues[]){
    int n = 2 + n_j + F;    //total node = source + sink + #jobs + #frames
    //capacities: 0 to n_j-1 -> job vertices; n_j to n_j+F-1 -> frame vertices; n-2 -> source; n-1 -> sink
    // int flow[n][n];
    int parent[n];             //used to store path of max flow 
    memset(parent, -1, sizeof(parent));
    
    int sum_exec = 0;

    // setting capacities of edges

    for(int i = 0; i < n_j; i++){
        node *itr = job_queues[i].front;
        while(itr){
            //itr points to jobs of task i
            //set capacity of source to job
            job* jb = (job*)itr->val;
            rGraph[n-2][i] = jb->wcet;
            rGraph[i][n-2] = jb->wcet;

            sum_exec += jb->wcet;

            //set capacity for this job to every frame vertex
            int s = 0, e = 0;
            for(int j = 0; j < F; j++){
                //this frame starts at i*f and ends at (i+1)*f
                //if this job has arrival before start and deadline after end, it has an edge to this frame
                //set this cap to frame size
                s = i*f; e = (i+1)*f;
                if(jb->arrival <= s && e <= jb->d){
                    rGraph[i][n_j+j] = f;
                    rGraph[n_j+j][i] = f;
                }
                if(jb->arrival > s)
                    break;
            }
            itr = itr->nx;
        }
    }
    //set cap of edges from frame vertices to sink as f
    for(int i = 0; i < F; i++){
        rGraph[n_j+i][n-1] = f;
        rGraph[n-1][n_j+1] = f;
    }

 
    int max_flow = 0; // There is no flow initially
    int u, v;
 
    while (bfs(rGraph, 0, n-1, parent, n)) {

        int path_flow = 1000000;
        for (v = 0; v != n; v = parent[v]) {
            u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }
 
        for (v = 0; v != n; v = parent[v]) {
            u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }
 
        max_flow += path_flow;
    }
 
    return (max_flow == sum_exec) ? true : false;
}

int makeJobQueues(queue* job_queues, task* p_tasks, int f, int F, int n_p){

    //make jobs for each task
    int numJobs = 0;
    for(int i = 0; i < n_p; i++){
        int n_j = (f * F)/p_tasks[i].p;
        numJobs += n_j;
        for(int j = 0; j < n_j; j++){
            job* pjob = (job*) malloc(sizeof(job));
            pjob->arrival = p_tasks[i].arrival*(j+1);
            pjob->d = pjob->arrival + p_tasks[i].D;
            pjob->wcet = p_tasks[i].wcet;
            // if(rnd_et){
            //     float factor = 0.20 + (float)rand()/(float)(RAND_MAX/0.80); //generates random number in 0.20 to 1.00 range
            //     pjob->et = factor * pjob->wcet;
            // }
            // else 
            pjob->et = pjob->wcet;

            //queue the job in task queue
            enqueue(&job_queues[i], &pjob);
        }
    }
    return numJobs;

}

int findFramesize(stime H, stime max_wcet, task* p_tasks, int n_p){
    //E1 -> frame size must be bigger than max_wcet
    //find all factors of hyperperiod - satisfying E2
    queue* factors = (queue*) malloc(sizeof(queue));
    findFactors(H, 0, factors);
    node* itr = factors->front;
    bool split = false;         //whether task splitting is required
    int f = 0;
    while(itr){
        f = *(int*)itr->val;
        if(f < max_wcet)
            split = true;
        for(int i = 0; i < n_p; i++){
            if( 2*f - gcd(p_tasks[i].p, f) > p_tasks[i].D)
                break;
            
            // if it is here, this frame size satisfies E2 and E3
            // we can check if it satisfies maximum_flow = sum of execution times.
            queue job_queues[n_p];  //one queue for each task, containing jobs of that task
            int numJobs = makeJobQueues(job_queues, p_tasks, f, H/f, n_p);
            int n = numJobs + 2 + H/f;
            int rGraph[n][n];
            memset(rGraph, -1, sizeof(rGraph)); 
            if(ford_fulkerson(rGraph, numJobs, f, H/f, job_queues)){
                for(int x = 0; x < n; x++){
                    for(int y = 0; y < n; y++){
                        printf("%d\t", rGraph[x][y]);
                    }
                    printf("\n");
                }
                return f;
            }

        }
        itr = itr->nx;
    }

}

void makeTable(task p_tasks[], int n_p, FILE* file, int f, int F, bool rnd_et){
    // f = frame size
    // F = no. of frames in schedule
    // rnd_et = whether or not the execution time will be smaller than wcet by random amount

    queue job_queues[n_p];  //one queue for each task, containing jobs of that task


    queue frame_queues[F];  //one queue for each frame, containing the jobs to be executed in that frame
    
    
}

int queue_aperiodic(FILE* file, queue* ap_q){
    //queuing jobs, not tasks here
    int n;
    fscanf(file, "d", &n);
    for(int i = 0; i < n; i++){
        job* tmp = (job*)malloc(sizeof(task));
        fscanf(file, "%d %d", &tmp->arrival, &tmp->wcet);
        tmp->type = APERIODIC;
        enqueue(ap_q, tmp);
    }
    return n;
}

