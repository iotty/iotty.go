#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

// sync primitive
int checkpoint[2];

static char child_stack[STACK_SIZE];

char *const child_args[] = {
	"/bin/bash",
	NULL
};

int child_main(void *arg)
{
	char c;

	// init sync primitive
	close(checkpoint[1]);

	// setup hostname
	printf(" - [%5d] World!\n", getpid());
	sethostname("In Namespace", 12);

	// remount "/proc" to get accurate "top" && "ps" output
	mount("proc", "/proc", "proc", 0, NULL);

	// wait...
	read(checkpoint[0], &c, 1);
	execv(child_args[0], child_args);
	printf("Ooops\n");
	return 1;
}

int main()
{
	// init sync primitive
	pipe(checkpoint);
	printf(" - [%5d] Hello?\n", getpid());
	int child_pid = clone(child_main, child_stack + STACK_SIZE,
			CLONE_NEWUTS|CLONE_NEWIPC|
			CLONE_NEWPID|CLONE_NEWNS|SIGCHLD, NULL);

	// further init here (nothing yet)

	// signal "done"
	close(checkpoint[1]);
	waitpid(child_pid, NULL, 0);
	return 0;
}
