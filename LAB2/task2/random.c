#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

#define MAX_SIZE 10000
int saver[MAX_SIZE];
int counter=0;

int main(int argc,char * argv[]){
	if (argc!=2) {
		fprintf(stderr, "usage: %s filename\n", argv[0] );
		exit(EXIT_FAILURE);
	}
	FILE * fw = fopen(argv[1],"w");
	if (fw == NULL) {
		fprintf(stderr,"File opening error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	while(1){
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
				//TODO write to the file
				for (int x=0;x<MAX_SIZE && (saver[x] != -1);x++){
					printf("%d\n",saver[x]);
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
	}
	int count = fprintf(fw,"test\n");
	if (count < 0) {
		fprintf(stderr,"File writing error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (fclose(fw)!=0){
		fprintf(stderr,"File closing error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}
