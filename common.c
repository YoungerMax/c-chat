#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <err.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

//#define uint16_t unsigned short // dumb fix for now because max value of short is 65535

typedef struct
{
    const char* name;
    //TODO: implement more here
} User;

typedef struct
{
    const char* content;
} Message;

typedef struct
{
    struct sockaddr addy;
    struct sockaddr_in addy_in;
    const char* host;
    socklen_t addysize;
} Address;

struct arg_struct
{
    int fd;
} *args;

int create_socket()
{
    // vars.
    int sfd;

    // insns.
    // init socket.
    if (0 > (sfd = socket(AF_INET, SOCK_STREAM, 0))) {
        return sfd;  // return the error code
    }
    
    // enable address reuse.
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*) 1, sizeof(int));
    
    return sfd;
}

Address create_addy(const char* host, in_port_t port, sa_family_t family)
{
    // vars.
    struct sockaddr_in addy = {
        .sin_port = htons(port),
        .sin_family = family
    };

    // convert ip.
    // TODO: handle error properly
    if (0 > inet_pton(AF_INET, host, &addy.sin_addr)) {
        printf("debug :: bad addy\n");
    }
    
    // vars.
    Address address = {
        .addy = *((struct sockaddr*) &addy),  // hack for casting to sockaddr.
        .addy_in = addy,
        .host = host
    };

    address.addysize = sizeof(address.addy);

    return address;
}

pthread_t create_thread(void*(*func)(void*), void* args, pthread_t thread_arr[], size_t arr_size)
{
    pthread_t thread;
    int err;

    err = pthread_create(&thread, NULL, func, args);
    if (err)
    {
        printf("thread creation error");
        return -1;
    }

    //TODO: make this safe
    thread_arr[arr_size + 1] = thread;

    return thread;
}

void clean_threads(pthread_t thread_arr[], size_t arr_size)
{
    for (int i = 0; i < arr_size; i++)
    {
        pthread_join(thread_arr[i], NULL);
    }
}