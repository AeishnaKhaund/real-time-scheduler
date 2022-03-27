# Frame Based Scheduling
A scheduler that schedules tasks with hard or soft deadlines. Ongoing and made as part of an assignment for a course called "Real Time Systems".

### Currently completed:
- takes a set of periodic tasks as input
- calculates hyperperiod
- finds best frame size for scheduling
- computes network flow graph for these tasks and applies ford fulkerson algorithm to this

### Planned design:
- finds the static schedule for the periodic tasks and stores it in periodicSchedule.txt
- at runtime, stores aperiodic jobs in job queue and completes them with slack stealing
- takes sporadic jobs, conducts acceptance test and schedules them if they pass the test.
