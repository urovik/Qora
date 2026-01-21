#include "server.h"



int create_server(int port){

    struct sockaddr_in address;
    int new_socket;
    socklen_t addrlen = sizeof(address);

    int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if (socket_fd == -1)
    {
        perror("ошибка сервера");
        abort();
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(socket_fd,(struct sockaddr*) &address,sizeof(address)) < 0)
    {
        perror("bind failde");
        abort();
    }

    if(listen(socket_fd, 5) == -1)
    {
        perror("listen failed");
        abort();
    }

    if((new_socket = accept(socket_fd,(struct sockaddr*) &address, &addrlen)) == -1)
    {
        perror("accept failed");
        abort();
    }


}
