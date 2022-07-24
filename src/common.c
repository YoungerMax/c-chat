// OS-Specific defns.
#ifdef __linux__ 
// OS is Linux
#define LINUX

#elif defined(__APPLE__)
// OS is macOS
#define MACOS

#elif defined(_WIN32) || defined(_WIN64)
// OS is Windows
#define WINDOWS

// Windows incl.
#include <winsock.h>
#include <processthreadsapi.h>
#include <synchapi.h>

// Windows defn.
#define close closesocket
void sleep(unsigned int seconds) { Sleep(seconds * 1000); }
int read(int fd, char* buf, int length) { return recv(fd, buf, length, 0); }

typedef LPDWORD thread_id_t;

#endif

// incl.
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// OS-Specific incl.
#if defined(LINUX) || defined(MACOS)

#include <sys/socket.h>
#include <netdb.h>
#include <err.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

typedef pthread_t thread_id_t;

#endif

//#define uint16_t unsigned short // dumb fix for now because max value of short is 65535

const unsigned int bufsize = 2048; // this runs out quick, error "broken pipe"

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
    void*(*target)(void*);
    void* args;
    void* native;
    thread_id_t threadId;
} Thread;

struct recv_args
{
    int fd;
} *r_args;

const char* get_input(int limit)
{
    int input_buf_size = 0;
    char* input_buf = malloc(sizeof(char) * input_buf_size);

    while (0 > limit || limit > input_buf_size)
    {
        const char next_char = getchar();

        if (next_char == '\n') {
            break;
        }

        input_buf_size++;
        input_buf = realloc(input_buf, input_buf_size);

        input_buf[input_buf_size-1] = next_char;
    }

    return input_buf;
}

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

Address create_addy(const char* host, uint16_t port, int family)
{
    // vars.
    struct sockaddr_in addy = {
        .sin_port = port,
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
    Thread thread;
    pthread_t thread_id;

    int err = pthread_create(&thread_id, NULL, func, args);

    if (err != 0)
    {
        printf("thread creation error");
        Thread err;
        return err;
        
        //TODO: proper error handling
    }

    thread.threadId = thread_id;

    //TODO: make this safe
    thread_arr[arr_size + 1] = thread;
    printf("thread %d created\n", arr_size+1);

    return thread;
}

void clean_threads(Thread thread_arr[], size_t arr_size)
{
    for (int i = 0; i < arr_size; i++)
    {
        pthread_join(*((pthread_t*) thread_arr[i].native), NULL);
    }
}

#elif defined(WINDOWS)

// SOCKETS //
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

Address create_addy(const char* host, uint16_t port, int family)
{
    // vars.
    struct sockaddr_in addy = {
        .sin_port = port,
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

// THREADING //
DWORD WINAPI win_thread_target(LPVOID p)
{
    Thread* data = (Thread*)p;
    data->target(data->args);

    return 0;
}

Thread create_thread(void*(*func)(void*), void* args, Thread thread_arr[], size_t arr_size)
{
    Thread thread = {
        .args = args,
        .target = func
    };
    
    HANDLE threadHandle = CreateThread(NULL, 0, win_thread_target, &thread, 0, thread.threadId);
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