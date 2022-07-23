// incl.

#include "common.c"
#include "config.h"

// decl.
const unsigned int max_threads = 2;

void* receive_message(void* data)
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

void* send_message(void* data)
{
    struct arg_struct* args = data;
    const char* message = "message from the client";

    send(args->fd, message, strlen(message), 0);
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

    args = malloc(sizeof(struct arg_struct) * 1);
    args->fd = cfd;

    Thread rec_thread = create_thread(receive_message, args, thread_arr, max_threads);

    send_message(args);

    sleep(20); // obviously a placeholder, segmentation fault afterwards

    free(args);
    close(cfd);
    clean_threads(thread_arr, max_threads);
    
    return 0;
}