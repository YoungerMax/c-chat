// incl.

#include "common.c"
#include "config.h"

// decl.
const unsigned int max_threads = 2;

void* receive_message(void* data)
{
    struct recv_args* args = data;
    char buf[bufsize];

    for (;;)
    {
        int bytesread = read(r_args->fd, buf, bufsize);
        
        for (int i = 0; bytesread > i; i++)
        {
            printf("%c", buf[i]);
        }
    }

    return 0; // clang gives warnings if you don't return anything
}

void send_message(const char* msg, void* data)
{
    struct recv_args* r_args = data;

    send(r_args->fd, msg, strlen(msg), 0);
}

// defn.
Address get_addy_from_user()
{
    printf("Host address: ");
    const char* user_host = get_input(-1);

    printf("Host port: ");
    uint16_t user_port = atoi(get_input(5)); // TODO: make sure get_input is actually an int

    return create_addy(user_host, user_port, AF_INET);
}

int main()
{
    int cfd = create_socket();
    Address addy = get_addy_from_user();  // connect to address

    printf("Connecting to %s:%hu\n", addy.host, addy.addy_in.sin_port);

    // connect to the server.
    if (0 > connect(cfd, &addy.addy, addy.addysize))
    {
        printf("Failed to connect to %s:%hu\n", addy.host, addy.addy_in.sin_port);
        return -1;
    }

    printf("Connected to %s:%hu\n", addy.host, addy.addy_in.sin_port);

    Thread thread_arr[max_threads];

    r_args = malloc(sizeof(struct recv_args) * 1);
    r_args->fd = cfd;

    Thread rec_thread = create_thread(receive_message, r_args, thread_arr, max_threads);

    send_message("new message from the client", r_args);

    sleep(20); // obviously a placeholder, segmentation fault afterwards

    free(r_args);
    close(cfd);
    clean_threads(thread_arr, max_threads);
    
    return 0;
}