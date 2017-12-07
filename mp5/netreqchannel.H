#ifndef _netreqchannel_H_
#define _netreqchannel_H_

#include <string>

class NetReqChannel
{
    private:
    int fd;

    public:
    /* Creates a CLIENT-SIDE local copy of the channel. The channel is connected
     * to the given port number at the given server host.
     * THIS CONSTRUCTOR IS CALLED BY THE CLIENT.
     */
    NetReqChannel(const std::string server_host_name, const std::string port_no);

    /* Creates a SERVER-SIDE local copy of the channel that is accepting connections
     * at the given port number.
     * NOTE that multiple clients can be connected to the same server-side end of the
     * request channel. Whenever a new connection comes in, it is accepted by the
     * the server, and the given connection handler is invoked. The parameter to
     * the connection handler is the file descriptor of the slave socket returned
     * by the accept call.
     * NOTE that the connection handler does not want to deal with
     * closing the socket. You will have to close the socket once the
     * connection handler is done.
     */
    NetReqChannel(const std::string port_no, const int backlog, void *(*connection_handler)(void *) );

    /* Destructor of the local copy of the channel. */
    ~NetReqChannel();

    /* Send a string over the channel and wait for a reply. */
    std::string send_request(std::string _request);

    /* Blocking read of data from the channel. Returns a string of characters
       read from the channel. Returns NULL if read failed. */
    std::string cread();

    /* Write the data to the channel. The function returns the number of
     * characters written to the channel.
     */
    int cwrite(std::string _msg);

    int read_fd();
};
#endif
