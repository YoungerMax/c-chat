// incl.
#include "common.c"
#include "config.h"

// decl.
const unsigned int max_threads = 10;

int create_server_socket(Address *addy);
void send_message(void* data);

// defn.
int main()
{
    //TODO: configuration file
    // vars.
    int sfd;
    Address addy = create_addy(host, port, AF_INET);  // temp.

    // insns.
    // create socket.
    if (0 > (sfd = create_server_socket(&addy))) {
        return sfd;  // this will return an error code
    }

    // listen for conns.
    listen(sfd, 5);

    // a loop. yes, this is a w̶h̶i̶l̶e̶ for loop. :)
    printf("Hosted on %s\n", addy.host);

    Thread threadarr[max_threads];

    for (;;) {
        // accept the new connection.
        int cfd = accept(sfd, &addy.addy, &addy.addysize);

        args = malloc(sizeof(struct arg_struct) * 1);
        args->fd = cfd;

        Thread sendthread = create_thread(send_message, args, threadarr, max_threads);
        
        free(args);
        // dc the client.
        sleep(10);
        close(cfd);
    }

    close(sfd);
    clean_threads(threadarr, max_threads);

    return 0;
}

int create_server_socket(Address *addy)
{
    // vars.
    int sfd;

    // insns.
    // create socket.
    if (0 > (sfd = create_socket())) {
        return sfd;  // this will return an error code
    }

    // bind.
    bind(sfd, &addy->addy, addy->addysize);
    
    return sfd;
}

void send_message(void* data)
{
    struct arg_struct* args = data;

    const char* message = "message from the server";

    for (;;)
    {
        printf("hello from server loop");
        send(args->fd, message, strlen(message), 0);
        sleep(1);
    }
}