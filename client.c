// incl.
#include "common.c"
#include "config.h"

// decl.
const unsigned int max_threads = 2;

void receive_message(void* data)
{
    struct arg_struct* args = data;
    char buf[bufsize];

    for (;;)
    {
        int bytesread = read(args->fd, buf, bufsize);

        for (int i = 0; sizeof(buf) > i; i++)
        {
            printf("%c", buf[i]);
        }
    }
}

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

    // send message to server
    const char* buf = "hello from the client! :)";
    send(cfd, buf, strlen(buf), 0);

    close(cfd);
    
    return 0;
}