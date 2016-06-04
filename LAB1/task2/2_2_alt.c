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
int backup();
int server(char *);
void wait_for_request(int);
void process_request();
int counter=0;
int fork_limit=MAX_FORKS;
int main(int argc, char* argv[]){
	printf("Parent process started with pid: %d \n",getpid());
	int ch;
	char wd[1024];
	if (getcwd(wd, sizeof(wd)) != NULL){} // default directory set to current directory
   	else{
       fprintf(stderr,"getcwd() error: %s\n",strerror(errno));
		return EXIT_FAILURE;
	}
	char *cvalue = NULL;
	while ((ch =getopt(argc,argv, "d:n:")) != -1){
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
	int ft = backup();
// Do parent stuff. backup process has started already.
	int rs = server(wd);
	if (rs ==EXIT_FAILURE) return EXIT_FAILURE;
}
int server(char * wd){
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
		chdir(wd);
		while((dptr = readdir(dp))!=NULL){
			if (dptr->d_type ==DT_REG){
				//file we were looking for
				//open it read it and delete and close it
				FILE *fp;
				if((fp =fopen(dptr->d_name,"r"))==NULL){
					fprintf(stderr, "File opening error: %s\n",strerror(errno));
					return EXIT_FAILURE;
				}
				fprintf(stdout,"server%d processed:%s\t from %s\n",
					getpid(),dptr->d_name,wd);
				if(usleep(500000)!=0){ 
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
		chdir(pd); //move back to previous directory
		closedir(dp);
	}
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
