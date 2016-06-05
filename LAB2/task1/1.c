#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <criu/criu.h>
#define DIRECTORY "dump"
void error_msg(int);
//int main(int argc, char* argv[]) {
int main() {
	int freturn=0;
	mkdir(DIRECTORY,0700);
/*	if ((mkdir(DIRECTORY,0700))!=0){
		fprintf(stderr,"Directory creation failed\n");	
		return EXIT_FAILURE;
 	}
*/
int fd;
 	if((fd = open(DIRECTORY,O_DIRECTORY))==-1){
		fprintf(stderr,
		"directory's open call failed: %s\n",strerror(errno));
        return EXIT_FAILURE;
	}

	if( (freturn = criu_init_opts())!=0){
		fprintf(stderr," CRIU init call failed:\n");
        return EXIT_FAILURE;
	}
		

	 criu_set_images_dir_fd(fd);
	 criu_set_service_address("criu_service.socket");
	 criu_set_service_comm(CRIU_COMM_SK);
	if( (freturn = criu_check())!=0){
		fprintf(stderr," CRIU check call failed:\n");
		error_msg(freturn); 
        return EXIT_FAILURE;
	}
 
 return 0;
}
void error_msg(int code){
	switch (code) {
	case -EBADE:
		perror("RPC has returned fail");
		break;
	case -ECONNREFUSED:
		perror("Unable to connect to CRIU");
		break;
	case -ECOMM:
		perror("Unable to send/recv msg to/from CRIU");
		break;
	case -EINVAL:
		perror("CRIU doesn't support this type of request."
		       "You should probably update CRIU");
		break;
	case -EBADMSG:
		perror("Unexpected response from CRIU.");
		break;
	default:
		perror("Unknown error type code.");
	}
}
