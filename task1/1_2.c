//Standard input output header file
#include<stdio.h>

//Header file for process identifications and types.
#include<sys/types.h>
#include<unistd.h>

//Header file for process terminations
#include<stdlib.h>


int main(int argc, char* argv[]){
	printf("Parent process started with pid: %d \n",getpid());
	return EXIT_SUCCESS;
}
