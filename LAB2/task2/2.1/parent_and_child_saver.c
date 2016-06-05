#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define fork_limit 1000

int pipefd[2];
int backup(void);
void server(int,char **,int );

int main(int argc, char *argv[]) {
	pid_t cpid;
	char buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
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
            while(ret == waitpid(ret,0,0))
                fprintf(stderr, "child dies, respawn\n");
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
		int x=	write(pipefd[1], argv, sizeof(argv));
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
