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
//Header file for random seed 
#include<time.h>
int backup();
void wait_for_request(int);
void process_request();
int counter=0;
int main(int argc, char* argv[]){
	printf("Parent process started with pid: %d \n",getpid());
	int ft = backup();
// Do parent stuff. backup process has started already.
	while (1){
		wait_for_request(ft);
		process_request();
	}
	return EXIT_SUCCESS;
}
int backup(){
	int ret, restarts = 0;
    for(;;restarts++){
        ret = fork();
        if (ret == 0){ // child process
            printf("Child process: my pid is %d, parent pid is %d\n",
               getpid(), getppid());
			return restarts;
        }
		else if (ret > 0 ){ //parent process
			fprintf(stdout, "parent process continues\n");
			while(ret != waitpid(ret,0,0)){
				fprintf(stderr, "child dies, respawn");
			}
		}
		else {
			fprintf(stderr,"Fork failed, %s\n",strerror(errno));
			return EXIT_FAILURE;
		}
	}
}
void wait_for_request(int ft){
	if(usleep(1000000)!=0){ 
		printf("usleep failed, error:%s\n",strerror(errno)); 
		exit(-1); 
	}
	fprintf(stdout,"request %d found, process countis: %d\n",counter,ft);
}
void process_request(){
	if(usleep(1000000)!=0){ 
		printf("usleep failed, error:%s\n",strerror(errno)); 
		exit(-1); 
	}
	fprintf(stdout,"request %d processed\n",counter++);
}
