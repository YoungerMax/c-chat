// incl.
#include "common.c"

// decl.
int create_server_socket(Address *addy);

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

    for (;;) {
        // accept the new connection.
        int cfd = accept(sfd, &addy.addy, &addy.addysize);

        // send a message to the client.
        Message message = {
            .content = "hello from the server! :)"
        };

        send(cfd, message.content, strlen(message.content), 0);

        // dc the client.
        close(cfd);
    }
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