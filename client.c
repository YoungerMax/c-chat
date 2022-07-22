#include <string.h>

#include "common.c"



// int connect(Server server)
// {
//     int sfd = create_socket();
// }

void receive() {
    for (;;)
    {
        printf("check");
    };
}

int main()
{
    int cfd = create_socket();

    struct sockaddr addy = create_addy("127.0.0.1", 12345, AF_INET);

    if (0 > connect(cfd, &addy, sizeof(addy)))
    {
        printf("connection failed");
        return -1;
    }

    //TODO: figure out a way to do this
    Message message = {
        .content = "message from the client :)"
    };
    send(cfd, message, strlen(message), 0);

    char buf[1024] = { 0 };
    ssize_t valread = read(cfd, buf, 1024);

    for (int i = 0; sizeof(buf) / sizeof(char) > i; i++) {
        printf("%s", buf[i]);
    }

    printf("\n");

    // pthread_t rec_thread = create_thread(receive);

    close(cfd);
    return 0;
}