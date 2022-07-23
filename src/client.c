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

void* send_message(void* data)
{
    struct recv_args* r_args = data;
    const char* message = "message from the client";

    send(r_args->fd, message, strlen(message), 0);

    return 0; // clang gives warnings if you don't return anything
}

// defn.
int main()
{
    int cfd = create_socket();
    Address addy = create_addy(host, port, AF_INET);  // connect to address

    // connect to the server.
    if (0 > connect(cfd, &addy.addy, addy.addysize))
    {
        return -1;
    }

    printf("Connected to %s:%d\n", addy.host, addy.addy_in.sin_port);

    Thread thread_arr[max_threads];

    r_args = malloc(sizeof(struct recv_args) * 1);
    r_args->fd = cfd;

    Thread rec_thread = create_thread(receive_message, r_args, thread_arr, max_threads);

    send_message(r_args);

    sleep(20); // obviously a placeholder, segmentation fault afterwards

    free(r_args);
    close(cfd);
    clean_threads(thread_arr, max_threads);
    
    return 0;
}