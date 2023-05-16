#include<stdio.h>
#include<string.h>
#include<stdbool.h> 
#include<stdint.h>
#include<stdlib.h>
#include<poll.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "client_actions.h"
#include "server.h"
#include "command.h"

int port = 6969;

bool use_ipv6 = false;
int static_connection_limit = 0;
bool shouldContinue = true;
char* client_message_buffer;

nfds_t max_connections;
struct pollfd *connections_list;


bool process_args(int argc, char** argv){
	if(argc <= 1) 
		return true;
	argv= &argv[1];
	argc--;
	while(argc != 0){
		if(strcmp("-p",argv[0]) == 0 || strcmp("--port",argv[0])==0){
			if(argc <= 1){
				fprintf(stderr,"you must provide an agrument with -p\n");
				return false;
			}
			port = atoi(argv[1]);
			if(port <= 0 || port > UINT16_MAX){
				fprintf(stderr,"invalid port: %s\n", argv[1]);
				return false;
			}
			argc--;
			argv= &argv[1];	
		}else if (strcmp("-s",argv[0]) == 0 | strcmp("--static-connection-limit",argv[0]) ==0){
			if(argc <= 1){
				fprintf(stderr, "you must provide an agrument with -s/--static-connection-limit\n");
				return false;
			}
			static_connection_limit = atoi(argv[1]);

			argc--;
			argv= &argv[1];
		}
		else if(strcmp("--use-ipv6",argv[0])==0 || strcmp("-6", argv[0]) == 0)
			use_ipv6=true;
		else{
			fprintf(stderr, "unknow option: %s",argv[0]);
			return false;
		}

		argv=&argv[1];
		argc--;
	}
	return true;
}
//TODO clean this part up a little bit.
void accept_new_client(struct pollfd server){
	if((POLLERR | POLLHUP | POLLNVAL) & server.revents)
		fprintf(stderr, "POLL ERROR ON SERVER SOCKET\n");

	int client_fd;

	if(use_ipv6){
		struct sockaddr_in6 addr;
		socklen_t addr_len = sizeof(addr);
		char str_addr[INET6_ADDRSTRLEN] = { 0 };
		client_fd = accept(server.fd,(struct sockaddr*)&addr,&addr_len);
		if(client_fd == -1){
			fprintf(stderr, "accept returned invalid client\n");
			return;
		}
		inet_ntop(AF_INET6, &(addr.sin6_addr), str_addr,sizeof(str_addr));
		printf("New Client: %s:%d\n",str_addr, ntohs(addr.sin6_port));
	}
	else{
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		char str_addr[INET_ADDRSTRLEN] = { 0 };
		client_fd = accept(server.fd,(struct sockaddr*)&addr,&addr_len);
		if(client_fd == -1){
			fprintf(stderr, "accept returned invalid client\n");
			return;
		}
		inet_ntop(AF_INET, &(addr.sin_addr), str_addr,sizeof(str_addr));
		printf("New Client: %s:%d\n",str_addr, ntohs(addr.sin_port));
	}

	before:
	for(int i=0;i<(int)max_connections;i++){
		if(!connections_list[i].events){
			connections_list[i].fd=client_fd;
			connections_list[i].events= POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
			return;
		}
	}
	if(static_connection_limit <= 0){
		struct pollfd* new_buffer = calloc(max_connections*2,sizeof(struct pollfd));
		memcpy(new_buffer,connections_list,max_connections*sizeof(struct pollfd));
		free(connections_list);
		max_connections*=2;
		connections_list = new_buffer;
		goto before;
	}

	const char* close_message = "E01 Server is full, you cannot join";
	write(client_fd,close_message,strlen(close_message));
	close(client_fd);

}

void process_client_data(struct pollfd* client){

	if((POLLERR | POLLHUP | POLLNVAL) & client->revents){
		close(client->fd);
		client->events=0;
		return;
	}

	int len = read(client->fd, client_message_buffer, 8191);
	if(len<=0){
		close(client->fd);
		client->events=0;
		return;
	}

	client_message_buffer[len] = 0;
	process_client_event(client,client_message_buffer);

	//i don't know if this is requied, or if poll() resets all the revent flags.
	client->revents=0;
	//and i am not going to check.
}

int main(int argc, char** argv){
	int server_fd=-1;
	if(!process_args(argc,argv))
		return -1;
	if(use_ipv6)
		server_fd = start_server_ipv6(port);
	else
		server_fd = start_server_ipv4(port);
	if(server_fd < 0)
		return -1;

	if(static_connection_limit <= 0){
		max_connections=10;
		connections_list = calloc(10,sizeof(struct pollfd));
	}else{
		max_connections=static_connection_limit;
		connections_list = calloc(max_connections,sizeof(struct pollfd));
	}

	connections_list[0].fd=server_fd;
	connections_list[0].events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
	connections_list[1].fd=0;
	connections_list[1].events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;

	client_message_buffer = calloc(8192,1);

	while(shouldContinue){
		int num_fds = poll(connections_list,max_connections,-1);
		int checked_fds = 0;
		for(int i=0;i < (int) max_connections;i++){
			if(!connections_list[i].revents)
				continue;

			if(i==0)
				accept_new_client(connections_list[0]);
			else if(i==1)
				process_command();
			else
				process_client_data(&connections_list[i]);

			connections_list[i].revents=0;
			if(num_fds == checked_fds)
				break;
		}
	}
}
