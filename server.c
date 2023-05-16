#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

int start_server_generic(int ipv,struct sockaddr* addr, socklen_t size){
    addr->sa_family = ipv;

    int ss_fd = socket(ipv, SOCK_STREAM, 0);
    if(ss_fd < 3){
        fprintf(stderr,"Failed to get socket file descriptor.\n");
        return -1;
    }

    int opt = 1;
    if(setsockopt(ss_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        fprintf(stderr, "Failed to setsockopt.\n");
        goto fail;
    }

    if(bind(ss_fd, addr, size)){
        fprintf(stderr, "Failed to bind socket, port may be in use.\n");
        goto fail;
    }

    if(listen(ss_fd, 10)){
        fprintf(stderr, "failed to listen on socket\n");
        goto fail;
    }

    return ss_fd;

    fail:
    close(ss_fd);
    return -1;
}

int start_server_ipv4(int port){

    struct sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    return start_server_generic(AF_INET, (struct sockaddr*)&addr, sizeof(addr));
}

int start_server_ipv6(int port){

    struct sockaddr_in6 addr;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(port);

    return start_server_generic(AF_INET6, (struct sockaddr*)&addr,sizeof(addr));
}
