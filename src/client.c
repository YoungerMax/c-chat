// incl.

#include "common.c"
#include "config.h"

// decl.
const unsigned int max_threads = 2;
unsigned int keep_running = 1;

void* receive_message(void* data);
void send_message(const char* msg, void* data);
address_t get_addy_from_user(void);
void connect_to_server(int cfd, address_t addy);
void on_sigint(int signal) { keep_running = 0; }

struct client_recv_args
{
    int cfd;
} *cr_args;

int main()
{
    int cfd = create_socket();
    address_t addy = create_addy("127.0.0.1", 12345, AF_INET); // get_addy_from_user();  // connect to address
    signal(SIGINT, on_sigint);

    connect_to_server(cfd, addy);

    thread_t thread_arr[max_threads];

    cr_args = malloc(sizeof(struct client_recv_args) * 1);
    cr_args->cfd = cfd;

    thread_t rec_thread = create_thread(receive_message, cr_args, thread_arr, max_threads);

    size_t tmp = bufsize;

    while (keep_running)  {

        //TODO: make all this work with get_input()
        printf(">> ");

        char message[bufsize];
        char* buffer = message;

        // warning: implicit declaration of function 'getline'
        size_t characters = getline(&buffer, &tmp, stdin);

        send_message(message, r_args);
    }
    
    close(cfd);
    clean_threads(thread_arr, max_threads);
    free(r_args);
    
    return 0;
}

void* receive_message(void* data)
{
    struct client_recv_args* args = data;
    char buf[bufsize];

    r_args = malloc(sizeof(struct recv_args) * 1);

    for (;;)
    {
        int bytesread = read(args->cfd, r_args, sizeof(r_args));

        // print out the buffer, un-ugly this
        printf("\n%s:", r_args->user.name);
        for (int i = 0; bytesread > i; i++) printf("%c", buf[i]);
        printf("\n");

        fflush(stdout);
    }

    free(r_args);
    return 0;
}

void send_message(const char* msg, void* data)
{
    struct recv_args* r_args = data;

    send(r_args->user.cfd, msg, strlen(msg), 0);
}

// defn.
address_t get_addy_from_user(void)
{
    //TODO: check that address and port are actually valid addresses and ports
    const char* user_host;
    const char* user_port_cs;

    printf("Host address: ");
    scanf("%15s", user_host);

    printf("Host port: ");
    scanf("%5s", user_port_cs);

    uint16_t user_port = atoi(user_port_cs); // TODO: make sure get_input is actually an int

    return create_addy(user_host, user_port, AF_INET);
}

void connect_to_server(int cfd, address_t addy)
{
    printf("Connecting to %s:%hu\n", addy.host, addy.addy_in.sin_port);

    if (0 > connect(cfd, &addy.addy, addy.addysize))
    {
        printf("Failed to connect to %s:%hu\n", addy.host, addy.addy_in.sin_port);
        abort();
    }

    printf("Connected to %s:%hu\n", addy.host, addy.addy_in.sin_port);
}