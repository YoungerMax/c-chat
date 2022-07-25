// incl.

#include "common.c"
#include "config.h"

// decl.
const unsigned int max_threads = 2;

// const char* get_input(const char* prompt)
// {
//     printf(prompt);

//     char* buffer = malloc(sizeof(char) * bufsize);
//     fgets(buffer, bufsize, stdin);

//     return buffer;
// }

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

void connect_to_server(int cfd, Address addy)
{
    printf("Connecting to %s:%hu\n", addy.host, addy.addy_in.sin_port);

    if (0 > connect(cfd, &addy.addy, addy.addysize))
    {
        printf("Failed to connect to %s:%hu\n", addy.host, addy.addy_in.sin_port);
        abort();
    }

    printf("Connected to %s:%hu\n", addy.host, addy.addy_in.sin_port);
}

int main()
{
    int cfd = create_socket();
    Address addy = create_addy("127.0.0.1", 12345, AF_INET); // get_addy_from_user();  // connect to address

    connect_to_server(cfd, addy);

    Thread thread_arr[max_threads];

    r_args = malloc(sizeof(struct recv_args) * 1);
    r_args->fd = cfd;

    Thread rec_thread = create_thread(receive_message, r_args, thread_arr, max_threads);

    size_t tmp = bufsize;

    for (;;)  {

        //TODO: make all this work with get_input()
        printf(">> ");

        char message[bufsize];
        char* buffer = message;

        // warning: implicit declaration of function 'getline'
        size_t characters = getline(&buffer, &tmp, stdin);

        send_message(message, r_args);

    }

    sleep(20); // obviously a placeholder, segmentation fault afterwards

    free(r_args);
    close(cfd);
    clean_threads(thread_arr, max_threads);
    
    return 0;
}