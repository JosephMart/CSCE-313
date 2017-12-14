#include "netreqchannel.H"
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define BACKLOG 10         // how many pending connections queue will hold
#define MAXDATASIZE 100    // max number of bytes we can get at once
struct sockaddr_in server_socket_in;
static int serverSize = sizeof(server_socket_in);

using namespace std;

/* Client Constructor */
NetReqChannel::NetReqChannel(const std::string server_host_name, std::string port_no)
{
    // socket();
    memset(&server_socket_in, 0, serverSize);
    server_socket_in.sin_family = AF_INET;

    if (struct servent *pse = getservbyname(port_no.c_str(), "tcp")) {
        server_socket_in.sin_port = pse->s_port;
    } else if ((server_socket_in.sin_port = htons((unsigned short) atoi(port_no.c_str()))) == 0) {
        cerr << "cant connect port\n";
    }

    if (struct hostent *h = gethostbyname(server_host_name.c_str())) {
        memcpy(&server_socket_in.sin_addr, h->h_addr, h->h_length);
    } else if ((server_socket_in.sin_addr.s_addr = inet_addr(server_host_name.c_str())) == INADDR_NONE) {
        cerr << "cant determine host <" << server_host_name << ">\n";
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        cerr << "cant create socket\n";
    }

    if (connect(fd, (struct sockaddr *) &server_socket_in, serverSize) < 0) {
        cerr << "cant connect to " << server_host_name << ":" << port_no;
    }
}

/* Server Constructor */
NetReqChannel::NetReqChannel(const std::string port_no, const int backlog, void *(*connection_handler)(void *) )
{
    memset(&server_socket_in, 0, sizeof(server_socket_in));
    server_socket_in.sin_family      = AF_INET;
    server_socket_in.sin_addr.s_addr = INADDR_ANY;

    if (struct servent *pse = getservbyname(port_no.c_str(), "tcp")) {
        server_socket_in.sin_port = pse->s_port;
    } else if ((server_socket_in.sin_port = htons((unsigned short) atoi(port_no.c_str()))) == 0) {
        cerr << "Not able to Connect to port\n";
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Not able to Create Socket\n";
    }

    if (bind(fd, (struct sockaddr *) &server_socket_in, sizeof(server_socket_in)) < 0) {
        cerr << "Not able to Bind\n";
    }

    listen(fd, backlog);

    for (;;) {
        pthread_t thread;
        int *fn = new int;

        *fn = accept(fd, (struct sockaddr *) &server_socket_in, (socklen_t *) &serverSize);

        if (!fn) {
            delete fn;
            if (errno == EINTR) {
                continue;
            } else {
                cerr << "UNKOWN ERRROR\n";
            }
        }
        pthread_create(&thread, NULL, connection_handler, (void *) fn);
    }
    cout << "Connection setup completed\n";
}

NetReqChannel::~NetReqChannel()
{
    close(fd);
}

/* Send a string over the channel and wait for a reply. */
std::string NetReqChannel::send_request(std::string _request)
{
    cwrite(_request);
    return cread();
}

/* Blocking read of data from the channel. Returns a string of characters
   read from the channel. Returns NULL if read failed. */
std::string NetReqChannel::cread()
{
    int numbytes;
    char buf[MAXDATASIZE];
    if ((numbytes = recv(this->fd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    return buf;
}

/* Write the data to the channel. The function returns the number of
 * characters written to the channel.
 */
int NetReqChannel::cwrite(std::string _msg)
{
    if (send(fd, _msg.c_str(), strlen(_msg.c_str()) + 1, 0) == -1)
        perror("send");
    return 0;
}

int NetReqChannel::read_fd()
{
    return fd;
}
