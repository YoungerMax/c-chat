// incl.
#include "common.c"

// decl.

// defn.
int main()
{
    int cfd = create_socket();
    Address addy = create_addy("127.0.0.1", 12345, AF_INET);  // connect to address

    // connect to the server.
    if (0 > connect(cfd, &addy.addy, addy.addysize))
    {
        return -1;
    }

    //TODO: figure out a way to do this
    // receive a message from the server.
    char buf[1024] = {};
    ssize_t bytesread = read(cfd, buf, 1024);

    // print out the received message.
    for (int i = 0; sizeof(buf) > i; i++) {
        printf("%c", buf[i]);
    }

    printf("\n");


    close(cfd);

    return 0;
}