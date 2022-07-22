// incl.
#include "common.c"

// decl.
int create_server_socket(struct sockaddr* addy);

// defn.
int main()
{
    //TODO: configuration file
    // vars.
    int sfd;
    struct sockaddr addy = create_addy("127.0.0.1", 12345, AF_INET);
    socklen_t addysize = sizeof(addy);

    // insns.
    // create socket.
    if (0 > (sfd = create_server_socket(&addy))) {
        printf("error: could not create & bind socket");
        return -1;
    }

    // listen for conns.
    listen(sfd, 5);

    // a loop. yes, this is a  ̶w̶h̶i̶l̶e̶ for loop. :)
    inet_pton(AF_INET, )
    printf("Hosted on %s\n", )

    for (;;) {
        int cfd = accept(sfd, &addy, &addysize);

        // temp.
        struct sockaddr_in c_addy;
        getpeername(cfd, (struct sockaddr*) &c_addy, &addysize);

        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &c_addy.sin_addr, str, INET_ADDRSTRLEN);
        printf(str);

        Message message = {
            .content = "hello from the server! :)"
        };

        send(cfd, message, sizeof(message), 0);
        // int __fd, const void *__buf, size_t __n, int __flags

        close(cfd);
    }
}

int create_server_socket(struct sockaddr* addy)
{
    // vars.
    int sfd;

    // insns.
    // create socket.
    if (0 > (sfd = create_socket())) {
        return sfd;
    }

    // bind.
    bind(sfd, addy, sizeof(addy));
    
    return sfd;
}