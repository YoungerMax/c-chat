// incl.
#include "common.c"
#include "config.h"

// decl.
#define max_connections 10
#define max_threads 10

// const unsigned int max_threads = 10;
unsigned int keep_running = 1;

// TODO: don't have these as a global var.
volatile user_t connections[max_connections];
thread_t thread_arr[max_threads];

void detect_exit(int signal);
int create_server_socket(address_t *addy);
void send_message(struct recv_args* args);
void* receive_message(void* data);
void* accept_connection(void* data);
user_t create_user(int cfd);
void on_disconnect(user_t user);

struct connection_args
{
    int sfd;
    address_t addy;
} *c_args;

// defn.
void detect_exit(int signal) { keep_running = 0; }

int main()
{
    // vars.
    int sfd;
    address_t addy = create_addy(host, port, AF_INET);  // temp.

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

    c_args = malloc(sizeof(struct connection_args) * 1);
    c_args->sfd = sfd;
    c_args->addy = addy;

    thread_t accept_thread = create_thread(accept_connection, c_args, thread_arr, max_threads);

    while (keep_running) {
        fflush(stdout);
        sleep(1); // sleep to prevent cpu from burning up
    }

    // shutdown
    printf("Shutting server down...\n");

    close(sfd);
    clean_threads(thread_arr, max_threads);
    free(c_args);

    return 0;
}

void* accept_connection(void* data)
{
    struct connection_args* c_args = data;

    for (;;)
    {
        int cfd;

        size_t connect_size = sizeof(connections)/sizeof(connections[0]);
        cfd = accept(c_args->sfd, &c_args->addy.addy,  &c_args->addy.addysize);

        if (0 > cfd) {
            printf("Could not accept connection %d\n", connect_size+1);
            continue;
        }

        user_t user = create_user(cfd);

        r_args = malloc(sizeof(struct recv_args) * 1);
        r_args->user = user;
        r_args->sfd = c_args->sfd;

        thread_t client_thread = create_thread(receive_message, r_args, thread_arr, max_threads);

        printf("accepted new connection %d\n", user.id);
    }

    return 0;
}

void* receive_message(void* data)
{
    struct recv_args* args = data;

    for (;;)
    {
        char buf[bufsize];

        int bytesread = read(args->user.cfd, buf, bufsize);

        // check if client has dc'd
        if (bytesread == 0) break;
    
        for (int i = 0; bytesread > i; i++) {
            printf("%c", buf[i]);
        }

        args->message = buf;

        send_message(args);
    }

    // de-allocate thread

    on_disconnect(args->user);

    return 0;
}

//void send_message(const char* msg, user_t from_user)
void send_message(struct recv_args* args)
{
    size_t connect_size = sizeof(connections)/sizeof(connections[0]);
    size_t connect_count = count_users(connections, connect_size, "send_message");

    for (size_t i = 0; i < count_users(connections, connect_size, "send_message"); i++)
    {
        // don't send message back to original client
        if (connections[i].cfd == args->user.cfd) continue;

        send(connections[i].cfd, args, strlen(args->message), 0);
    }
}

user_t create_user(int cfd)
{
    size_t connect_size = sizeof(connections)/sizeof(connections[0]);
    size_t connect_count = count_users(connections, connect_size, "create_user");

    user_t user = {
        .cfd = cfd,
        .id = connect_count,
        .name = "user"
    };

    connections[connect_count] = user;
}

void on_disconnect(user_t user)
{
    printf("client %d has disconnected from the server\n", user.id);
    
    //TODO: deallocate from connections array
}

//TODO: implement delete thread method that deallocated thread from thread array

int create_server_socket(address_t *addy)
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
