// incl.
#include "common.c"

// decl.
int create_server_socket(Address *addy);

void send_message(void* data)
{
    int cfd = *((int*) data);
    const char* message = "message from the server";

    for (;;)
    {
        printf("hello from server loop");
        send(cfd, message, strlen(message), 0);
        sleep(1);
    }
}

// defn.
int main()
{
    //TODO: configuration file
    // vars.
    int sfd;
    Address addy = create_addy("127.0.0.1", 12345, AF_INET);  // temp.

    // insns.
    // create socket.
    if (0 > (sfd = create_server_socket(&addy))) {
        return sfd;  // this will return an error code
    }

    // listen for conns.
    listen(sfd, 5);

    // a loop. yes, this is a w̶h̶i̶l̶e̶ for loop. :)
    printf("Hosted on %s\n", addy.host);

    const unsigned int max_threads = 2;
    pthread_t threadarr[max_threads];

    for (;;) {
        // accept the new connection.
        int cfd = accept(sfd, &addy.addy, &addy.addysize);

        // send a message to the client.
        // Message message = {
        //     .content = "hello from the server! :)"
        // };

        pthread_t sendthread = create_thread(send_message, &cfd, threadarr, max_threads);

        // dc the client.
        close(cfd);
    }

    close(sfd);
    clean_threads(threadarr, max_threads);

    return 0;
}

int create_server_socket(Address *addy)
{
    // vars.
    int sfd;

    // insns.
    // create socket.
    if (0 > (sfd = create_socket())) {
        return sfd;  // this will return an error code
    }

    // bind.
    bind(sfd, &addy->addy, addy->addysize);
    
    return sfd;
}