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
#include<dirent.h> //Header file for directory manipulations
#define MAX_FORKS 10
int backup(void);
void backup_terminated(void);
int server(char *,int,int);
int counter=0;
int fd[2];
int restarts=0;
char buf;
int fork_limit=MAX_FORKS;
char arguments[10][20];
int main(int argc, char* argv[]){
	for(int i=1;i<argc;i++){
		strcpy(arguments[i],argv[i]);
		printf("%s\n",arguments[i]);
	}
	printf("Parent process started with pid: %d \n",getpid());
	int ch;
	int chances=50;
	char wd[1024];
	if (getcwd(wd, sizeof(wd)) != NULL){} // default directory set to current directory
   	else{
       fprintf(stderr,"getcwd() error: %s\n",strerror(errno));
		return EXIT_FAILURE;
	}
	char *cvalue = NULL;
	while ((ch =getopt(argc,argv, "d:n:f:")) != -1){
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
			case 'f':
				cvalue=optarg;
				chances= strtoimax(cvalue,NULL,10);
//				printf("chances is set to %d\n",chances);
				if(chances ==UINTMAX_MAX && errno ==ERANGE){
					fprintf(stderr,"wrong optional argument, error in conversion: %s\n",
						strerror(errno));
					return EXIT_FAILURE;
				}
				if (chances < 1 || chances > 100){
					fprintf(stderr,"valid range for -f is 1-100\n");
					return EXIT_FAILURE;
				}
				break;
			case 'd':
				cvalue=optarg;
				strcpy(wd,optarg);
				printf("current directory is set to %s\t%s \n",wd,optarg);
				break;
			case ':':
				fprintf(stderr,"Option -%c requires an operand\n",optopt);
				return EXIT_FAILURE;
			case '?':
				fprintf(stderr,"Unrecognised option: -%c\n",optopt);
				return EXIT_FAILURE;
		}
	}
//	printf("current directory is set to %s \n",wd);
	int count = backup();
// Do parent stuff. backup process has started already.
	int rs = server(wd,chances,count);
	if (rs ==EXIT_FAILURE) exit(EXIT_FAILURE);
}
void backup_terminated(void){
	close(fd[0]); //child should close read end
	if((write(fd[1],"x",1))== -1){ //we couldn't write a character on the pipe	
		fprintf(stderr,"pipe not ready:%s\n",strerror(errno));	
	//	if(execv(arguments[0],arguments)){
	//		fprintf(stderr,"execv failed:%s\n",strerror(errno));
	//		exit(EXIT_FAILURE);
	//	}
	}
}
int server(char * wd,int chances,int count ){
	while(1){
		DIR * dp = NULL;
		struct dirent *dptr =NULL;
		if ((dp = opendir(wd))==NULL){
			fprintf(stderr,"directory opening error:%s: %s\n",wd,strerror(errno));
			return EXIT_FAILURE;
		}
		char pd[1024];
		if (getcwd(pd, sizeof(pd)) != NULL){} // save current directory 
   		else{
	       fprintf(stderr,"getcwd() error: %s\n",strerror(errno));
			return EXIT_FAILURE;
		}
  		if (chdir(wd)== -1){
            fprintf(stderr,"change directory error : %s\n",strerror(errno));
        }
		while((dptr = readdir(dp))!=NULL){
			if (dptr->d_type ==DT_REG){
				backup_terminated();
				//file we were looking for
				//open it read it and delete and close it
				FILE *fp;
				if((fp =fopen(dptr->d_name,"r"))==NULL){
					fprintf(stderr, "File opening error: %s\n",strerror(errno));
					return EXIT_FAILURE;
				}
				if(strstr(dptr->d_name,"fail")!=NULL){ //case where file name has fail in it
					srand(time(NULL));
					if((rand() % 100) < chances ) { //probability matched
						exit(EXIT_FAILURE);
					}
				}
				fprintf(stdout,"server:%d count:%d processed:%s\t from %s\n",
					getpid(),count,dptr->d_name,wd);
				if(usleep(5000000)!=0){ 
					fprintf(stderr,"usleep failed, error:%s\n",strerror(errno)); 
					exit(-1); 
				}
				if (fclose(fp)!=0){
					fprintf(stderr,"File Closing error: %s\n",strerror(errno));
				}
				if (unlink(dptr->d_name)== -1){
					fprintf(stderr,"File deletion error: %s\n",strerror(errno));
				}
			}
		}
	 	if (chdir(pd)== -1){
            fprintf(stderr,"change directory error : %s\n",strerror(errno));
        }

		closedir(dp);
	}
}
int backup(){
	int ret;
	if(pipe(fd) == -1){
		fprintf(stderr,"Pipe creation failed:%s",strerror(errno));
		exit(EXIT_FAILURE);
	}
	for(;;restarts++){
		if (restarts > fork_limit){
			printf("num_forks exceeded: %d ",restarts);
			return restarts;
		}
        ret = fork();
        if (ret == 0){ // child process
            printf("Child process: my pid is %d, parent pid is %d\n",
               getpid(), getppid());
			return restarts;
        }
		else if (ret > 0 ){ //parent process
			fprintf(stdout, "parent process continues\n");
			close(fd[1]); //close write end in parent
//			read(fd[0], &buf,1);
			while(ret != waitpid(ret,0,0)){
				fprintf(stderr, "child dies, respawn\n");
			}
		}
		else {
			fprintf(stderr,"Fork failed retrying, %s\n",strerror(errno));
		}
	}
}
