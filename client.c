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

    Thread threads[max_threads];

    args = malloc(sizeof(struct arg_struct) * 1);
    args->fd = cfd;

    printf("after allocation");

    Thread recthread = create_thread(receive_message, args, threads, max_threads);
    
    free(args);

    scanf_s("waiting");

    printf("\n");

    close(cfd);
    clean_threads(threads, max_threads);
    
    return 0;
}