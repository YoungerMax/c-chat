#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __linux__ 

#define LINUX

#include <sys/socket.h>
#include <netdb.h>
#include <err.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#elif defined(_WIN32) || defined(_WIN64)

#define WINDOWS

#include <winsock.h>
#include <processthreadsapi.h>
#include <synchapi.h>

#define close closesocket

void sleep(unsigned int seconds) { Sleep(seconds * 1000); }
int read(int fd, char* buf, int length) { return recv(fd, buf, length, 0); }

#elif defined(__APPLE__)

#define MACOS

#endif

//#define uint16_t unsigned short // dumb fix for now because max value of short is 65535

const unsigned int bufsize = 1024;

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
    int addysize;
} Address;

typedef struct
{
    void* (*target)(void*);
    void* args;
    int threadId;
    void* native;
} Thread;

struct arg_struct
{
    int fd;
} *args;

#if defined(LINUX) || defined(MACOS)

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
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*)1, sizeof(int));

    return sfd;
}

Address create_addy(const char* host, int port, int family)
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
        .addy = *((struct sockaddr*)&addy),  // hack for casting to sockaddr.
        .addy_in = addy,
        .host = host
    };

    address.addysize = sizeof(address.addy);

    return address;
}

Thread create_thread(void*(*func)(void*), void* args, Thread thread_arr[], size_t arr_size)
{
    Thread thread
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

void clean_threads(Thread thread_arr[], size_t arr_size)
{
    for (int i = 0; i < arr_size; i++)
    {
        pthread_join(thread_arr[i], NULL);
    }
}

#elif defined(WINDOWS)

int create_socket()
{
    // check if winsock is initialized.
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
        return -1;
    }

    // vars.
    int sfd;

    // insns.
    // init socket.
    if (0 > (sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) {
        WSACleanup();
        return sfd;  // return the error code
    }

    // enable address reuse.
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void*)1, sizeof(int));

    return sfd;
}

Address create_addy(const char* host, int port, int family)
{
    // vars.
    struct sockaddr_in addy = {
        .sin_port = htons(port),
        .sin_family = family,
        .sin_addr.s_addr = inet_addr(host)
    };

    // vars.
    Address address = {
        .addy = *((struct sockaddr*)&addy),  // hack for casting to sockaddr.
        .addy_in = addy,
        .host = host
    };

    address.addysize = sizeof(address.addy);

    return address;
}

Thread create_thread(void* (*func)(void*), void* args, Thread thread_arr[], size_t arr_size)
{
    Thread thread = {
        .args = args,
        .target = func
    };
    
    HANDLE threadHandle = CreateThread(NULL, 0, func, args, 0, &thread.threadId);
    thread.native = (void*)&threadHandle;

    thread_arr[arr_size + 1] = thread;

    return thread;
}

void clean_threads(Thread thread_arr[], size_t arr_size)
{
    for (int i = 0; i < arr_size; i++) {
        WaitForSingleObject(*(HANDLE*) thread_arr[i].native, INFINITE);
    }
}

#endif