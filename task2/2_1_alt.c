#define _BSD_SOURCE // used for usleep function
#include<stdio.h> //Standard input output header file
#include<sys/types.h> //Header file for process creation, identifications and types.
#include<unistd.h> //Header file for getopt processing
#include<stdlib.h> //Header file for process terminations
#include<errno.h> //Header file for error number
#include<string.h> //Header file for string processing
#include<sys/wait.h> //Header file for wait calls
#include<time.h> //Header file for random seed 
#include<inttypes.h> //Header file for integer conversion 
#define MAX_FORKS 5
int backup();
void wait_for_request(int);
void process_request();
int counter=0;
int fork_limit=MAX_FORKS;
int main(int argc, char* argv[]){
	printf("Parent process started with pid: %d \n",getpid());
	int ch;
	char *cvalue = NULL;
	while ((ch =getopt(argc,argv, "n:")) != -1){
		switch (ch){
			case 'n':
				cvalue=optarg;
			fork_limit= strtoimax(cvalue,NULL,10);
//				printf("fork limit is set to %d\n",fork_limit);
				if(fork_limit ==UINTMAX_MAX && errno ==ERANGE){
					fprintf(stderr,"wrong optional argument, error in conversion: %s\n",
						strerror(errno));
					return EXIT_FAILURE;
				}
				if (fork_limit<1 || fork_limit > 50){
					fprintf(stderr,"valid range for -n is 1-50\n");
					return EXIT_FAILURE;
				}
				break;
			case ':':
				fprintf(stderr,"Option -%c requires an operand\n",optopt);
				return EXIT_FAILURE;
			case '?':
				fprintf(stderr,"Unrecognised option: -%c\n",optopt);
				return EXIT_FAILURE;
		}
	}
	int ft = backup();
// Do parent stuff. backup process has started already.
	fprintf(stdout,"server %d\t restart:%d\n",getpid(),ft);
	return EXIT_SUCCESS;
}
int backup(){
	int ret, restarts = 0;
	for(;;restarts++){
        ret = fork();
        if (ret == 0){ // child process
//            printf("Child process: my pid is %d, parent pid is %d\n",
//              getpid(), getppid());
			return restarts;
        }
		else if (ret > 0 ){ //parent process
//			fprintf(stdout, "parent process continues\n");
			while(ret == waitpid(ret,0,0)){
				fprintf(stderr, "child dies, respawn\n");
			}
		}
		else {
			if (restarts > fork_limit){
				fprintf(stderr,"Fork failed and num_forks exceeded: %d error: , %s\n",restarts,strerror(errno));
				return restarts;
			}
			fprintf(stderr,"Fork failed retrying, %s\n",strerror(errno));
		}
	}
}
void wait_for_request(int ft){
	if(usleep(1000000)!=0){ 
		printf("usleep failed, error:%s\n",strerror(errno)); 
		exit(-1); 
	}
	fprintf(stdout,"request %d found, process count is: %d\n",counter,ft);
}
void process_request(){
	if(usleep(1000000)!=0){ 
		printf("usleep failed, error:%s\n",strerror(errno)); 
		exit(-1); 
	}
	fprintf(stdout,"request %d processed\n",counter++);
}
