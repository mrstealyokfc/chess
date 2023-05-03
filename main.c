#include<stdio.h>
#include<string.h>
#include<stdbool.h> 
#include<stdint.h>
#include<stdlib.h>

#include "server.h"

uint16_t port;

bool process_args(int argc, char** argv){
	if(argc <= 1) 
		return true;
	argv= &argv[1];
	argc--;
	while(argc != 0){
		if(strcmp("-p",argv[0]) == 0){	
			if(argc <= 1){
				perror("you must provide an agrument with -p\n");
				return false;
			}
			port = atoi(argv[1]);
			if(port == 0){
				fprintf(stderr,"invalid port: %s\n", argv[1]);
				return false;
			}
			argc--;
			argv= &argv[1];	
		}

		argv=&argv[1];
		argc--;
	}
	return true;
}

int main(int argc, char** argv){
	process_args(argc,argv);
}
