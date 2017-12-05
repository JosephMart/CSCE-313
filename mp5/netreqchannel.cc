#include "netreqchannel.H"
#include <arpa/inet.h>
#include <errno.h>
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

#define BACKLOG 10         // how many pending connections queue will hold
#define MAXDATASIZE 100    // max number of bytes we can get at once

using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errno;
}
/* Client Constructor */
NetReqChannel::NetReqChannel(const std::string server_host_name, std::string port_no)
{
    // socket()
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(server_host_name.c_str(), port_no.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }

    // connect()
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(fd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo);    // all done with this structure
}

/* Server Constructor */
NetReqChannel::NetReqChannel(const std::string port_no, void *(*connection_handler)(void *) )
{
    int new_fd;    // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;    // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;    // use my IP
    if ((rv = getaddrinfo(NULL, port_no.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }
    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(fd);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);    // all done with this structure
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if (listen(fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    sa.sa_handler = sigchld_handler;    // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    printf("server: waiting for connections...\n");
    while (1) {
        sin_size = sizeof their_addr;
        new_fd   = accept(fd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
        if (!fork()) {    // this is the child process
            close(fd);    // child doesn't need the listener
            fd = new_fd;
            (*connection_handler)((void *) this);
            exit(0);
        }
        close(new_fd);    // parent doesn't need this
    }
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
