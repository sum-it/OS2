#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int
main(int argc, char *argv[])
{
	int pipefd[2];
	pid_t cpid;
	char buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) {/* Child reads from pipe */
		close(pipefd[0]);  /* Close unused read end */
		while(1){
		signal(SIGPIPE, SIG_IGN);
		int x=	write(pipefd[1], argv[1], strlen(argv[1]));
			if(x==-1){
				fprintf(stderr,"we couldn't write:%s\n",strerror(errno));
				if(execv(argv[0],argv)){
					fprintf(stderr,"execv failed%s\n",strerror(errno));
				}
			}
			if(usleep(500000)!=0){ 
               fprintf(stderr,"usleep failed, error:%s\n",strerror(errno)); 
                    exit(EXIT_FAILURE); 
            }
		}
		close(pipefd[1]);  /* Reader will see EOF */
		_exit(EXIT_SUCCESS);

	} else {/* Parent writes argv[1] to pipe */
		close(pipefd[1]);  /* Close unused write end */
		int z=0;
//		read(pipefd[0], &buf, 1);
		 while(cpid!= waitpid(cpid,0,0)){
                 fprintf(stderr, "child dies, respawn\n");
            }
/*		while (read(pipefd[0], &buf, 1) > 0){
			write(STDOUT_FILENO, &buf, 1);
			z++;
	//		if(z==20){
	//			exit(EXIT_FAILURE);
	//		}
		}
*/
		write(STDOUT_FILENO, "\n", 1);
		close(pipefd[0]);
		wait(NULL);/* Wait for child */
		exit(EXIT_SUCCESS);
	}
}
