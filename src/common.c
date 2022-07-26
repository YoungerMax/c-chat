// Misc defns.
#define bufsize 4096 // this runs out quick, error "broken pipe"

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
#include <stdlib.h>

// OS-Specific incl.
#if defined(LINUX) || defined(MACOS)

#include <sys/socket.h>
#include <netdb.h>
#include <err.h> //TODO: use
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

typedef pthread_t thread_id_t;

#endif

typedef struct
{
    const char* name;
    //TODO: implement more here
} user_t;

typedef struct
{
    const char* content;

    // timestamp, sender
} message_t;

typedef struct
{
    struct sockaddr addy;
    struct sockaddr_in addy_in;
    const char* host;
    int addysize;
} address_t;

typedef struct
{
    void*(*target)(void*);
    void* args;
    void* native;
    thread_id_t threadId;
} thread_t;

struct recv_args
{
    int fd;
} *r_args;

//TODO: refactor
const char* get_input(int limit, const char* prompt)
{
    printf(prompt);
    fflush(stdout);

    int input_buf_size = 0;
    char* input_buf = malloc(sizeof(char) * input_buf_size);

    while (0 > limit || limit > input_buf_size)
    {
        printf("gello");
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

void print_array(void* array[], size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%d", array[i]);
    }
    printf("\n");
}


// currently for ints only
size_t count_array(void* array[], size_t size)
{
    size_t count;
    for (int i = 0; i < size; i++)
    {
        if (!array[i]) count++;
    }

    printf("%d, ", count);
    print_array(array, size);

    return count;
}

#if defined(LINUX) || defined(MACOS)

int create_socket(void)
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

address_t create_addy(const char* host, uint16_t port, int family)
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
    address_t address = {
        .addy = *((struct sockaddr*)&addy),  // hack for casting to sockaddr.
        .addy_in = addy,
        .host = host
    };

    address.addysize = sizeof(address.addy);

    return address;
}

thread_t create_thread(void*(*func)(void*), void* args, thread_t thread_arr[], size_t arr_size)
{
    thread_t thread;
    pthread_t thread_id;

    int err = pthread_create(&thread_id, NULL, func, args);

    size_t thread_count = count_array(thread_arr, arr_size);

    if (err != 0)
    {
        printf("thread creation error");
        thread_t err;
        return err;
        
        //TODO: proper error handling
    }

    thread.threadId = thread_id;

    //TODO: make this safe
    thread_arr[thread_count] = thread;
    printf("thread %d created\n", thread_count);

    return thread;
}

void clean_threads(thread_t thread_arr[], size_t arr_size)
{
    for (int i = 0; i < arr_size; i++)
    {
        pthread_join(*((pthread_t*) thread_arr[i].native), NULL);
    }
}

#elif defined(WINDOWS)

// SOCKETS //
int create_socket(void)
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

address_t create_addy(const char* host, uint16_t port, int family)
{
    // vars.
    struct sockaddr_in addy = {
        .sin_port = port,
        .sin_family = family,
        .sin_addr.s_addr = inet_addr(host)
    };

    // vars.
    address_t address = {
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
    thread_t* data = (thread_t*)p;
    data->target(data->args);

    return 0;
}

thread_t create_thread(void*(*func)(void*), void* args, thread_t thread_arr[], size_t arr_size)
{
    thread_t thread = {
        .args = args,
        .target = func
    };
    
    HANDLE threadHandle = CreateThread(NULL, 0, win_thread_target, &thread, 0, thread.threadId);
    thread.native = (void*)&threadHandle;

    thread_arr[arr_size + 1] = thread;

    return thread;
}

void clean_threads(thread_t thread_arr[], size_t arr_size)
{
    for (int i = 0; i < arr_size; i++) {
        WaitForSingleObject(*(HANDLE*) thread_arr[i].native, INFINITE);
    }
}

#endif