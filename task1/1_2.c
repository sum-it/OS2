#define _BSD_SOURCE // used for usleep function
//Standard input output header file
#include<stdio.h>
//Header file for process creation, identifications and types.
#include<sys/types.h>
#include<unistd.h>
//Header file for process terminations
#include<stdlib.h>
//Header file for error number
#include<errno.h>
//Header file for string processing
#include<string.h>
//Header file for wait calls
#include<sys/wait.h>
#define FORK_WAIT 250000
#define CHILD_WAIT 500000

int main(int argc, char* argv[]){
	printf("Parent process started with pid: %d \n",getpid());
	pid_t p_return_val = fork();
	if(usleep(FORK_WAIT)!=0){
		printf("usleep failed, error:%s\n",strerror(errno));
		return EXIT_FAILURE;
	}
	if (p_return_val == 0){// child process
		if(usleep(CHILD_WAIT)!=0){
			printf("usleep failed, error:%s\n",strerror(errno));
			return EXIT_FAILURE;
		}
		printf("Child process: my pid is %d, parent pid is %d\n",
               getpid(), getppid());
		return EXIT_SUCCESS;
	}
	else if (p_return_val > 0){ // parent process
		printf("Parent process: Child created, my pid is:%d,\t"
			 "Child's pid is %d\n",getpid(), p_return_val);
		int child_status;
		int wait_return_v;
		int error_val;
		wait_return_v =waitpid(p_return_val,&child_status,0);
		if(wait_return_v == p_return_val){
			error_val=errno;
			printf("Child:%d, we were waiting for, is terminated\n"
					,p_return_val);
			if(WIFEXITED(child_status)){
				printf("Child terminated normally with exit status"
					": %d\n",WEXITSTATUS(child_status));
			}
			else{
				printf("Child exited abnormally. Exit status:"
					"%s\n",strerror(error_val));
				return EXIT_FAILURE;
			}
		}
		else{
			error_val=errno;
			printf("waitpid call failed, error value:%s\n",
					strerror(error_val));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}
	else if (p_return_val < 0){
		printf("fork creation failed with error: %s",strerror(errno));
		return EXIT_FAILURE;
	}
	else{
		printf("This should never happen since all cases are"
				" covered by other if else branches");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
