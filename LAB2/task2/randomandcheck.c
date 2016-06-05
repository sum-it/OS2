#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

#define MAX_SIZE 10000
#define fork_limit 1000
int saver[MAX_SIZE];
int counter=0;
int pipefd[2];
int backup(void);
void server(int,char **,int );

int main(int argc,char * argv[]){
	if (argc!=2) {
		fprintf(stderr, "usage: %s filename\n", argv[0] );
		exit(EXIT_FAILURE);
	}
	int count = backup();
    server(argc,argv,count);
}
	
int backup(){
    int ret, restarts = 0;
    for(;;restarts++){ 
        if (restarts > fork_limit){
            fprintf(stderr,"num_forks exceeded: %d \n",restarts);
            return restarts;
        }
        if (pipe(pipefd) == -1) {
            perror("pipe creation failed\n");
            exit(EXIT_FAILURE);
        }
        ret = fork();
        if (ret == 0){ // child process
            printf("Child process: my pid is %d, parent pid is %d\n",
               getpid(), getppid());
            return restarts;
        }
        else if (ret > 0 ){ //parent process
            close(pipefd[1]);  /* Close unused write end */
//            fprintf(stdout, "parent process continues\n");
			int * returnstatus;
            while(ret == waitpid(ret,returnstatus,0)){
                fprintf(stderr, "child dies\n");
				if (WIFEXITED(returnstatus)){ //child exited normly
						printf("child exited with return %d\n",*returnstatus);
					if(WEXITSTATUS(returnstatus)==0){
						fprintf(stdout,
							"child exited with return 0\n");
						exit(EXIT_SUCCESS);
					}
					else continue;
				}
			}
				
        }
        else {
            fprintf(stderr,"Fork failed retrying, %s\n",strerror(errno));       
        }
    }
}
void server (int argc, char ** argv, int count){ //child process
        close(pipefd[0]);  /* Close unused read end */
        while(1){
        signal(SIGPIPE, SIG_IGN);
		char buffer[MAX_SIZE];
		printf("$ ");
		if (fgets(buffer,sizeof buffer, stdin) == NULL){
			fprintf(stderr,"fgets error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		int number=strtoimax(buffer,NULL,10);
		//printf("result:%d\n",	strcmp("0\n",buffer));	
		if (number ==0){
			if (strcmp("0\n",buffer)){
//				printf("%s\n",buffer);
				printf("Writing output to the file.. \n");
				//TODO write to the file fsync and then clean shutdown FDs specially
				FILE * fw = fopen(argv[1],"w");
				if (fw == NULL) {
					fprintf(stderr,"File opening error: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				for (int x=0;x<MAX_SIZE && (saver[x] != -1);x++){
	int count = fprintf(fw,"%d\n",saver[x]);
	if (count < 0) {
		fprintf(stderr,"File writing error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
					printf("%d\n",saver[x]);
				} 
	if (fclose(fw)!=0){
		fprintf(stderr,"File closing error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
				exit(EXIT_SUCCESS);
			}
			else continue;
		}
		if (number < 0){
			fprintf(stderr,"Negative value supplied\n");
			exit(number);
		}
		else{ 
			//printf("%d\n",number);
			int i;
			for (i=0;i<number;i++){
				srand(time(NULL)+i);
				saver[counter] = ( rand() % number);
				printf("%d\n",saver[counter]);
				counter = (counter + 1) % MAX_SIZE;
				saver[counter]=-1;
			}
		}
        int x=  write(pipefd[1], argv, sizeof(argv));
        if(x==-1){
        	fprintf(stderr,"write error:%s\n",strerror(errno));
            if(execv(argv[0],argv)){
           		fprintf(stderr,"execv failed:\t%s\n",strerror(errno));
            }
        }
       	if(usleep(500000)!=0){
       		fprintf(stderr,"usleep failed, error:%s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
   }
   close(pipefd[1]);  /* Reader will see EOF */
   _exit(EXIT_SUCCESS);
}
