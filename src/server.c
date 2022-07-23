// incl.
#include "common.c"
#include "config.h"
#include <signal.h>

// decl.
#define max_connections 10

const unsigned int max_threads = 10;
unsigned int keep_running = 1;

// TODO: don't have this as a global var.
volatile int connections[max_connections];

void detect_exit(int signal);
int create_server_socket(Address *addy);
void* send_message(void* data);
void* receive_message(void* data);
void* accept_connection(void* data);

struct connection_args
{
    int sfd;
    Address addy;
} *c_args;

// defn.
void detect_exit(int signal) { keep_running = 0; }

int main()
{
    // vars.
    int sfd;
    Address addy = create_addy(host, port, AF_INET);  // temp.

    // insns.
    // create exit handler
    signal(SIGINT, detect_exit);

    // create socket.
    if (0 > (sfd = create_server_socket(&addy))) {
        return sfd;  // this will return an error code
    }

    // listen for conns.
    listen(sfd, max_connections);

    printf("Hosted on %s:%d\n", addy.host, addy.addy_in.sin_port);

    Thread thread_arr[max_threads];

    // a loop. yes, this is a w̶h̶i̶l̶e̶ for loop. :)
    //TODO: make different tasks split up across different loops in different threads rather than one big loop

    c_args = malloc(sizeof(struct connection_args) * 1);
    c_args->sfd = sfd;
    c_args->addy = addy;

    Thread accept_thread = create_thread(accept_connection, c_args, thread_arr, max_threads);

    // for (;;) {
    //     args = malloc(sizeof(struct recv_args) * 1);
    //     args->fd = cfd;

    //     Thread rec_thread = create_thread(receive_message, args, thread_arr, max_threads);

    //     // dc the client.
    //     close(cfd);
    // }

    while (keep_running) {
        fflush(stdout);
        sleep(1); // sleep to prevent cpu from burning up
    }

    // shutdown
    printf("Shutting server down...\n");

    free(c_args);
    close(sfd);
    clean_threads(thread_arr, max_threads);

    return 0;
}

void* accept_connection(void* data)
{
    struct connection_args* c_args = data;
    
    // needed args
    // sfd, addy, cfd array, cfd array size

    for (;;)
    {
        int cfd;

        size_t connect_size = sizeof(connections)/sizeof(connections[0]);
        
        if (0 > (cfd = accept(c_args->sfd, &c_args->addy.addy,  &c_args->addy.addysize))) {
            printf("Could not accept connection %d\n", connect_size+1);
            continue;
        }
        
        connections[connect_size+1] = cfd;

        printf("accepted new connection %d\n", connect_size+1);
    }

    return 0;
}

void* send_message(void* data)
{
    struct recv_args* args = data;
    const char* message = "message from the server";

    send(args->fd, message, strlen(message), 0);

    return 0;
}

void* receive_message(void* data)
{
    struct recv_args* args = data;

    char buf[bufsize];
    int bytesread = read(args->fd, buf, bufsize);

    for (int i = 0; bytesread > i; i++) {
        printf("%c", buf[i]);
    }

    printf("\ntotal read bytes: %d\n", bytesread);

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
