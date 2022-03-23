sched: sched.c utils.c driver.c
	gcc -o sched driver.c sched.c utils.c -I /include