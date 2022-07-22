// incl.
#include "common.c"

// decl.

void receive_message(void* data)
{
    struct arg_struct* args = data;
    const unsigned int bufsize = 1024;
    char buf[bufsize];

    for (;;)
    {
        ssize_t bytesread = read(args->fd, buf, bufsize);

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

    const unsigned int max_threads = 2;
    pthread_t threads[max_threads];

    args = malloc(sizeof(struct arg_struct) * 1);
    args->fd = cfd;

    printf("after allocation");

    pthread_t recthread = create_thread(receive_message, args, threads, max_threads);
    
    free(args);

    scanf("waiting");

    printf("\n");

    close(cfd);
    clean_threads(threads, max_threads);
    
    return 0;
}